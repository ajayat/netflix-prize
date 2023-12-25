#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "parser.h"
#include "predictors.h"

#define MAX_USER_ID 2649430

static double shrink(double value, u_int n, double alpha)
{
    return value * (double)n / (n + alpha);
}

double mse_correlation(MovieData *data, Movie *movie1, Movie *movie2)
{
    u_int user_ratings1[MAX_USER_ID] = { -1 };
    u_int user_ratings2[MAX_USER_ID] = { -1 };

    for (int i = 0; i < movie1->nb_ratings; i++)
        user_ratings1[get_customer_id(movie1->ratings[i])] = i;
    
    for (int i = 0; i < movie2->nb_ratings; i++)
        user_ratings2[get_customer_id(movie2->ratings[i])] = i;

    // Calculate MSE between score given by same user
    double sum = 0;
    u_int nb_ratings = 0;
    u_int r1, r2;

    for (int i = 0; i < MAX_USER_ID; i++) {
        r1 = user_ratings1[i];
        r2 = user_ratings2[i];
        if (r1 != -1 && r2 != -1) {
            sum += pow(movie1->ratings[r1].score - movie2->ratings[r2].score, 2);
            nb_ratings++;
        }
    }
    double c = (double)nb_ratings / sum;
    return shrink(c, nb_ratings, 100);
}

void create_similarity_matrix(MovieData *data, double **similarity)
{
    for (int i = 0; i < data->nb_movies; i++) {
        for (int j = 0; j < i + 1; j++) {
            similarity[i][j] = mse_correlation(data, data->movies[i], data->movies[j]);
            similarity[j][i] = similarity[i][j];
        }
    }
}

static double logistic(double value, double a, double b)
{
    return 1 / (1 + exp(-a * (value - b)));
}   

UserRating *knn(double **similarity, u_int i, u_int k, User *u)
{
    return NULL;
}

double knn_movie(MovieData *data, double **similarity, User *user, Movie *movie)
{
    u_int k = 10;
    double tau = 30;
    double offset = 1;

    double score = 0;
    double sum_weights = 0;
    UserRating *nearest_ratings;
    if (k > data->nb_movies)
        nearest_ratings = knn(similarity, movie->id, k, user);
    else
        nearest_ratings = user->ratings;

    for (u_int i = 0; i < k; i++) {
        UserRating rating = nearest_ratings[i];
        double s = similarity[movie->id][rating.movie_id];
        u_int delta = abs(rating.date - movie->date);
        double s_date = logistic(s * exp(-delta / tau) + offset, 1, 0);
        score += s_date * user->ratings[rating.movie_id].score;
        sum_weights += s_date;
    }
    return score / sum_weights;
}
