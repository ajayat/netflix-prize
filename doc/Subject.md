# Subject

`Télécom Physique Strasbourg: 1A-IR`
`Teacher: Pierre Galmiche`

## 1. General idea of the project

Your objective during this project will be to implement a movie recommendation algorithm based on a dataset intended for a competition called “Netflix Prize”. This work will allow you to improve your programming skills, but also your project management!

## 2. Expected

The project must be carried out alone or in pairs. Header files must all be documented. Overall documentation should be generated from the source files (generation using make doc) and the documentation home page should quickly explain the structure of your program , how it should be used, and what doesn't work!! The source code must be deposited on a “git” project on the “unistra git”. The teacher must be added to the “maintainer” members of the project to have access. The source codes must be deposited “on the git” of the project before `Friday January 12, 2024, 11:59 PM`.

Please note, if you use source code found on the internet, you must cite it clearly in the source code and you must understand what you have copied (I reserve the right to ask you questions about it). The difficulty of the project depends on how much you implemented yourself and is taken into account in the rating. The grading grid takes into account the clarity of the code, the structure, the implementation choices, the tests and the quantity of things achieved.

In particular you must:

- organize your files correctly (in multiple folders).
- write documentation (in header files).
- render a project that compiles without errors or warnings (by activating all warnings), and without valgrind errors.
- write separate tests (unit with a tests.c program and/or execution with a test.sh) which allow you to check that your functions are correct.
- write a makefile with appropriate rules (all, doc, tests).
- support the SIGINT signal (which occurs during a ”Ctrl+C”).

## 3. Details about the project

### 3.1. A parser to do

Your program must parse the files from the “Netflix Prize” database available at [this link](https://academictorrents.com/details/9b13183dc4d60676b773c9e2cd6de5e5542cee9a). The files being relatively large, it is strongly recommended to “make” a smaller file with the same characteristics in order to test the correct functioning of your program. Be careful, remember to put a “.gitignore” file in your project which contains the path to the files with the data, so that they are not added by git. If a “.txt” file appears when you do a “git status”, it is not correctly ignored.

The file parsing step must allow the relevant information of the file to be stored in structures (we can thus ignore a lot of information). Parsing can be specific to the file but it would be preferable if this step could tolerate small variations in the format, or in any case, display readable errors in the event of a format problem.

### 3.2. A binary format

Once parsing is complete, your program must be able to save the structure in a binary file (indicated with an `-o OUTPUT FILE` option) with a well-defined format that you will need to document. This step will notably allow the structure to be reloaded into memory much more quickly than doing so from the “.txt” files.

### 3.3. A recommendation algorithm based on observed statistics

#### Some statistics

Once the structure is in memory, you must implement algorithms to obtain statistics from the saved data. Each algorithm is executed by giving a command line option. For example :

- the `-f FOLDER` option will specify the path of the folder where the files corresponding to the requested results will be saved.
- the option `-l LIMITED` will prohibit taking notes into account if their date is greater than LIMITED.
- the `-s FILM_ID` option will give statistics on the film's FILM_ID identifier (number of ratings, average rating, etc.).
- the `-c X, Y` option could only take into account the ratings of customers X and Y.
- the `-b BAD_REVIEWERS` option will allow you to not take into account the ratings of certain customers.
- the `-e MIN` option will only take into account elite customers who have seen a minimum of MIN films.
- the `-t option` will specify the execution time of your algorithm.

#### A recommendation algorithm

If the basis of your statistics program works, you can test it with the recommendation part. For example, your algorithm could give a list of recommended movies from a list of films that the user liked.

`./prog -r ”M1,M2,M3,M4,M5,M6” -n 10` would give a list of 10 movies that might interest someone who likes the M1-M6 films. It should work if we give a file as input: `./prog -r my_likes.txt -n 10`. With the file *my_likes.txt* which contains:

```md
M1
M2
M3
...
```

We could even imagine `-a RECOMMENDATION_ALGORITHM` to let the user choose his recommendation algorithm if several are implemented.

Obviously many of the examples above are possible extensions, you must start by solving the basic problem before launching into extensions. Other options can be added. You can also implement a graphical display in order to visualize the statistics made with histograms for example. Or look for a film that contains a specific word or letter in its title.

### 3.4. Tests to check that everything is working properly

The project must have a Makefile with a clean rule and whose first rule allows the main executable to be compiled. A tests folder must contain a Makefile allowing you to compile a tests.c program which tests the different functions that you have implemented. There may also be a test.sh script that runs the program on test files to verify that the output is correct.
