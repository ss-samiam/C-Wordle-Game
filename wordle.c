#include <stdio.h>
#include <stdlib.h>
#include <csse2310a1.h>
#include <string.h>
#include <ctype.h>

#define DEFAULT_LENGTH 5
#define DEFAULT_MAX 6
#define DEFAULT_FILE "/usr/share/dict/words"
#define DICTIONARY_MAX_CHAR 50
#define GUESS_MAX_CHAR 52
#define HYPHEN '-'
#define EXIT_SUCCESS 0
#define EXIT_USAGE_ERROR 1
#define EXIT_DICTIONARY_ERROR 2
#define EXIT_EOF  3
    
/* usage_error()
 * −−−−−−−−−−−−−−−
 * Exits the program with the usage error message.
 *
 * Returns: Exit code 1
 */
void usage_error() {
    fprintf(stderr, 
	    "Usage: wordle " 
	    "[-len word-length] [-max max-guesses] [dictionary]\n");
    exit(EXIT_USAGE_ERROR);   
}

/* int_to_string()
 * −−−−−−−−−−−−−−−
 * Converts a given integer to a string (char*).
 *
 * number = the integer to be converted
 *
 * Returns: the string converted integer
 * Reference: This code is taken from stackoverflow
 */
// The following code is inspired by the code at: 
// https://stackoverflow.com/questions/8257714
char* int_to_string(int number) {
    int length = snprintf(NULL, 0, "%d", number); // Determines length of int
    char* str = malloc((length + 1) * sizeof(char));
    snprintf(str, length + 1, "%d", number);
    return str;
}

/* load_argument()
 * −−−−−−−−−−−−−−−
 * Extracts and handles "-len" and "-max" arguments from the command line and 
 * returns accordingly to whichever specified.
 *
 * Precondition: argument can only be specified as "-len" or "-max"
 *
 * argument: argument to be loaded
 * argc: argument count
 * argv: argument vector
 *
 * Returns: The "-len" or "-max" value initialised by the user
 */
int load_argument(char* argument, int argc, char** argv) {
    int maxLength = DEFAULT_LENGTH;
    int maxGuess = DEFAULT_MAX;
    // Determine if user has entered a -len or -max argument, if so, extract
    // the number entered
    for (int i = 1; i <= argc - 1; i++) {
	if (!strcmp(argv[i], "-len")) {
	    if ((argc - 1) > i) {
		maxLength = atoi(argv[i + 1]);
	    }
	}
	if (!strcmp(argv[i], "-max")) {
    	    if ((argc - 1) > i) {
		maxGuess = atoi(argv[i + 1]);
	    }
	}
    }
    if ((maxLength < 3 || maxLength > 9) || (maxGuess < 3 || maxGuess > 9)) {
	usage_error();
    }
    if (!strcmp(argument, "-len")) {
    	return maxLength;
    } else { // Case for when argument is "-max". Enforced by precondition. 
	return maxGuess;
    }
}

/* expected_arguments()
 * −−−−−−−−−−−−−−−
 * Calculates the expected number of arguments had the user entered in the
 * correct format.
 *
 * argc: argument count
 * argv: argument vector
 *
 * Returns: the expected number of arguments
 */
int expected_arguments(int argc, char** argv) {
    int lenExist = 0;
    int maxExist = 0;
    int expected = 0;
    
    // Check for valid inputs of -len and -max, if so, the expected argument is
    // incremented and the respective argument flags are also incremented
    for (int i = 1; i <= argc - 1; i++) {
	if (strlen(argv[i]) < 1) {
	    usage_error();
	}
	if (!strcmp(argv[i], "-len") && i < (argc - 1) && !lenExist) {
	    if (strlen(argv[i + 1]) == 1) {
		lenExist++;
		expected += 2;
	    }
	}
	if (!strcmp(argv[i], "-max") && i < (argc - 1) && !maxExist) {
	    if (strlen(argv[i + 1]) == 1) {
		maxExist++;
		expected += 2;
	    }
	}
    }
    // Check for valid input of dictionary
    if ((argc - 1) == (expected + 1)) {
	if (argv[expected + 1][0] != HYPHEN) {
	    expected++;
	}
    }
    return expected;
}

/* load_file()
 * −−−−−−−−−−−−−−−
 * Loads a given filename.
 *
 * filename: the filename to be loaded
 * expected: expected number of arguments
 * argc: argument count
 *
 * Returns: the loaded file
 * Errors: will return exit code 2 if the file does not exist/cannot be opened
 */
FILE* load_file(char* filename, int expected, int argc) {
    FILE* validFile;
    char* defaultFile = DEFAULT_FILE;
    // Check for valid file format if the user has entered one
    if (expected % 2 != 0) {
	validFile = fopen(filename, "r");
	if (validFile == NULL) {
	    fprintf(stderr, 
		    "wordle: dictionary file \"%s\" cannot be opened\n", 
		    filename);
	    exit(EXIT_DICTIONARY_ERROR);
	}
	return validFile;
    }
    validFile = fopen(defaultFile, "r");
    return validFile;
}

