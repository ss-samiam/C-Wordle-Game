all: wordle
wordle: wordle.c
	gcc -Wall -pedantic -std=gnu99 -I/local/courses/csse2310/include -L/local/courses/csse2310/lib -lcsse2310a1 -o $@ $< -g 
