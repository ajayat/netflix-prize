# Explanations

## Composition of the dataset Netflix Prize

Read **[About Dataset](AboutDataset.md)**.

### Details

- Each pairs of movies and customer contained in 📄 *probe.txt* **also present** in the 📁 *training_set*[¹][1]. This allow us to use it to evaluate the accuracy of our recommendation algorithm because we know real ratings.

- Conversely, pairs contained in 📄 *qualifying.txt* are **not present** in the 📁 *training_set*. This file was used as part of the competition, but it does not interest us since we do not know real ratings of customers.

- A ⚙️ *rmse.pl* program providing an implementation in Perl of the RMSE (Root Mean Square Error) algorithm which will allow us to evaluate the accuracy of our recommendantion algorithm.

## The parser

The parser will read the data of the 📁 **training_set** and stock them in the form of a structure in C. This one should contain all information without adding any in order to optimize memory and reading time.

The idea is to store an array of movies for which their index is their identifier. Each movie is represented by a structure containing:

- The movie identifier
- The title
- The date (see note below)
- The number of ratings
- An array of customers who rated this movie

> **Note**: The date is stored in number of days since the Epoch date (defined as `January 1st 1889`).

Fields of structures are stored in a specific order that takes into account structure padding, in order to minimize memory.

Likewise, each rating will be represented by a structure containing:

- The customer identifier (see note below)
- The given score
- The rating date

> **Note**: The customer identifier is stored in 3 bytes, divided into 2 bytes for the most significant bytes and 1 byte for the less significant byte. That allows us to reducing memory by ~100MB.

These data will be written in a binary file thanks to `fwrite` in C to optimize date recovery, because unicode encoding and decoding is very time-consuming. Data are stored in this order:

- Number of movies (2 bytes), and for each one:
  - Its identifier (2 bytes)
  - The number of bytes to encode its title (1 byte)
  - Its title (previous value × bytes)
  - Its date (2 bytes)
  - Its number of ratings (4 bytes), and for each one:
    - The customer identifier (3 bytes) divided into:
      - Most significant bytes (2 bytes)
      - Less significant byte (1 byte)
    - The given score (1 byte)
    - The date of the rating (2 bytes)

| Number of movies | Movie identifier | Length of title | Title                   | Date    | Number of ratings | Customer identifier         | Score  | Date of rating |
| :--------------: | :--------------: | :-------------: | :---------------------: | :-----: | :---------------: | :-------------------------: | :----: | :------------: |
|     2 bytes      | 2 byte           | 1 byte          | Length of title × bytes | 2 bytes | 4 bytes           | msb: 2 bytes & lsb: 1 byte | 1 byte | 2 bytes        |

A function `to_user_oriented` will allow us to convert this structure to a user-oriented structure, which will be more convenient for the recommendation algorithm.

Functions `read_movie_data_from_file`, `write_movie_data_to_file`, `read_user_data_from_file` and `write_user_data_to_file` will allow us to read and write data from and to binary files, like the movie data above.

## The statistics

### Statistics from the data

In this part and before to begin the recommedaion algorithm, it is important to analyze the size and the repartition of our data.
We calculate two kind of statistics : those which are based on movies (average rating, minimum, maximum, date) and those based on users (number of ratings, their average, number of ratings per day).
For example, a user who rated more than 2 or 3 movies/series in one single day can't be taken seriously, because he certainly rated movies he did'nt watch. But it's also possible that he decided to rate movies he saw before opening his account. So his ratings will be taken in account, but with an impact more moderate than a "regular" user.

In addition, we take into account the arguments of the command line. In fact, statistics are generated only from ratings which respect all given options. And we allow to create a file with the statistics of one movie especially, to answer to the use of the option `-s`. You can find the different options bellow:

