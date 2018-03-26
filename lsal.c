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
const char* getLastModifiedTime(char *filePath);

int main (int argc, char *argv[]) {
    
    DIR *pDir;
    struct dirent **namelist;
    struct stat mystat;
    int i, j, n;
    char buf[512];
    setlocale(LC_ALL, ""); // ensures alphabetical order

    for (i = argc-1; i > 0; i--) {
        // Point to current directory if none specified
        if (argc == 1) pDir = opendir(".");
        else pDir = opendir(argv[i]);
        
        // For alphabetical sorting
        n = scandir(argv[i], &namelist, NULL, alphasort);
        
        // If directory not found, exit
        if (pDir == NULL) {
            printf("lsal: cannot access '%s': No such file or directory\n", argv[i]);
            return 1;
        }

        // Print directory name
        if (argc > 1) printf("%s:\n", argv[i]); 

        // Print total blocks or whatever that is
        printf("total %d\n", 4);

        // Print contents of directory
        j = 0;
        while (j < n) {
            sprintf(buf, "%s/%s", argv[i], namelist[i]->d_name);
            stat(buf, &mystat);

            if (namelist[j]->d_type == DT_DIR || namelist[j]->d_type == DT_REG) {
                printf("%s %2ld %s %s %4ld %s %s", 
                    getFilePermissions(mystat.st_mode),
                    mystat.st_nlink,
                    getpwuid(mystat.st_uid)->pw_name,
                    getgrgid(mystat.st_gid)->gr_name,
                    mystat.st_size,
                    //formatTime(ctime(&mystat.st_mtime)),
                    getLastModifiedTime(namelist[j]->d_name),
                    namelist[j]->d_name
                );
            }

            if (namelist[j]->d_type == DT_DIR) 
                printf("/");
            printf("\n");
            j++;
        }
        
        if (i > 1) printf("\n");
    }
    free(namelist);
    closedir(pDir);
    return 0;
}


// Returns the last time a file or directory was modified
const char* getLastModifiedTime(char *filePath) {
    struct stat attrib;
    stat(filePath, &attrib);
    static char date[10];
    time_t t = time(0);
    strftime(date, 16, "%b %2d %H:%M ", localtime(&(attrib.st_mtime)));
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


// // Sort file/dir names
// static int sortFiles(const struct dirent *ent) {
//     if (ent->d_name[0] == '.')
//         return 0;
//     return 1;
// }