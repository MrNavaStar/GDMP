#define _POSIX_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pty.h>
#include <termios.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include "include/util.h"
#include "include/args.h"
#include "include/tty.h"

int fd;
int child_pid;
Signal signals[31];

static void sig_handler(int sig) {
    trap(sig, sig_handler);
    Signal signal = signals[sig - 1];

    if (!signal.block) {
        if (sig == SIGWINCH) {
            struct winsize size;
            ioctl(STDIN_FILENO, TIOCGWINSZ, (char *) &size);
            ioctl(fd, TIOCSWINSZ, (char *) &size);
        }

        // TODO: Make signal forwarding actually work
        else {
            #ifdef TIOCSIGNAL
            ioctl(fd, TIOCSIGNAL, sig);
            #endif
            #ifdef TIOCSIG
            ioctl(fd, TIOCSIG, sig);
            #endif
        }
    }
    if (signal.mode == 2) {
        write(fd, signal.text, strlen(signal.text));
        write(fd, "\n", 1);
    }

    /*else if (signal.mode == 3) {
        //TODO: Run text as bash script
    }*/
}

void io_handler(int pty) {
    pid_t reader_pid = fork();
    if (reader_pid < 0) err("Failed to create io process.");

    int n;
    char buf[512];
    if (reader_pid == 0) {

        // Block all signals on this process
        sigset_t mask;
        sigfillset(&mask);
        sigprocmask(SIG_BLOCK, &mask, NULL);

        for (;;) {
            n = read(STDIN_FILENO, buf, 1);
            int sig = get_sig_from_code(buf[0]);
            if (sig != 0) sig_handler(sig);
            else if (write(pty, buf, n) != n) err("Writen error to master pty.");
        }
    }

    for (int i = 1; i < 32; i++) trap(i, sig_handler);

    while (waitpid(child_pid, NULL, WNOHANG) == 0) {
        if ((n = read(pty, buf, sizeof(buf))) <= 0) continue;
        if (write(STDOUT_FILENO, buf, n) != n) err("Writen error to stdout.");
    }

    kill(reader_pid, SIGKILL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 0;
    int cmd_offset = parse_args(argc, argv, signals);
    if (cmd_offset == 0) return 0;

    struct winsize size;
    struct termios orig_termios;

    if (isatty(STDIN_FILENO)) {
        tcgetattr(STDIN_FILENO, &orig_termios);
        ioctl(STDIN_FILENO, TIOCGWINSZ, (char *) &size);
        child_pid = forkpty(&fd, NULL, &orig_termios, &size);
    } else {
        child_pid = forkpty(&fd, NULL, NULL, NULL);
    }

    if (child_pid < 0) err("Failed to create child process.");
    if (child_pid == 0) {
        execvp(argv[cmd_offset], argv + cmd_offset);
        err("Failed to run command.");
    }

    tty_raw(STDIN_FILENO);
    atexit(tty_atexit);
    io_handler(fd);
    return 0;
}