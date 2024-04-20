typedef struct signal {
    int sig;
    int block;
    int mode;
    char *text;
} Signal;

int parse_args(int argc, char *argv[], Signal signals[32]);