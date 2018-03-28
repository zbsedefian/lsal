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
int getFormatWidth( char* buf, 
                    char* currArg,
                    struct dirent **namelist, 
                    struct stat mystat, 
                    int n, 
                    int choice);

int main (int argc, char *argv[]) 
{
    DIR *pDir;
    //struct dirent *pDirent;
    struct dirent **namelist;
    struct stat mystat;
    static const struct stat emptystat;
    int i, j, n, formatWidthLink = 0, formatWidthSize = 0, total = 0;
    size_t nonNullArgCount = 1;
    char buf[512];
    char currArg[512];
    char *nonNullArgs[argc-1];

    // Both help ensure alphabetical order
    setlocale(LC_ALL, ""); 

    // Check if files found. Add to array if they are.
    if (argc > 1) 
    {
        for (i = 1; i < argc; i++) 
        {
            pDir = opendir(argv[i]);
            if (pDir == NULL) {
                printf("lsal: cannot access '%s': No such file or directory\n", 
                    argv[i]);
            }
            else {
                nonNullArgs[i-0] = malloc(strlen(argv[i]) + 1);
                strcpy(nonNullArgs[i-0], argv[i]); 
            }
            
        }
        nonNullArgCount = sizeof(nonNullArgs) / sizeof(nonNullArgs[0]);;
        qsort(&nonNullArgs[0], nonNullArgCount - 1, sizeof(char *), cmpstringp); // sorts argv
    }



    



    for (i = 0; i < nonNullArgCount; i++) 
    {
        if (argc == 1) 
        {
            pDir = opendir("."); // open current directory
            // For alphabetical sorting of directories in current dir
            n = scandir(".", &namelist, NULL, alphasort);
            strcpy(nonNullArgs[i], ".");
        }
        else 
        {
            if (i == 0) i = 1;  // if argc > 1, you want to start at index 1
            pDir = opendir(nonNullArgs[i]);
            // For alphabetical sorting of directories
            n = scandir(nonNullArgs[i], &namelist, NULL, alphasort);
        }
        
        // Error if directory not found
        if (pDir != NULL) 
        {
            printf("lsal: cannot access '%s': No such file or directory\n", 
                nonNullArgs[i]);
        }
        else 
        {
            // Print directory name
            if (argc > 2) printf("%s:\n", nonNullArgs[i]);
            
            // Get format width for links and file size, get total
            formatWidthLink = getFormatWidth(buf, nonNullArgs[i], namelist, mystat, n, 0);
            formatWidthSize = getFormatWidth(buf, nonNullArgs[i], namelist, mystat, n, 1);
            total = getFormatWidth(buf, nonNullArgs[i], namelist, mystat, n, 2);

            // Print total blocks
            printf("total %d\n", total);

            // Print contents of directory
            for (j = 0; j < n; j++) 
            {
                sprintf(buf, "%s/%s", nonNullArgs[i], namelist[j]->d_name);
                mystat = emptystat;
                lstat(buf, &mystat);
                
                if (namelist[j]->d_type == DT_DIR || 
                    namelist[j]->d_type == DT_REG) 
                {
                    printf("%s %*ld %s %s %*ld %s %c[1;34m%s%c[0m", 
                        getFilePermissions(mystat.st_mode),
                        formatWidthLink,
                        mystat.st_nlink,
                        getpwuid(mystat.st_uid)->pw_name,
                        getgrgid(mystat.st_gid)->gr_name,
                        formatWidthSize,
                        mystat.st_size,
                        //formatTime(ctime(&mystat.st_mtime)),
                        getLastModifiedTime(buf),
                        27,
                        namelist[j]->d_name,
                        27
                    );
                }

                if (namelist[j]->d_type == DT_DIR) 
                    printf("/");
                printf("\n");
            }
        }    
        if (i < argc-1) printf("\n");
    }

    free(namelist);
    closedir(pDir);
    return 0;
}


// https://linux.die.net/man/3/qsort
static int cmpstringp(const void *p1, const void *p2) 
{
    /* The actual arguments to this function are "pointers to
       pointers to char", but strcmp(3) arguments are "pointers
       to char", hence the following cast plus dereference */

   return strcmp(* (char * const *) p1, * (char * const *) p2);
}


// Function name a little inaccurate -- 
// this will return format width but it will
// also return total blocks
int getFormatWidth( char* buf, 
                    char* currArg,
                    struct dirent **namelist, 
                    struct stat mystat, 
                    int n, 
                    int choice) 
{
    int j, formatWidth = 0;
    for (j = 0; j < n; j++) 
    {
        sprintf(buf, "%s/%s", currArg, namelist[j]->d_name);
        stat(buf, &mystat);

        if (choice == 0) 
        {
            if (formatWidth < (int)floor(log10(abs(mystat.st_nlink))) + 1)
                formatWidth = (int)floor(log10(abs(mystat.st_nlink))) + 1;
        }
        else if (choice == 1)
        {
            if (formatWidth < (int)floor(log10(abs(mystat.st_size))) + 1)
                formatWidth = (int)floor(log10(abs(mystat.st_size))) + 1;
        }
        else if (choice == 2)
        {
            // Get total blocks
            formatWidth += mystat.st_blocks;
        }
    }

    if (choice == 2)
        return formatWidth / 2;
    return formatWidth;
}


// Returns the last time a file or directory was modified
const char* getLastModifiedTime(char *filePath) 
{
    struct stat attrib;
    stat(filePath, &attrib);
    static char date[10] = {0};
    strftime(date, 15, "%b %02d %H:%M", localtime(&(attrib.st_mtime)));
    return date;
}


// Converts integer file permission to string
const char* getFilePermissions(int mode) 
{
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