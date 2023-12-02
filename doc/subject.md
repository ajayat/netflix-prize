
1
Projet Programmation 2023
T ́el ́ecom Physique Strasbourg: 1A-IR
Enseignant: Pierre Galmiche
1 Id ́ee g ́en ́erale du projet
Votre objectif lors de ce projet sera d’impl ́ementer un algorithme de recommandation de film bas ́e bas ́e sur un jeu de
donn ́ees pr ́evu pour une comp ́etition appel ́ee ”Netflix Prize”. Ce travail vous permettra d’am ́eliorer vos comp ́etences en
programmation, mais aussi votre gestion de projet !
2 Attendus
Le projet doit s’effectuer seul ou en binˆome. Les fichiers d’en-tˆetes doivent tous ˆetre document ́es. Une documentation
globale devra ˆetre g ́en ́er ́ee `a partir des fichier source (g ́en ́eration `a l’aide de make doc) et la page d’accueil de la documentation
doit expliquer rapidement la structure de votre programme, comment il doit ˆetre utilis ́e, et ce qui ne fonctionne pas !! Le
code source doit ˆetre d ́epos ́e sur un projet ”git” sur le ”git de l’unistra”. L’enseignant doit ˆetre ajout ́e dans les membres
”maintainer” du projet pour y avoir acc`es. Les codes source doivent ˆetre d ́epos ́es ”sur le git” du projet avant le vendredi 12
Janvier 2024, 23h59.
Attention, si vous utilisez du code source trouv ́e sur internet, vous devez le citer clairement dans le code source et vous devez
comprendre ce que vous avez copi ́e (je me r ́eserve le droit de vous poser des questions dessus). La difficult ́e du projet d ́epend
de la quantit ́e de choses que vous aurez impl ́ement ́ee vous-mˆeme et est prise en compte dans la notation. La grille de notation
tient compte de la clart ́e du code, de la structure, des choix d’impl ́ementation, des tests et de la quantit ́e de choses r ́ealis ́ees.
Notamment vous devez:
•organiser vos fichiers correctement (dans plusieurs dossiers).
• ́ecrire une documentation (dans les fichiers d’entˆete).
•rendre un projet qui compile sans erreurs ni warning (en activant tous les warning), et sans erreur valgrind.
• ́ecrire des tests (unitaires avec un programme tests.c et/ou d’ex ́ecution avec un test.sh) s ́epar ́es qui permettent de
v ́erifier que vos fonctions sont correctes.
• ́ecrire un makefile avec des r`egles appropri ́ees (all, doc, tests).
•prendre en charge le signal SIGINT (qui se produit lors d’un ”Ctrl+C)
3 D ́etails sur le projet
3.1 Un parser `a faire
Votre programme doit parser les fichiers de la base de donn ́ee ”Netflix Prize” disponible sur ce lien. Les fichiers  ́etant
relativement volumineux, il est fortement conseill ́e de se ”fabriquer” un fichier plus petit poss ́edant les mˆemes caract ́eristiques
afin de tester le bon fonctionnement de votre programme. Attention, pensez `a mettre un fichier ”.gitignore” dans votre projet
qui contient le chemin vers les fichiers avec les donn ́ees, afin qu’ils ne soit pas ajout ́es par git. Si un fichier ”.txt” apparaˆıt
lorsque vous faites un ”git status”, c’est qu’il n’est pas correctement ignor ́e.
L’ ́etape de parsage du fichier doit permettre de stocker les informations pertinentes du fichier dans des structures (on peut
ainsi ignorer beaucoup d’informations). Le parsage peut ˆetre sp ́ecifique au fichier mais il serait pr ́ef ́erable que cette  ́etape
puisse tol ́erer des petites variations dans le format, ou en tout cas, afficher des erreurs lisibles en cas de probl`eme de format.
1
3 D ́etails sur le projet 2
3.2 Un format binaire
Une fois le parsage termin ́e, votre programme doit ˆetre capable de sauvegarder la structure dans un fichier binaire (indiqu ́e avec
une option -o OUTPUT FILE) ayant un format bien d ́efini que vous devrez documenter. Cette  ́etape permettra notamment
de recharger la structure en m ́emoire beaucoup plus rapidement que de le faire depuis les fichiers ”.txt”.
3.3 Un algo de recommandation bas ́e sur les statistiques observ ́ees
Quelques statistiques
Une fois la structure en m ́emoire, vous devez impl ́ementer des algorithmes pour obtenir des statistiques `a partir des donn ́ees
sauvegard ́ees. Chaque algorithme est ex ́ecut ́e en donnant une option de ligne de commande. Par exemple :
•l’option -f FOLDER pr ́ecisera le chemin du dossier o`u seront sauvegard ́es les fichiers correspondants aux r ́esultats
demand ́es
•l’option -l LIMITE permettra d’interdire de prendre en compte des notes si leur date est sup ́erieur `a LIMITE
•l’option -s FILM ID donnera des statistiques sur le film d’identifiant FILM ID (nombre de notes, note moyenne ...).
•l’option -c X, Y pourrait permettre de ne prendre en compte que les notes des clients X et Y.
•l’option -b BAD REVIEWERS permettra de ne pas prendre en compte les notes de certains clients
•l’option -e MIN permettra de ne prendre en compte que les clients d’ ́elite ayant vu un minimum de MIN films
•l’option -t pr ́ecisera le temps d’ex ́ecution de votre algorithme
Un algorithme de recommendation
Si la base de votre programme de statistiques fonctionne, vous pouvez le tester avec la partie recommendation. Par exemple,
votre algo pourra donner une liste de films recommand ́es `a partir d’une liste de films que l’utilisateur a aim ́e.
”./prog -r ”F1,F2,F3,F4,F5,F6” -n 10” donnerais une liste de 10 films qui peuvent int ́eresser quelqu’un qui aime les films
F1-F6. Il faudrait que  ̧ca puisse fonctionner si on donne un fichier en entr ́ee: ”./prog -r mes likes.txt -n 10” Avec le fichier
mes likes.txt qui contient:
”
F1
F2
F3
...”
On pourrait mˆeme imaginer -a ALGO DE RECOMMANDATION pour laisser l’utilisateur choisir son algorithme de recom-
mandation si plusieurs sont impl ́ement ́es.
 ́Evidement beaucoup d’exemple ci-dessus sont des extensions possibles, vous devez commencer par r ́esoudre le probl`eme
de base avant de vous lancer dans des extensions. D’autres options peuvent ˆetre ajout ́ees. Vous pourrez aussi impl ́ementer
un affichage graphique afin de visualiser les statistiques faites avec des histogrammes par exemple. Ou encore chercher un
film qui contiendrait un mot ou une lettre sp ́ecifique dans son titre.
3.4 Des tests pour v ́erifier que tout fonctionne bien
Le projet doit avoir un Makefile avec une r`egle clean et dont la premi`ere r`egle permet de compiler l’ex ́ecutable principal. Un
dossier tests doit contenir un Makefile permettant de compiler un programme tests.c qui teste les diff ́erentes fonctions que
vous avez impl ́ement ́ees. Il pourra aussi y avoir un script test.sh qui ex ́ecute le programme sur des fichiers de tests afin de
v ́erifier que la sortie est correcte.