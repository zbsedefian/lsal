#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <locale.h>
#include <math.h>

char* formatTime(char* str);
const char* getFilePermissions(int mode);
const char* getLastModifiedTime(char *filePath);
static int cmpstringp(const void *p1, const void *p2);
int getFormatWidth(char* buf, struct dirent **nameList, 
                   struct stat mystat, int n, int choice);

int main (int argc, char *argv[]) {
    
    DIR *pDir;
    //struct dirent *pDirent;
    struct dirent **namelist;
    struct stat mystat;
    int i, j, n, setToOne = 0, formatWidthLink = 0, formatWidthSize = 0;
    char buf[512];

    // Both help ensure alphabetical order
    setlocale(LC_ALL, ""); 
    qsort(&argv[1], argc - 1, sizeof(char *), cmpstringp); // sorts argv

    for (i = 0; i < argc; i++) {
        if (argc == 1) {
            pDir = opendir(".");
            // For alphabetical sorting of directories in current dir
            n = scandir(".", &namelist, NULL, alphasort);
        }
        else {
            if (i == 0)
                i = 1;
            pDir = opendir(argv[i]);
            // For alphabetical sorting of directories
            n = scandir(argv[i], &namelist, NULL, alphasort);
        }
        
        // If directory not found, exit
        if (pDir == NULL) {
            printf("lsal: cannot access '%s': No such file or directory", argv[i]);
        }
        else {
            // Print directory name
            if (argc > 2) printf("%s:\n", argv[i]); 
            // Print total blocks or whatever that is
            printf("total %d\n", 0000);
        }

        // Get format width for links and file size
        formatWidthLink = getFormatWidth(buf, namelist, mystat, n, 0);
        formatWidthSize = getFormatWidth(buf, namelist, mystat, n, 1);

        // Print contents of directory
        for (j = 0; j < n; j++) {
            sprintf(buf, "%s", namelist[j]->d_name);
            stat(buf, &mystat);
            
            if (namelist[j]->d_type == DT_DIR || namelist[j]->d_type == DT_REG) {
                printf("%s %*ld %s %s %*ld %s %s", 
                    getFilePermissions(mystat.st_mode),
                    formatWidthLink,
                    mystat.st_nlink,
                    getpwuid(mystat.st_uid)->pw_name,
                    getgrgid(mystat.st_gid)->gr_name,
                    formatWidthSize,
                    mystat.st_size,
                    //formatTime(ctime(&mystat.st_mtime)),
                    getLastModifiedTime(namelist[j]->d_name),
                    namelist[j]->d_name
                );
            }

            if (namelist[j]->d_type == DT_DIR) 
                printf("/");
            printf("\n");
        }
            
        if (i < argc-1) printf("\n");
    }

    free(namelist);
    closedir(pDir);
    return 0;
}


// Returns the last time a file or directory was modified
const char* getLastModifiedTime(char *filePath) {
    struct stat attrib;
    stat(filePath, &attrib);
    static char date[10] = {0};
    strftime(date, 15, "%b %2d %H:%M", localtime(&(attrib.st_mtime)));
    return date;
}


// Because st_mtime returns a string ending with new line character
char* formatTime(char* str) {
    int i = 0;
    str += 4;  // remove weekday name
    while(str[i] != '\0')
        i++;
    str[i-9] = '\0'; // remove seconds, year, new line char
    return str;
}


// Converts integer file permission to string
const char* getFilePermissions(int mode) {
    static char output[11] = {0};
    strcpy(output, "\0");
    (S_ISDIR(mode))  ? strcat(output, "d") : strcat(output, "-");
    (mode & S_IRUSR) ? strcat(output, "r") : strcat(output, "-");
    (mode & S_IWUSR) ? strcat(output, "w") : strcat(output, "-");
    (mode & S_IXUSR) ? strcat(output, "x") : strcat(output, "-");
    (mode & S_IRGRP) ? strcat(output, "r") : strcat(output, "-");
    (mode & S_IWGRP) ? strcat(output, "w") : strcat(output, "-");
    (mode & S_IXGRP) ? strcat(output, "x") : strcat(output, "-");
    (mode & S_IROTH) ? strcat(output, "r") : strcat(output, "-");
    (mode & S_IWOTH) ? strcat(output, "w") : strcat(output, "-");
    (mode & S_IXOTH) ? strcat(output, "x") : strcat(output, "-");
    strcat(output, "\0");
    return output;
}


// https://linux.die.net/man/3/qsort
static int cmpstringp(const void *p1, const void *p2) {
    /* The actual arguments to this function are "pointers to
       pointers to char", but strcmp(3) arguments are "pointers
       to char", hence the following cast plus dereference */

   return strcmp(* (char * const *) p1, * (char * const *) p2);
}


int getFormatWidth(char* buf, struct dirent **namelist, 
                   struct stat mystat, int n, int choice){
    int j, formatWidth = 0;
    for (j = 0; j < n; j++) {
        sprintf(buf, "%s", namelist[j]->d_name);
        stat(buf, &mystat);
        if (choice == 0) {
            if (formatWidth < (int)floor(log10(abs(mystat.st_nlink))) + 1)
                formatWidth = (int)floor(log10(abs(mystat.st_nlink))) + 1;
        }
        else {
            if (formatWidth < (int)floor(log10(abs(mystat.st_size))) + 1)
                formatWidth = (int)floor(log10(abs(mystat.st_size))) + 1;
        }
    }
    return formatWidth;
}

// // Sort file/dir names
// static int sortFiles(const struct dirent *ent) {
//     if (ent->d_name[0] == '.')
//         return 0;
//     return 1;
// }

