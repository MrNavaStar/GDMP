#define _POSIX_SOURCE
#define __USE_POSIX199309
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
    sigemptyset(&act.sa_mask);
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigaction(sig, &act, NULL);
}