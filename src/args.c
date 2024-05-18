#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>

#include "include/args.h"

int get_type(char *str) {
    if (str[0] == '-' && str[1] == '-') return 0;
    if (strcmp(str, "-x") == 0) return 1;
    if (strcmp(str, "-w") == 0) return 2;
    if (strcmp(str, "-s") == 0) return 3;
    if (strchr("abcdefghijklmnopqrstuvwxyz./[{`\"", str[0]) == NULL) return 4;
    return 5;
}

int get_sig(char *str) {
    if (strcmp(str, "hup") == 0) return SIGHUP;
    if (strcmp(str, "int") == 0) return SIGINT;
    if (strcmp(str, "quit") == 0) return SIGQUIT;
    if (strcmp(str, "ill") == 0) return SIGILL;
    if (strcmp(str, "trap") == 0) return SIGTRAP;
    if (strcmp(str, "abrt") == 0) return SIGABRT;
    if (strcmp(str, "iot") == 0) return SIGIOT;
    if (strcmp(str, "bus") == 0) return SIGBUS;
    if (strcmp(str, "fpe") == 0) return SIGFPE;
    if (strcmp(str, "usr1") == 0) return SIGUSR1;
    if (strcmp(str, "segv") == 0) return SIGSEGV;
    if (strcmp(str, "usr2") == 0) return SIGUSR2;
    if (strcmp(str, "pipe") == 0) return SIGPIPE;
    if (strcmp(str, "alrm") == 0) return SIGALRM;
    if (strcmp(str, "term") == 0) return SIGTERM;
    if (strcmp(str, "stkflt") == 0) return SIGSTKFLT;
    if (strcmp(str, "chld") == 0) return SIGCHLD;
    if (strcmp(str, "cont") == 0) return SIGCONT;
    if (strcmp(str, "stop") == 0) return SIGSTOP;
    if (strcmp(str, "tstp") == 0) return SIGTSTP;
    if (strcmp(str, "ttin") == 0) return SIGTTIN;
    if (strcmp(str, "ttou") == 0) return SIGTTOU;
    if (strcmp(str, "urg") == 0) return SIGURG;
    if (strcmp(str, "xcpu") == 0) return SIGXCPU;
    if (strcmp(str, "xfsz") == 0) return SIGXFSZ;
    if (strcmp(str, "vtalrm") == 0) return SIGVTALRM;
    if (strcmp(str, "prof") == 0) return SIGPROF;
    if (strcmp(str, "winch") == 0) return SIGWINCH;
    if (strcmp(str, "io") == 0) return SIGIO;
    if (strcmp(str, "poll") == 0) return SIGPOLL;
    if (strcmp(str, "pwr") == 0) return SIGPWR;
    if (strcmp(str, "sys") == 0) return SIGSYS;

    printf("Invalid Arg: %s is not a supported signal.\n", str);
    exit(1);
}

int parse_args(int argc, char *argv[], Signal signals[31]) {
    bzero(signals, 31);
    Signal current[31];
    int c = 0;
    int cmd_offset = 0;

    // SIGWINCH needs to be registered by default for resizing to work
    signals[27].sig = SIGWINCH;

    for (int i = 1; i < argc && cmd_offset == 0; i++) {
        char *arg = argv[i];
        int type = get_type(arg);
        switch (type) {
            case 0: {
                Signal signal = {get_sig(arg + 2)};
                current[c] = signal;
                c++;
                break;
            }
            case 1: {
                for (int j = 0; j < c; j++) current[j].block = 1;
                break;
            }
            case 2:
            case 3: {
                i++;
                for (int j = 0; j < c; j++) {
                    current[j].mode = type;
                    current[j].text = argv[i];
                    if (current[j].text[0] == '-') {
                        printf("Invalid Arg: %s\n", current[j].text);
                        exit(1);
                    }
                    signals[current[j].sig - 1] = current[j];
                }
                c = 0;
                break;
            }
            case 4: {
                printf("Invalid Arg: %s\n", arg);
                exit(1);
            }
            case 5: {
                cmd_offset = i;
            }
        }
    }

    for (int i = 0; i < c; i++)
        signals[current[i].sig - 1] = current[i];
    return cmd_offset;
}

int get_sig_from_code(char code) {
    switch (code) {
        case '\3':
            return SIGINT;
        default:
            return 0;
    }
}