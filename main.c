#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <stdbool.h>

#include "parser.h"
#include "utils.h"

static char doc[] = "Programming Project";
static char args_doc[] = "";

static struct argp_option options[] = { 
    { "folder", 'f', "FOLDER_PATH", 0, "Folder path of the binary file.", 0 },
    { "limite", 'l', "LIMIT", 0, "Exclude ratings before the given date.", 0 },
    { "movie", 's', "MOVIE_ID", 0, "Movie ID.", 0 },
    { "customers", 'c', "CUSTOMER_IDS", 0, "List of customer ID.", 0 },
    { "bad_reviewers", 'b', "BAD_REVIEWERS", 0, "List of customer ID.", 0 },
    { "min", 'e', "MIN", 0, "Restrict to customers who have seen >= MIN films", 0 },
    { "t", 't', 0, 0, "Display execution time.", 0 },
    { 0 }
};

typedef struct arguments {
    const char* folder;
    unsigned int limit;
    unsigned int movie_id;
    unsigned int customer_ids[2];
    unsigned int bad_reviewers[2];
    unsigned int min;
    bool time;
} arguments;

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    arguments *args = state->input;
    switch (key) {
    case 'f':
        args->folder = arg;
        return 0;
    case 'l':
        args->limit = strtoul(arg, NULL, 10);
        return 0;
    case 's':
        args->movie_id = strtoul(arg, NULL, 10);
        return 0;
    case 'c':
        args->customer_ids[0] = strtol(arg, NULL, 10);
        args->customer_ids[1] = strtol(arg, NULL, 10);
        return 0;
    case 'b':
        args->bad_reviewers[0] = strtol(arg, NULL, 10);
        args->bad_reviewers[1] = strtol(arg, NULL, 10);
        return 0;
    case 'e':
        args->min = strtol(arg, NULL, 10);
        return 0;
    case 't':
        args->time = true;
        return 0;
    case ARGP_KEY_ARG:
        return 0;
    default:
        return ARGP_ERR_UNKNOWN;
    }
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int main(int argc, char *argv[])
{
    arguments args;
    argp_parse(&argp, argc, argv, 0, 0, &args);

    return 0;
}
