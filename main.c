#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "parser.h"
#include "utils.h"
#include "stats.h"
#include "predictors.h"

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
    { "likes_file", 'r', "FILE", 0, "Gives recommandations.", 0 },
    { "number", 'n', "NUMBER", 0, "Number of recommandations.", 0 },
    { 0 }
};

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int main(int argc, char *argv[])
{
    Arguments args = {
        .folder = "stats/",
        .limit = INT16_MAX,
        .movie_id = 0,
        .nb_customer_ids = 0,
        .customer_ids = NULL,
        .nb_bad_reviewers = 0,
        .bad_reviewers = NULL,
        .min = 0,
        .time = false,
        .likes_file = NULL,
        .nb_recommandations = 10
    };
    argp_parse(&argp, argc, argv, 0, 0, &args);

    // Parse data
    MovieData *data = read_movie_data_from_file("data/data.bin");
    if (data == NULL) {
        data = parse();
        write_movie_data_to_file("data/data.bin", data);
    }
    Stats *stats = read_stats_from_file("data/stats.bin");
    if (stats == NULL) {
        stats = read_stats_from_data(data, &args);
        write_stats_to_file(stats, "data/stats.bin");
    }
    if ((access("data/probe_predictions.txt", F_OK) == -1))
        parse_probe("data/probe.txt", stats, data);

    if (args.likes_file != NULL) {
        uint *ids = NULL;
        uint n = parse_likes(args.likes_file, data, &ids);
        if (n == 0) {
            fprintf(stderr, "Could not parse titles in file %s\n", args.likes_file);
            exit(EXIT_FAILURE);
        }
        puts("============= Recommandations =============");
        uint *recommandations = knn_movies(stats, ids, n, args.nb_recommandations);
        for (uint i = 0; i < args.nb_recommandations; i++)
            printf("%u. %s\n", i+1, data->movies[recommandations[i]-1]->title);
        free(ids);
    }
    // Free memory
    free_args(&args);
    free_stats(stats);
    free_movie_data(data);

    return 0;
}