| flag |     argument     |                                                     description                                                      |
| :--: | :--------------: | :------------------------------------------------------------------------------------------------------------------- |
| `-r` |    LIKES_FILE    |                                          List of movies liked by the user.                                           |
| `-n` |        N         |                              Length of the recommendation list the algorithm will give.                              |
| `-f` |      FOLDER      |                      The path of the folder where files corresponding to results will be saved.                      |
| `-l` |      LIMIT       |                       Forbidden to take in acount ratings with a date greater than the LIMIT.                        |
| `-s` |     MOVIE_ID     |                            Give statistics about the movie with the identifier MOVIE_ID.                             |
| `-c` |   CUSTOMER_IDS   |                 Allow to take into account only the ratings of the customers with given identifiers.                 |
|  ∅   | NB_CUSTOMER_IDS  |                                            Number of given customer ids.                                             |
| `-b` |  BAD_REVIEWERS   |                 Allow to not take into account the ratings of the customers with given identifiers.                  |
|  ∅   | NB_BAD_REVIEWERS |                                            Number of given bad reviewers.                                            |
| `-e` |       MIN        |                       Allow to take into account only customers who rated at least MIN movies.                       |
| `-t` |       TIME       |                                    Precise the executive time of the algorithme.                                     |
| `-p` |     PERCENT      | Percentage (between 0 and 1) to quantify the importance of personnal recommendations versus popular recommendations. |

Note that options `-r`, `-n`, `-t` and `-p` are not used for statistics processing.

### The similarity matrix

The similarity matrix is a matrix which show the proximity between two movies, based on their ratings, especially when they have been note by same users. It gives a coefficient between 0 and 1: more the score is close to 1, more the movies are "similar".

For example, movies with identifiers `11164` and `270` reach a score around 0.81. But they correspond to `Sex and the City: Season 3` and `Sex and the City: Season 4`: a high score is normal! In an other hand, we have `Mississippi Burning` (id `442`) and `The Game` (id `143`) which reached a score aroud 0.45: they have similarity, like their place (USA) or the fact they are realistic and intriguing, but don't have more links than that.

#### The Hashmap data structure

To create the similarity matrix, we use a hashmap structure: we need to quickly know if a user who rated this movie also rated this other one. But it should be too long to sort all user identifiers for a movie. Thanks to the hashmap, we have an access to the corresponding identifier in *O*(1) amortized time.

This hashmap uses open addressic to resolve collisions, and uses quadratic probing in order to optimize the searching.

The load factor (number of elements / capacity) is 0.5, which is a good value to optimize the searching time, according to this very useful benchmark[³][3].

## The recommendation algorithm

### Objective

The aim of the recommendation algorithm is to predict the rating a customer would give a movie, with a certain degree of accuracy.

Accuracy is measured by the RMSE, in relation to actual ratings.
Our algorithm must therefore **obtain the smallest possible RMSE**.

This rating must therefore be floating-point, to be as accurate as possible from a probabilistic point of view.

### Algorithm

To design a recommendation algorithm, we have two main approaches[²][2]:

- A customer-oriented system
- A movie-oriented system

Here, due to the large number of customers (over 480,000), the first option is unthinkable, as it would require storing a 480,000 x 480,000 matrix.

We therefore used a movie x movie matrix expressing the similarity between two movies by a coefficient ranging from 0 to 1.

In this way, given a list of movies liked by a customer, we can recommend a list of `k` movies, in order of priority using the k nearest neighbors agorithm kNN.

The heart of the problem is therefore to establish this matrix precisely, which we'll call the *movie correlation matrix*.

Some possible correlation functions:

- Pearson's law
- Spearman's law
- Ensemble correlation
- MSE correlation

We have chosen to use MSE correlation on ratings rated by the same users, as it is simple to implement and gives good results.

## References

1. [The Netflix Prize, Netflix researchers]([3])
2. [Recommender System, Wikipedia]([2])
3. [Hashtables, *thenumb.at*]([3])

[1]: https://www.cs.uic.edu/~liub/KDD-cup-2007/proceedings/The-Netflix-Prize-Bennett.pdf
[2]: https://en.wikipedia.org/wiki/Recommender_system
[3]: https://thenumb.at/Hashtables/