/* word_exist()
 * −−−−−−−−−−−−−−−
 * Checks if a word exists in a given dictionary.
 *
 * dictionary: the dictionary to be checked on
 * guess: the guess entered by the user
 *
 * Returns: 1 (True) if the word exists otherwise return 0 (False)
 */
int word_exist(char** dictionary, char* guess) {
    char* word;
    guess[strcspn(guess, "\n")] = '\0'; // Replaces newline with a null
    for (int i = 1; i <= atoi(dictionary[0]); i++) {
	word = dictionary[i];
	if (!strcasecmp(word, guess)) {
	    return 1;
	}
    }
    return 0;
}

/* load_dictionary()
 * −−−−−−−−−−−−−−−
 * Reads the dictionary file and loads it into memory.
 *
 * file: the dictionary file to be read
 * wordLength: length of the word used in game play
 *
 * Returns: the dictionary
 */
char** load_dictionary(FILE* file, int wordLength) {
    int wordNumber = 0;
    char word[DICTIONARY_MAX_CHAR];
    
    // Initialises a malloc with enough size to store every word present
    char** dictionary = (char**) malloc(sizeof(char*));
    
    //Iterates through the dictionary and assigns a pointer to each one
    while (fgets(word, DICTIONARY_MAX_CHAR, file)) {
	word[strcspn(word, "\n")] = '\0';
	// Only assigns a pointer when they are the valid length
	if (strlen(word) == wordLength) {
	    wordNumber++;
	    // Resizing the memory to store an additional word
	    dictionary = (char**) realloc(dictionary,
		    (wordNumber + 1) * sizeof(char*));
	    // Allocate enough space and store that word to the pointer
	    dictionary[wordNumber] 
		    = (char*) malloc((strlen(word) + 1) * sizeof(char));
	    strcpy(dictionary[wordNumber], word);
	}
    }

    // Allocate first pointer to the number of valid words in the file
    dictionary[0] = int_to_string(wordNumber);
    return dictionary;
}
	    
/* free_dictionary()
 * −−−−−−−−−−−−−−−
 * Frees every pointer assigned to the dictionary
 *
 * dictionary: the dictionary with pointers to be freed
 */
void free_dictionary(char** dictionary) {
    int dictionarySize = atoi(dictionary[0]);
    // Frees all the pointer to a pointer in the dictionary
    for (int i = dictionarySize; i >= 0; i--) {
	free(dictionary[i]);
    }
    free(dictionary);
}

/* contains_only_letters()
 * −−−−−−−−−−−−−−−
 * Checks if a string only contains letters.
 *
 * str: the string to be checked upon
 *
 * Returns: 1 (True) if string only contains letters otherwise 0 (False)
 */
int contains_only_letters(char* str) {
    for (int i = 0; str[i]; i++) {
	if (!isalpha(str[i])) {
	    return 0;
	}
    }
    return 1;
}

/* check_guess_validity()
 * −−−−−−−−−−−−−−−
 * Checks if the guess entered by the user is valid. Prints the appropriate
 * response if invalid.
 *
 * dictionary: the dictionary of words
 * wordLength: the length of the word used in game play
 * guess: guess entered by the user
 *
 * Returns: 0 (False) if the guess entered is invalid, otherwise 1 (True).
 */
int check_guess_validity(char** dictionary, int wordLength, char* guess) {
    if ((strlen(guess)) != wordLength) {
	printf("Words must be %d letters long - try again.\n", wordLength);
	return 0;
    } else if (!contains_only_letters(guess)) {
	printf("Words must contain only letters - try again.\n");
	return 0;
    } else if (!word_exist(dictionary, guess)) {
	printf("Word not found in the dictionary - try again.\n");
	return 0;
    }
    return 1;
}

/* string_to_lower()
 * −−−−−−−−−−−−−−−
 * Converts a string to lowercase.
 *
 * str: string to be converted
 *
 * Returns: the converted string
 */
char* string_to_lower(char* str) {
    for (int i = 0; str[i]; i++) {
	str[i] = tolower(str[i]);
    }
    return str;
}

/* find_frequency()
 * −−−−−−−−−−−−−−−
 * Counts the number of times the character has occured in a string
 *
 * str: string for character to count to
 * char: character used to count
 *
 * Returns: the converted string
 */
int find_frequency(char* str, char ch) {
    int occurrence = 0;
    for (int i = 0; i < strlen(str); i++) {
	if (str[i] == ch) {
	    occurrence++;
	}
    }
    return occurrence;
}

/* process_word()
 * −−−−−−−−−−−−−−−
 * Handles the guess entered to give the game play string representation of a 
 * valid guess compared to the answer.
 *
 * guess: valid guess entered by the user
 * answer: the answer word
 *
 * Returns: the game play string representation of a valid guess
 */
