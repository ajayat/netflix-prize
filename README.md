# Programming Project

This project is a recommendation system for movies. It uses the Netflix Prize dataset published by Netflix in 2006.

## Get Started

This project is using two submodules:

- the [Unity](https://www.throwtheswitch.org/unity)
framework for testing.

- the [Doxygen](https://www.doxygen.nl/download.html) tool for documentation generation, with the [Doxygen Awesome](https://jothepro.github.io/doxygen-awesome-css/) theme.

First, clone the project and its submodules:

```sh
git clone https://gitlab.unistra.fr/jayat/projet-programmation-2023
cd projet-programmation-2023
git submodule init && git submodule update
```

### Dependencies

Install `make`, `gcc`, `doxygen` and [Zstandard](https://github.com/facebook/zstd) compression algorithm:

#### Ubuntu

```sh
sudo apt update
sudo apt install make gcc doxygen zstd
```

#### Arch Linux

```sh
sudo pacman -Syu
sudo pacman -S make gcc doxygen zstd
```

### Build

Build the project with the `make` command. It will generate the `main` executable in the current working directory.

You can run all tests with the `make tests` command:

```sh
make clean && make tests
```

## How to use

After building the project, you can run the `./main` executable to start the program.

### Options

| Flag |     Argument     | Description                       |
| :--: | :--------------: | :-------------------------------- |
| `-r` |    LIKES_FILE    | List of movies liked by the user. |
| `-n` |        N         | Length of the recommendation list the algorithm will give. |
| `-d` |    DIRECTORY     | The path of the folder where files corresponding to results will be saved. |
| `-l` |      LIMIT       | Forbidden to take in acount ratings with a date greater than the LIMIT. |
| `-s` |    MOVIE_ID      | Give statistics about the movie with the identifier MOVIE_ID. |
| `-c` |      IDS         | Allow to take into account only the ratings of the customers with given identifiers. |
|  ∅  |  NB_CUSTOMER_IDS | Number of given customer ids. |
| `-b` |       IDS        | Allow to not take into account the ratings of the customers with given identifiers. |
|  ∅  | NB_BAD_REVIEWERS | Number of given bad reviewers. |
| `-e` |       MIN        | Allow to take into account only customers who rated at least MIN movies. |
| `-t` |       TIME       | Precise the executive time of the algorithme. |
| `-p` |     PERCENT      | Percentage (between 0 and 1) to quantify the importance of personnal recommendations over popular recommendations. |

Note that options `-r`, `-n`, `-t` and `-p` are not used for statistics processing.

### Examples

#### Gives the 10 best recommendations from liked movies

```sh
./main -r likes.txt -n 10
```

The `likes.txt` file contains titles of movies liked by the user.
You can also give a list of movie ids directly from the command line.

```sh
./main -r 872 996 463 5582 -n 10 -p 0.8
```

Add the `-p` option to give a percentage to quantify the importance of personalized recommendations over popularity.

#### Gives statistics about a movie

```sh
./main -s 42
```

It will create a file named `stats_mv_000042.csv` in the `stats` folder, containing the min, max and average score of the movie 42.

## Results

Our algorithm gives a RMSE of **0.971** on the test set.

In comparison, the RMSE of the Cinematch, the Netflix algorithm in 2006, was 0.9525.

## About

This project was realized by Adrien JAYAT and Ysée JACQUET.
