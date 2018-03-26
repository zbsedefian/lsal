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

char* formatTime(char* str);
const char* getFilePermissions(int mode);

int main (int argc, char *argv[]) {
    
    struct dirent *pDirent;
    DIR *pDir;
    int i;

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
       
           // Print contents of directory
        while ((pDirent = readdir(pDir)) != NULL) {

            char *fd = pDirent->d_name;
            struct stat buf;

            stat(fd, &buf);
            //printf("buf.st_mode is %d\n\n", buf.st_mode);

            if (pDirent->d_type == DT_DIR || pDirent->d_type == DT_REG)
                printf("%s %ld %s %d %ld %s %s", getFilePermissions(buf.st_mode),
                                            buf.st_nlink,
                                            getpwuid(buf.st_uid)->pw_name,
                                            // getgrgid(
                                            //      buf.st_gid
                                            // )->gr_name
                                            buf.st_gid
                                            ,
                                            buf.st_size,
                                            formatTime(ctime(&buf.st_ctime)),
                                            pDirent->d_name
                );


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