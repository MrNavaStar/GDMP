typedef struct {
    int sig;
    int block;
    int mode;
    char *text;
} Signal;

int parse_args(int argc, char *argv[], Signal signals[31]);
int get_sig_from_code(char code);