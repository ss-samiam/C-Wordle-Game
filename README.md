# C-Wordle-Game

This program was developed as part of a course (CSSE2310 - Computer Systems Principles and Programming) I undertook at the University of Queensland.

The program plays the popular Wordle game in a terminal window. The program is run with the following command line arguments:
```
./wordle [-len (word-length)] [-max (max-guesses)] [dictionary] 
```
+ ``-len`` – if specified, this option argument should be followed by an integer between 3 and 9 inclusive that indicates the length of the word to be used in game play. If the argument is not specified, a default word length of 5 shall be used.
+ ``-max`` – if specified, this option argument should be followed by an integer between 3 and 9 inclusive that indicates the maximum number of guesses that will be permitted in game play. If the argument is not specified, a default value of 6 shall be used.
+ ``dictionary`` – the last argument (if present) is the path name of the dictionary file, i.e. the name of a file containing words – one per line. Each line (including the last) is assumed to be terminated by a newline character (\n) only. There should be no blank lines and no words are longer than 50 characters.