char* process_word(char* guess, char* answer) {
    char currentCharacter;
    char correctPosition;
    char incorrectCharacter = HYPHEN;
    int wordLength = strlen(answer);
    // Allocates enough memory to store the string representation
    char* result = (char*) malloc((wordLength + 1) * sizeof(char));
    // Initialise pointer as null to prevent jump of uninitialised values
    for (size_t i = 0; i < wordLength + 1; ++i) {
	result[i] = '\0';
    }
    // Initialises a char array  to store the lowercase string representation
    char lowerResult[wordLength];
    lowerResult[0] = '\0';
    for (int i = 0; i <= wordLength; i++) {
	currentCharacter = guess[i];
	correctPosition = toupper(currentCharacter);
	// Checks if the letter in that position is found in that position of
	// the answer, if so, add it to the result in that position
	if (currentCharacter == answer[i]) {
	    result[i] = correctPosition;
	    strncat(lowerResult, &currentCharacter, 1);
	    // Checks for double ups (result has more letters than the answer)
	    if (find_frequency(lowerResult, answer[i]) 
		    > find_frequency(answer, answer[i])) {
		// When double up is found, replace them with a hyphen
		for (int j = 0; j < strlen(result); j++) {
		    if (result[j] == tolower(answer[i])) {
			result[j] = incorrectCharacter;
		    }
		}
	    }		
	// Handles when letter is found but not in that position of the answer
	} else if (strchr(answer, currentCharacter) != NULL){
	    // Using lowerResult eliminates the case where duplicates persist
	    // when upper and lower characters are compared to one another
	    if (strchr(lowerResult, currentCharacter) == NULL) {
		result[i] = currentCharacter;
		strncat(lowerResult, &currentCharacter, 1);
	    } else {
		result[i] = incorrectCharacter;
		strncat(lowerResult, &incorrectCharacter, 1);
	    }
	// No instance of the letter is found in the answer
	} else {
	    result[i] = incorrectCharacter;
	    strncat(lowerResult, &incorrectCharacter, 1);
	} 
    }
    return result;
}

/* check_correct_guess()
 * −−−−−−−−−−−−−−−
 * Checks if the given guess is the answer
 *
 * guess: valid guess entered by the user
 * answer: the answer word
 *
 * Returns: Exit code 0, if the guess matches the answer, else return from the
 *     function
 */
void check_correct_guess(char** dictionary, char* guess, char* answer) {
    if (!strcasecmp(guess, answer)) {
	printf("Correct!\n");
	free(answer);
	free_dictionary(dictionary);
	exit(EXIT_SUCCESS);
    }
    return;
}

/* process_guesses_left()
 * −−−−−−−−−−−−−−−
 * Handles how the program should prompt the user based on his remaining
 * number of guesses
 * 
 * answer: the answer word
 * wordLength: length of the word use in game play
 * remainingGuess: the number of guesses left
 */
void process_guesses_left(char* answer, int wordLength, int remainingGuess) {
    if (remainingGuess > 1) {
    	printf("Enter a %d letter word (%d attempts remaining):\n",
		wordLength, remainingGuess);
    } else {
	printf("Enter a %d letter word (last attempt):\n", wordLength);
    }
}

int main(int argc, char** argv) {
    // Checks if the arguments entered in stdin are valid
    int expected = expected_arguments(argc, argv);
    if (expected != (argc - 1)) {
	usage_error();
    }
  
    // Loads the arguments entered by the user to their respective variables
    int wordLength = load_argument("-len", argc, argv);
    int remainingGuess = load_argument("-max", argc, argv);
    char* filename = argv[argc - 1];
    FILE* file = load_file(filename, expected, argc);
    char** dictionary = load_dictionary(file, wordLength);
    fclose(file);

    // Initialise the buffer, answer, and prompt for the game loop
    char* answer = get_random_word(wordLength);
    char word[GUESS_MAX_CHAR];
    char* userGuess;
    char* attemptGuess;
    printf("Welcome to Wordle!\n");
    printf("Enter a %d letter word (%d attempts remaining):\n", 
	    wordLength, remainingGuess);
    
    // Main game loop
    while ((userGuess = fgets(word, GUESS_MAX_CHAR, stdin))) {
	userGuess[strcspn(userGuess, "\n")] = '\0';
	userGuess = string_to_lower(userGuess);
	check_correct_guess(dictionary, userGuess, answer);
	// Handles if the guess entered is valid and react accordingly
	if (check_guess_validity(dictionary, wordLength, userGuess)) {
	    attemptGuess = process_word(userGuess, answer);
	    printf("%s\n", attemptGuess);
	    free(attemptGuess);
	    remainingGuess--;
	}
	if (remainingGuess < 1) {
	    break;
	}
	process_guesses_left(answer, wordLength, remainingGuess);
    }
    // End of game loop triggerred by EOF or no guesses left
    if (feof(stdin) || remainingGuess < 1) {
	fprintf(stderr, "Bad luck - the word is \"%s\".\n", answer);
	free_dictionary(dictionary);
	free(answer);
	exit(EXIT_EOF);
    }
}
