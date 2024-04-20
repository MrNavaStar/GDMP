#include <signal.h>

void err(char * str);
void trap(int sig, __sighandler_t handler);