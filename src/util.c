#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>

void err(char * str) {
    perror(str);
    exit(1);
}

void trap(int sig, __sighandler_t handler) {
    struct sigaction act;
    bzero(&act, sizeof(act));
    act.sa_handler = handler;
    sigaction(sig, &act, NULL);
}