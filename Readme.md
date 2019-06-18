
This repository containes a dictionary checking utility.
The utility loads a dictionary file and stores it in a hash table, and a list
files to check. The utility reads all files in order, and stores the words in
each into a red-black tree. Then the utility scans the red-black tree in
lexicographical order, and removes all words that are in the dictionary. The
utility displays all words not found in the dictionary, and suggestion of
words that might have been the intended words in place of those typed.

The utility checks the following criteria for word suggestions:
1. Words that are the same excpet for a letter appearing twice (e.g. 'businness' -> 'business').
2. Words that are the same except for a swapped pair of letters (e.g. 'buisness' -> 'business').
3. Words that are the same except for a double letter written once (e.g. 'busines' -> 'business').
4. Words that are the same except for one homophonic letter (e.g. 'buciness' -> 'business').

The repository is written in the modern C++17 standard, and will not work
on compilers that do not support it.  Tested with gcc 8.3.0.

Building:
Run `make spellChecker` in current folder. [Tested on gcc 8.3.0].

Usage:
`./spellChecker <dict-file> [checked-file-1 checked-file-2 ...]`

This software is written by Itay Knaan-Harpaz AKA KanHar https://github.com/KanHarI/

All rights reserved to the Open University of Israel https://www.openu.ac.il/
