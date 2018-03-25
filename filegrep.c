#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMARG 3
#define INFILEARG 1
#define OUTFILEARG 2

int countWords(const char *sentence);

int main(int argc, char *argv[]){

	// Argument count must equal NUMARG
	if (argc != NUMARG) {
		fprintf(stderr, "%s", "Usage: filegrep PATTERN FILE\n");
		exit(1);
	}

	// Retrieve search pattern
	int searchTermLength = strlen(argv[INFILEARG]);
	char *searchTerm = malloc(sizeof(char) * (searchTermLength+4));
	strcpy(searchTerm, argv[INFILEARG]);

	// Open file
	FILE *fin;
	if( (fin = fopen(argv[OUTFILEARG], "r")) == NULL ){
		fprintf(stderr, "filegrep: %s: No such file or directory\n", argv[OUTFILEARG]);
		exit(1);
	}

	char *holdWord = NULL, *holdLine = NULL, *holdText = NULL,
		 *lineToken = NULL, *wordToken = NULL;
	size_t size = 0;
	int totalStrings, i = 0, lineNumber = 1;

	/* Count char total in input file and allocate size in holdText. */
	fseek(fin, 0, SEEK_END); /* Bring pointer to end of input file */
	size = ftell(fin); /* Return length of file */
	rewind(fin); /* Bring pointer back to beginning */

	/* Allocate space for holdText */
	holdText = malloc((size + 1) * sizeof(*holdText));
	fread(holdText, size, 1, fin); /* Insert file data in holdText */
	holdText[size] = '\0'; 
	
	/* Break holdText up into lines */
	for(lineToken = strtok(holdText, "\n"); lineToken != NULL; 
	lineToken = strtok(lineToken + strlen(lineToken) + 1, "\n")){

		/* Dynamically allocate space for each line */
		holdLine = realloc(holdLine, strlen(lineToken) + 1);
		strcpy(holdLine, lineToken);

		/* Get number of strings in the line */
		totalStrings = countWords(holdLine);

		/* Break holdLine up into words, store in holdWords */
		i = 0;
		for(wordToken = strtok(holdLine, " \t"); wordToken != NULL && i < totalStrings; 
		wordToken = strtok(wordToken + strlen(wordToken) + 1, " \t")){ 
			/* Allocate space for word */
			holdWord = realloc(holdWord, strlen(wordToken) + 1);
			strcpy(holdWord, wordToken);

			if (strcmp(searchTerm, holdWord) == 0) {
				printf("%d:%s\n", lineNumber, lineToken);
				break;
			}
			i++;
		}
		lineNumber++;
	}

	// Close file, then free memory.
	if (fclose(fin) == EOF)
		printf("Error closing input file.\n");

	free(holdText);
	free(holdWord);

	return 0;
}


int countWords(const char *sentence){
    int total = 0, contains = 0;
    const char *s = sentence;
    do switch(*s) {
        case '\0': case ' ': case '\t':
            if (contains) { 
            	contains = 0; 
            	total++; 
            }
            break;
        default: contains = 1;
    } while (*s++);
    return total;
}