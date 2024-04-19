#include <string.h>
#include <argp.h>

#include "include/args.h"

const char *bug_tracker = "mr.navastar@gmail.com";
static struct argp_option options[] = {
        { "launch", 'l', "command", 0, "The command used to start the application."},
        { "stop", 's', "command", 0, "The command sent to the shell of the application to stop it."},
        { 0 }
};

static error_t parse_args(int key, char *arg, struct argp_state *state){
    Args *arguments = state->input;
    switch(key){
        case 'l':
            arguments->launch = arg;
            break;

        case 's':
            arguments->quit = arg;
            break;

        case ARGP_KEY_ARG:
            arguments->args[state->arg_num] = arg;
            break;

        //case ARGP_KEY_END:
            //if (strcmp(arguments->launch, "") == 0 || strcmp(arguments->quit, "") == 0) argp_usage (state);
            //break;

        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_args};

struct arguments get_args(int argc, char *argv[]) {
    Args arguments;
    arguments.launch = "";
    arguments.quit = "";
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    return arguments;
}