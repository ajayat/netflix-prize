# Explanations

## Composition of the dataset Netflix Prize

Read **[About Dataset](AboutDataset.md)**.

### Details

- Each pairs of movies and customer contained in 📄 *probe.txt* **also present** in the 📁 *training_set*[¹][1]. This allow us to use it to evaluate the accuracy of our recommendation algorithm because we know real ratings.

- Conversely, pairs contained in 📄 *qualifying.txt* are **not present** in the 📁 *training_set*. This file was used as part of the competition, but it does not interest us since we do not know real ratings of customers.

- A ⚙️ *rmse.pl* program providing an implementation in Perl of the RMSE (Root Mean Square Error) algorithm which will allow us to evaluate the accuracy of our recommendantion algorithm.

## The parser

The parser will read the data of the 📁 **training_set** and stock them in the form of a structure in C.

La structure doit contenir toutes les informations fournies, sans en rajouter des superflues pour optimiser la mémoire et ainsi le temps de lecture.
Par exemple, on ne va pas stocker la note moyenne pour chaque film car elle peut etre facilement recalculée et son calcul ne va pas nous être essentiel pour notre algorithme de recommandation.

L'idée est de stocker un tableau de films dont l'id est donné par son index, chaque film étant représenté par une structure contenant:

- The movie title
- The number of ratings
- An array of customers who rated this movie

De même, chaque client sera représenté par une structure contenant:

- The customer identifier
- The given rating
- The rating date, stocked in the form of an integral representing number of days since the Epoch date (defined as January 1st 1889).
- la date de notation, stockée sous la forme d'un entier représentant le nombre de jours depuis la date Epoch.

Ces données seront écrits dans un fichier binaire avec *fwrite* en C pour optimiser la récupération des données, car l'encodage et le décodage unicode est très couteux en temps.

## Les statistiques

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
