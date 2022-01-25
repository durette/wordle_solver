# wordle_solver
Wordle solver written in C using the default Linux dictionary

# Context
This is a solver for this popular game.
https://www.powerlanguage.co.uk/wordle/

It builds and uses a histogram of letter frequencies to score the words according to the frequencies of their letters. Every guess has a chance of being correct; there are no sacrificial moves. If the current guess isn't right, we want to eliminate as many words as possible from the dictionary, so every guess is based on the highest letter modality.

## Prerequisite:
Dictionary file stored in <code>/usr/share/dict/words</code>

## Usage:
    gcc -O2 -o wordle_solver wordle_solver.c -Wall -Wextra
    ./wordle_solver

* It'll give you a word to try.
* Enter the color codes for the results, where orange = O, black = B, green = G. Example: OOBOB (then press enter)
* If the word is missing from their dictionary, type X to get a new guess.

