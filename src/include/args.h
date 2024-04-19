typedef struct arguments {
    char *args[99];
    char *launch;
    char *quit;
} Args;

Args get_args(int argc, char *argv[]);