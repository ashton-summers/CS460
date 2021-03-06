#include "ucode.c"

char buffer[1024];
char tty[128];

int main(int argc, char *argv[])
{
    int fd;
    
    //printf("\n$$$$$$$$$$$$$$$$$$$$$$$\n",0);
    //printf("ASHTON'S GREP IN ACTION\n", 0);
    //printf("$$$$$$$$$$$$$$$$$$$$$$$\n", 0);
    
    char *pattern = argv[1];
    char *fileName = argv[2];
    
    // No file was entered in grep command.
    if(fileName == 0)
    {
        fd = dup(0);
        close(0);
        grepFile(fd, pattern);
    }
    else if(pattern == 0)
    {
        // If no pattern was entered then return.
        // 0 for failure.
        printf("Cannot grep. No pattern was entered\n%c", 0);
        exit(1);
    }
    else
    {
        fd = open(argv[2], O_RDONLY);
        grepFile(fd, pattern);
        close(fd);
    }
}

/*
 * Reads through a file and prints lines that contain the 
 * specified pattern
 * @param int fd: The file descriptor to read from
 * @param char *pattern: Pattern to look for in file.
 * @returns: -1 for failure.
*/
int grepFile(int fd, char *pattern)
{
    
    char *line;
    char *retVal;
    int returnKey = '\r';
    
    //printf("---------------------- %c",0);
    //printf("START LINES IN FILE %c", 0);
    //printf("---------------------- %c\n",0);
    
    if(fd < 0)
    {
        printf("Cannot grep. Either file does not exist or is not opened\n%c", 0);
        return -1;
    }
    
    // Loop until no more lines in the file.
    while(line = fgets(buffer, 128, fd))
    {
        if(retVal = strstr(line, pattern) != 0)
        {
            // Write the line and a return key to stdout.
            write(1, line, strlen(line) + 1);
            write(1, &returnKey, 1);
        }
    }
    
    
    //printf("----------------------",0);
    //printf("END LINES IN FILE", 0);
    //printf("----------------------\n",0);
 
    exit(0);
}


