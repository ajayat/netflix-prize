#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <stdbool.h>

#include "parser.h"
#include "utils.h"
#include "stats.h"

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

static u_int parse_ints(char *arg, u_long *ids)
{
    char* end = NULL;
    u_int i = 0;
    u_int id;
    while ((id = strtoul(arg, &end, 10)) != 0) {
        ids[i++] = id;
        arg = end + 1;
    }
    return i;
}

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    Arguments *args = state->input;
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
        args->customer_ids = malloc(2 * sizeof(u_long));
        args->nb_customer_ids = parse_ints(arg, args->customer_ids);
        return 0;
    case 'b':
        args->bad_reviewers = malloc(2 * sizeof(u_long));
        args->nb_bad_reviewers = parse_ints(arg, args->bad_reviewers);
        return 0;
    case 'e':
        args->min = strtoul(arg, NULL, 10);
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
    Arguments args;
    // defaults values
    args.folder = "data/";
    args.movie_id = 0;
    args.limit = INT16_MAX;
    args.min = 0;
    args.time = false;
    args.nb_customer_ids = 0;
    args.nb_bad_reviewers = 0;

    argp_parse(&argp, argc, argv, 0, 0, &args);

    MovieData *movie_data = parse();
    UserData *user_data = to_user_oriented(movie_data);
    Stats *stats = read_stats_from_data(movie_data, user_data, &args);
    // stats->similarity = create_similarity_matrix(movie_data);

    // printf("nb_movies: %d\n", stats->nb_movies);
    free_movie_data(movie_data);
    free_user_data(user_data);
    free_stats(stats);

    return 0;
}