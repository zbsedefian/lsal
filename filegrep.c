#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMARG 3
#define INFILEARG 1
#define OUTFILEARG 2

int main(int argc, char *argv[]){
	// Argument count must equal NUMARG
	if (argc != NUMARG) {
		fprintf(stderr, "%s", "Usage: filegrep PATTERN FILE\n");
		exit(1);
	}

	// Retrive search pattern
	int searchTermLength = strlen(argv[INFILEARG]);
	char *searchTerm = malloc(sizeof(char) * (searchTermLength+4));
	strcpy(searchTerm, argv[INFILEARG]);

	// Open file
	FILE *fin;
	if( (fin = fopen(argv[OUTFILEARG], "r")) == NULL ){
		printf("filegrep: %s: No such file or directory\n", argv[OUTFILEARG]);
		exit(1);
	}

	printf("%s\n", searchTerm);

	return 0;
}