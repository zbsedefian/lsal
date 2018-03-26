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

char* formatTime(char* str);
const char* getFilePermissions(int mode);

int main (int argc, char *argv[]) {
    
    DIR *pDir;
    struct dirent *pDirent;
    struct stat mystat;
    int i;
    char buf[512];
    setlocale(LC_ALL, ""); // alphabetical order

    for (i = argc-1; i > 0; i--) {
        // Point to current directory if none specified
        if (argc == 1) pDir = opendir(".");
        else pDir = opendir(argv[i]);

        // If directory not found, exit
        if (pDir == NULL) {
            printf("lsal: cannot access '%s': No such file or directory\n", argv[i]);
            return 1;
        }

        // Print directory name
        if (argc > 1) printf("%s:\n", argv[i]); 
        printf("total %d\n", 4);

        // Print contents of directory
        while ((pDirent = readdir(pDir)) != NULL) {

            sprintf(buf, "%s/%s", argv[i], pDirent->d_name);
            stat(buf, &mystat);

            if (pDirent->d_type == DT_DIR || pDirent->d_type == DT_REG) {
                printf("%s %2ld %s %s %4ld %s %s", 
                    getFilePermissions(mystat.st_mode),
                    mystat.st_nlink,
                    getpwuid(mystat.st_uid)->pw_name,
                    getgrgid(
                        mystat.st_gid
                    )->gr_name,
                    mystat.st_size,
                    formatTime(ctime(&mystat.st_ctime)),
                    pDirent->d_name
                );
            }

            if (pDirent->d_type == DT_DIR) printf("/");

            printf("\n");
        }
        
        if (i > 1) printf("\n");
    }
    closedir(pDir);
    return 0;
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


// Sort file/dir names
static int sortFiles(const struct dirent *ent) {
    if (ent->d_name[0] == '.')
        return 0;
    return 1;
}