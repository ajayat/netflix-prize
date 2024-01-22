#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "parser.h"
#include "utils.h"
#include "stats.h"
#include "predictors.h"

static char doc[] = "Programming Project";
static char args_doc[] = "";

static struct argp_option options[] = {
    { "force", 'f', 0, 0, "Force to recompute all stats.", 0 },
    { "directory", 'd', "PATH", 0, "Path of the binary file.", 0 },
    { "limite", 'l', "LIMIT", 0, "Exclude ratings before the given date.", 0 },
    { "movie", 's', "MOVIE_ID", 0, "Movie ID.", 0 },
    { "customers", 'c', "IDS", 0, "List of customer IDs.", 0 },
    { "bad_reviewers", 'b', "IDS", 0, "List of customer IDs.", 0 },
    { "min", 'e', "MIN", 0, "Restrict to customers who have seen >= MIN films", 0 },
    { "t", 't', 0, 0, "Display execution time.", 0 },
    { "likes", 'r', "FILE|MOVIE_IDS", 0, "Give recommendations from movies present in FILE.", 0 },
    { "number", 'n', "NUMBER", 0, "Number of recommendations to display.", 0 },
    { "percent", 'p', "PERCENT", 0, "Percentage of personnalized recommandations (vs. popular recommendations).", 0 },
    { 0 }
};

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int main(int argc, char *argv[])
{
    clock_t t_start = clock();  // Start time

    Arguments args = {
        .force = false,
        .directory = "stats",
        .limit = INT16_MAX,
        .movie_id = 0,
        .nb_customer_ids = 0,
        .customer_ids = NULL,
        .nb_bad_reviewers = 0,
        .bad_reviewers = NULL,
        .min = 0,
        .time = false,
        .likes = NULL,
        .n = 10,
        .percent = 0.9
    };
    argp_parse(&argp, argc, argv, 0, 0, &args);

    // Parse data
    MovieData *data = read_movie_data_from_file("data/data.bin");
    if (data == NULL) {
        data = parse();
        write_movie_data_to_file("data/data.bin", data);
    }
    // Compute stats
    char *stats_fp = get_filepath(args.directory, "stats.bin");
    Stats *stats = read_stats_from_file(stats_fp);
    if (stats == NULL
        || args.force
        || args.limit != INT16_MAX 
        || args.movie_id != 0 
        || args.min > 0
        || args.nb_customer_ids > 0 
        || args.nb_bad_reviewers > 0) 
    {
        stats = read_stats_from_data(data, &args);
        write_stats_to_file(stats, stats_fp);
    }
    free(stats_fp);

    // Calculate predictions
    if ((access("data/probe_predictions.txt", F_OK) == -1)) 
    {
        UserData *user_data = to_user_oriented(data);
        write_probe_predictions("data/probe_predictions.txt", stats, user_data);
        // free_user_data(user_data);
        // Calculate RMSE
        double rmse = rmse_probe_calculation("data/probe_predictions.txt");
        printf("RMSE value: %.3lf\n", rmse);
    }

    if (args.likes != NULL) 
    {
        ulong *ids = NULL;
        uint n = parse_likes(args.likes, data, &ids);
        if (n == 0) {
            fprintf(stderr, "Could not parse liked movies.\n");
            goto end;
        }
        puts("============= Recommandations =============");
        uint *recommandations = knn_movies(stats, ids, n, args.n, args.percent);
        for (uint i = 0; i < args.n; i++)
            printf("%u. %s\n", i+1, data->movies[recommandations[i]-1]->title);
        free(ids);
    }
end: 
    // Free memory
    free_args(&args);
    free_stats(stats);
    free_movie_data(data);

    if (args.time) {
        double time_elapsed_ms = (double)(clock() - t_start) / CLOCKS_PER_SEC;
        printf( "Finished in %.2lfs\n", time_elapsed_ms);
    }
    return 0;
}