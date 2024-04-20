#define _POSIX_SOURCE
#include <stdlib.h>
#include <unistd.h>
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

void sigint(int sig) {
    write(fd, "stop", 4);
}

void sigwinch(int sig) {
    struct winsize size;
    ioctl(STDIN_FILENO, TIOCGWINSZ, (char *) &size);
    ioctl(fd, TIOCSWINSZ, (char *) &size);
}

void io_handler(int pty) {
    pid_t pid = fork();
    if (pid < 0) err("Failed to create io process.");

    int n;
    char buf[512];
    if (pid == 0) {

        trap(SIGINT, sigint);
        trap(SIGWINCH, sigwinch);

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
    parse_args(argc, argv);

    /*struct winsize size;
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
        execvp(argv[1], argv + 2);
        err("Failed to run command.");
    }

    tty_raw(STDIN_FILENO);
    atexit(tty_atexit);

    io_handler(fd);*/
}