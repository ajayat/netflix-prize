# Explanations

## Composition of the dataset Netflix Prize

Read **[About Dataset](AboutDataset.md)**.

### Details

- Each pairs of movies and customer contained in 📄 *probe.txt* **also present** in the 📁 *training_set*[¹][1]. This allow us to use it to evaluate the accuracy of our recommendation algorithm because we know real ratings.

- Conversely, pairs contained in 📄 *qualifying.txt* are **not present** in the 📁 *training_set*. This file was used as part of the competition, but it does not interest us since we do not know real ratings of customers.

- A ⚙️ *rmse.pl* program providing an implementation in Perl of the RMSE (Root Mean Square Error) algorithm which will allow us to evaluate the accuracy of our recommendantion algorithm.

## The parser

The parser will read the data of the 📁 **training_set** and stock them in the form of a structure in C. This one should contain all information without adding any in order to optimize memory and readding time.

The idea is to stock an array of movies for which their index is their identifier. Each movie is represented by a structure containing:

- The movie title
- The number of ratings
- An array of customers who rated this movie

Likewise, each rating will be represented by a structure containing:

- The customer identifier
- The given rating
- The rating date, stocked in the form of an integral representing number of days since the Epoch date (defined as January 1st 1889).

These data will be written in a binary file thanks to `fwrite` in C to optimize date recovery, because unicode encoding and decoding is very time-consuming. Data are stocked in this order:

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

## The statistics

### Statistics from the data

In this part and before to begin the recommedaion algorithm, it is important to analyze the size and the repartition of our data.
We calculate two kind of statistics : those which are based on movies (average rating, minimum, maximum, date) and those based on users (number of ratings, their average, number of ratings per day).
For example, a user who rated more than 2 or 3 movies/series in one single day can't be taken seriously, because he certainly rated movies he did'nt watch. But it's also possible that he decided to rate movies he saw before opening his account. So his ratings will be taken in account, but with an impact more moderate than a "regular" user.

In addition, we take into account the arguments of the command line. In fact, statistics are generated only from ratings which respect all given options. And we allow to create a file with the statistics of one movie especially, to answer to the use of the option `-s`. You can find the different options bellow:

| flag |    content    |                                      description                                      |
| :--: | :-----------: | :-----------------------------------------------------------------------------------: |
| `-r` |   LIKES.TXT   |                           List of movies liked by the user.                           |
| `-n` |    NUMBER     |              Length of the recommendation list the algorithm will give.              |
| `-f` |    FOLDER     |      The path of the folder where files corresponding to results will be saved.       |
| `-l` |     LIMIT     |        Forbidden to take in acount ratings with a date greater than the LIMIT.        |
| `-s` |   MOVIE_ID    |             Give statistics about the movie with the identifier MOVIE_ID.             |
| `-c` |     X, Y      | Allow to take into account only the ratings of the cusstomers with given identifiers. |
| `-b` | BAD_REVIEWERS |  Allow to not take into account the ratings of the customers with given identifiers.  |
| `-e` |      MIN      |       Allow to take into account only customers who rated at least MIN movies.        |
| `-t` |       ∅       |                    Precise the executive time of the algorithme.                     |

Note that options `-r`, `-n` and `-t` are not used for statistics processing.

### The similarity matrix

The similarity matrix is a matrix which show the proximity between two movies, based on their ratings, especially when they have been note by same users. It gives a coefficient between 0 and 1: more the score is close to 1, more the movies are "similar".

For example, movies with identifiers `11164` and `270` reach a score around 0.81. But they correspond to `Sex and the City: Season 3` and `Sex and the City: Season 4`: a high score is normal! In an other hand, we have `Mississippi Burning` (id `442`) and `The Game` (id `143`) which reached a score aroud 0.45: they have similarity, like their place (USA) or the fact they are realistic and intriguing, but don't have more links than that.

To create the similarity matrix, we use a hashmap structure: we need to quickly know if a user who rated this movie also rated this other one. But it should be too long to sort all user identifiers for a movie. Thanks to the hashmap, we have a more or less direct access to the user, using a quadratic probing to sort them in order to optimize the searching.

## L'algorithme de recommandation

### Objectif

Le but de l'algorithme de recommandation est de prédire la note qu'un client donnerait à un film, avec un certaine précision.

La précision est évaluée avec le RMSE, par rapport aux notes réelles.
Notre algorithme devra donc **obtenir un RMSE le plus petit possible.**

Cette note doit donc être necessairement à virgule flottante, pour qu'elle soit la plus précise possible du point de vu probabiliste.

### Conception

Pour concevoir un algorithme de recommandation, nous avons deux principales approches[²][2]:

- Un système orienté client
- Un système orienté film

Ici, dû à l'important nombre de clients (plus de 480 000), la première option est inenvisageable, car elle nécessiterait de stocker une matrice de 480 000 x 480 000.

Une première idée, serait de se baser sur une matrice film x film exprimant la similarité entre deux films par un coefficient (allant de 0 à 1 par exemple).

De cette manière, étant donnée une liste de films aimés par un client, on peut recommander une liste de k films, par ordre de priorité en utilisant l'agorithme des k plus proches voisins kNN.

Le coeur du problème est donc d'établir avec précision cette matrice, qu'on va appeler *matrice de corrélation des films*.

Quelques fonctions de corrélation possibles:

- Loi de Pearson
- Loi de Spearman
- Corrélation d'ensemble

## References

1. <https://www.cs.uic.edu/~liub/KDD-cup-2007/proceedings/The-Netflix-Prize-Bennett.pdf>
2. <https://en.wikipedia.org/wiki/Recommender_system>

[1]: https://www.cs.uic.edu/~liub/KDD-cup-2007/proceedings/The-Netflix-Prize-Bennett.pdf
[2]: https://en.wikipedia.org/wiki/Recommender_system
