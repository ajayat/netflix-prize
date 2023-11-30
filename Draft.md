# Projet Programmation - Reflexion

## Composition du dataset Netflix Prize

- un dossier *training_set* contenant 17770 fichiers sous la forme mv_{id_film}.txt qui contiennt des informations sur les clients ayant noté pour le film {id_film}:
    - L'id du client
    - La note donnée (de 1 à 5)
    - La date de notation

- un fichier *movies_titles.txt* contenant les identifiants de 1 à 17770 des films et leur titres.

- un fichier *probe.txt* contenant pour un film donné un sous ensemble de clients, **présents** dans le *training_set*, ayant noté pour ce film.
    - Ce fichier va nous servir pour évaluer la précision de notre algorithme de recommandation, car on connait les notes réelles de chaque client.

- un fichier *qualifying.txt* contenant pour un film donné un sous ensemble de clients, **qui ne sont pas présents** dans le *training_set*, ayant noté pour ce film, et leur date de notation.
    - Ce fichier est utilisé dans le cadre de la compétition pour classer les candidats, mais ne vas donc pas nous être utile, car on ne connait pas les notes réelles de chaque client.

- un programme *rmse.pl* fournissant une implémentation en Perl de l'algorithme RMSE (Root Mean Square Error) qui va nous permettre d'évaluer la précision de notre algorithme de recommandation.

Références: [The Netflix Prize Bennett](https://www.cs.uic.edu/~liub/KDD-cup-2007/proceedings/The-Netflix-Prize-Bennett.pdf)

## Le parser

Le parser va lire les données du *training_set* et les stocker sous forme de structure en C.

La structure doit contenir toutes les informations fournies, sans en rajouter des superflues pour optimiser la mémoire et ainsi le temps de lecture.
Par exemple, on ne va pas stocker la note moyenne pour chaque film car elle peut etre facilement recalculée et son calcul ne va pas nous être essentiel pour notre algorithme de recommandation.

L'idée est de stocker un tableau de films dont l'id est donné par son index, chaque film étant représenté par une structure contenant:

- le nombre d'évaluations
- le titre du film
- un tableau de clients ayant noté pour ce film.

De même, chaque client sera représenté par une structure contenant:

- l'id du client
- la note donnée
- la date de notation, stockée sous la forme d'un entier représentant le nombre de jours depuis la date Epoch (1er Janvier 1970).

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

Pour concevoir un algorithme de recommandation, nous avons deux principales approches:

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

Références: [Recommender system (Wikipedia)](https://en.wikipedia.org/wiki/Recommender_system)