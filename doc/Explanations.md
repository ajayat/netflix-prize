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

Dans cette partie, et avant de commencer l'algorithme de recommandation, il est important d'analyser la taille et la répartition de nos données.

Par exemple, jusque là on pouvait penser que la date était une information peu pertinente, cependant on peut observer que certains clients ont noté plusieurs fois le même film, ou ont notés plusieurs films le même jour, voire trop.

Il sera donc interessant de calculer la fréquence de notation de chaque client chaque jour, afin d'ajuster la pertinence d'une note donnée.

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
