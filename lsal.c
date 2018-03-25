#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

char* removeFinalChar(char* name);

int main (int argc, char *argv[]) {
    
    int len;
    struct dirent *pDirent;
    DIR *pDir;
    int i;

    for (i = argc-1; i > 0; i--) {
	    // Point to current directory if none specified
	    if (argc == 1) pDir = opendir(".");
	    else pDir = opendir(argv[i]);

	    // If directory not found, exit
	    if (pDir == NULL) {
	        printf ("lsal: cannot access '%s': No such file or directory\n", argv[i]);
	        return 1;
	    }
 
 		// Print directory name
	    if (argc > 1) printf("%s:\n", argv[i]); 
	   
	   	// Print contents of directory
	    while ((pDirent = readdir(pDir)) != NULL) {

			char *fd = pDirent->d_name;
			struct stat buf;

			stat(fd, &buf);
			
	    	if (pDirent->d_type == DT_DIR || pDirent->d_type == DT_REG)
	        	printf("%d %ld %d %d %ld %s %s", buf.st_mode,
	        									buf.st_nlink,
	        									buf.st_uid,
	        									buf.st_gid,
	        									buf.st_size,
	        									removeFinalChar(ctime(&buf.st_mtime)),
	        									pDirent->d_name);


	        if (pDirent->d_type == DT_DIR) printf("/");

	     	printf("\n");
	    }
	    
	    if (i > 1) printf("\n");
    }
    closedir(pDir);
    return 0;
}


// Because st_mtime returns a string ending with new line character
char* removeFinalChar(char* str) {
    int i = 0;
    while(str[i] != '\0')
        i++;
    str[i-1] = '\0';
    return str;
}