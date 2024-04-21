#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pty.h>
#include <termios.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "include/util.h"
#include "include/args.h"
#include "include/tty.h"

sig_atomic_t fd;
int child_pid;
Signal signals[32];

void sig_handler(int sig) {
    Signal signal = signals[sig];

    if (!signal.block) {
        if (sig == SIGWINCH) {
            struct winsize size;
            ioctl(STDIN_FILENO, TIOCGWINSZ, (char *) &size);
            ioctl(fd, TIOCSWINSZ, (char *) &size);
        }
        //else kill(child_pid, sig);
    }
    if (signal.mode == 2) write(fd, signal.text, strlen(signal.text));
    else if (signal.mode == 3) {
        //TODO: Run text as bash script
    }
}

void io_handler(int pty) {
    pid_t pid = fork();
    if (pid < 0) err("Failed to create io process.");

    int n;
    char buf[512];
    if (pid == 0) {
        // SIGWINCH needs to be registered by default for resizing to work
        trap(SIGWINCH, sig_handler);

        for (int i = 0; i < 32; i++) {
            if (signals[i].sig) {
                printf("PAIN: %d", signals[i].sig);
                trap(signals[i].sig, sig_handler);
            }
        }

        for (;;) {
            n = read(STDIN_FILENO, buf, sizeof(buf));
            if (write(pty, buf, n) != n) err("Writen error to master pty.");
        }
    }

    for (;;) {
        if ((n = read(pty, buf, sizeof(buf))) <= 0) break;
        if (write(STDOUT_FILENO, buf, n) != n) err("Writen error to stdout.");
    }

    kill(pid, SIGKILL);
}

int main(int argc, char *argv[]) {
    int cmd_offset = parse_args(argc, argv, signals);

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
}