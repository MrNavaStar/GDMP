#define _POSIX_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pty.h>
#include <termios.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "include/tty.h"

int fd;

void err(char * str) {
    printf("%s\n", str);
    exit(1);
}

void sigint(int sig) {
    write(fd, "stop", 4);
}

void io_handler(int pty) {
    pid_t pid = fork();
    if (pid < 0) err("Failed to create io process.");

    int n;
    char buf[512];
    if (pid == 0) {
        for (;;) {
            if ((n = read(STDIN_FILENO, buf, sizeof(buf))) < 0)
                err("Read error from stdin.");
            if (write(pty, buf, n) != n)
                err("Writen error to master pty.");
        }
    }

    for (;;) {
        if ((n = read(pty, buf, sizeof(buf))) <= 0)
            break; /* signal caught, error, or EOF */
        if (write(STDOUT_FILENO, buf, n) != n)
            err("Writen error to stdout.");
    }

    kill(pid, SIGKILL);
}

int main(int argc, char *argv[]) {
    int pid;
    struct termios orig_termios;
    struct winsize size;

    if (isatty(STDIN_FILENO)) {
        tcgetattr(STDIN_FILENO, &orig_termios);
        ioctl(STDIN_FILENO, TIOCGWINSZ, (char *) &size);
        pid = forkpty(&fd, NULL, &orig_termios, &size);
    } else {
        pid = forkpty(&fd, NULL, NULL, NULL);
    }

    if (pid < 0) err("Failed to create child process.");
    if (pid == 0) {
        execvp(argv[1], argv + 2);
        err("Failed to run command.");
    }

    signal(SIGINT, sigint);

    tty_raw(STDIN_FILENO);
    atexit(tty_atexit);

    io_handler(fd);
}