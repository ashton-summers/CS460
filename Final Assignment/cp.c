#include "ucode.c"

/*
 * Finds the directory name in a filepath provided.
 * @param char *filepath: The filepath
 * @returns: A pointer to the dirname
*/
char *dirname(char *path)
{
    char *temp = path;
    
    // Iterate to the end of the string.
    while(*temp)
        temp++;
    
    // Go until we find a '/' character.
    while(*temp != '/' && temp != path)
    {
        temp--;
    }
    
    // if temp and path point to same place. 
    if(temp == path)
    {
        if(*path == '/')
            return '/';
        else
            return '.';
    }
    
    // Null terminate so we can end dirname string.
    *temp = 0;
    return path;
}


/*
 * Determines if a file is a dir or not.
 * @param int mode: The file's mode
 * @returns: nothing.
*/
int S_ISDIR(int mode)
{
    if ((mode & 0040000)==0040000)
        return 1;
    return 0;
}

/*
 * Determines if a file is regular or not.
 * @param int mode: The file's mode
 * @returns: nothing.
*/
int S_ISREG(int mode)
{
    if ((mode & 0100000)==0100000)
        return 1;
    return 0;
}

int main(int argc, char *argv[])
{
    char *src, *dest;
    int srcIsDir, destIsDir;
    STAT statSrc, statDest;
    
    // Make sure we aren't accessing invalid array bounds.
    if(argc > 2)
    {
        src = argv[1];
        dest = argv[2];
    }
   
    printf("************* ASHTON's COPY *************%c\n", 0);
    
    // The function wasn't passed any arguments.
    if(!src && !dest)
    {   
        printf("Usage: cp <file/dir> <file/dir>%c\n", 0);
        exit(1);
    }
    
    // Stat the arguments.
    stat(src, &statSrc);
    stat(dest, &statDest);
    
    // Check to see if src or dest are dirs.
    srcIsDir = S_ISDIR(statSrc.st_mode);
    destIsDir = S_ISDIR(statDest.st_mode);
    
    
    // If neither args are directories.
    if(!srcIsDir && !destIsDir)
    {
        // Do a normal copy.
        myCp(src, dest);
    }
    else if(!srcIsDir && destIsDir) // Copying a file to a directory.
    {
        // Copy file to dir. 
        cpFileToDirectory(src, dest);
    }
    else if(srcIsDir && destIsDir) // Both are directories
    {
        cpDirToDir(src, dest);
    }
    else
    {
        printf("Cannot copy a directory to a file%c\n", 0);
        exit(1);
    }
}

/*
 * Copies a file to another file.
 * @param char *src: Contents to copy
 * @param char *dest: The place to save the contents.
 * @returns: nothing.
*/
void myCp(char *src, char *dest)
{
    int nbytes = 0;
    int srcFd = open(src, O_RDONLY);
    int destFd = open(dest, O_WRONLY | O_CREAT | O_TRUNC);
    char buf[1024];
    
    if(srcFd < 0)
    {
        printf("The source could not be opened for reading %c\n", 0);
        exit(1);
    }
    else if(destFd < 0)
    {
        printf("The destination could not be opened for reading %c\n", 0);
        exit(1);
    }
    
    // Write the content to the destination.
    while(nbytes = read(srcFd, buf, 1024))
    {
        printf("The buf contains %s\n", buf);
        write(destFd, buf, nbytes);
    }
    
    // File descriptor cleanup.
    close(src);
    close(dest);
    
    exit(0);  
}

/*
 * Copies a file to another directory.
 * @param char *src: Contents to copy
 * @param char *dest: The place to save the contents.
 * @returns: nothing.
*/
void cpFileToDirectory(char *src, char *dest)
{
    int srcFd = open(src, O_RDONLY);
    char dname[64], tempDest[64];
    int slash = '/';
    
    
    if(srcFd < 0)
    {
        printf("The source could not be opened for reading%c\n", 0);
        exit(1);
    }
    
    // Get the name of the directory.
    dirname(dest);
    
    // dirname modifies the dest pointer.
    strcpy(dname, dest);
    
    // Append the directory name and the name of the file to copy to.
    strcat(tempDest, dname);
    strcat(tempDest, &slash);
    strcat(tempDest, src);
    
    // Copy the file to the directory.
    myCp(src, tempDest);
    
    exit(0);
}

/*
 * Copies a directory to another directory.
 * @param char *src: Contents to copy
 * @param char *dest: The place to save the contents.
 * @returns: nothing.
*/
void cpDirToDir(char *src, char *dest)
{
    printf("************** COPYING DIR TO DIR **************\n");
    DIR *dp;
    char dname[64], filename[64], buf[1024];
    char srcBuf[64], destBuf[64], *cp, dirTemp[64];
    int fd = open(src, O_RDONLY), nbytes = 0;
    STAT st;
    
    if(fd < 0)
    {
        printf("Source could not be opened for reading%c\n", 0);
        exit(1);
    }
    
    // Read all blocks of the source directory.
    while(nbytes = read(fd, buf, 1024))
    {
        cp = buf; 
        dp = (DIR *)buf;
        
        // While not at the end of dir entry.
        while(cp < &buf[1024])
        {
            // Copy the dir name and null terminate.
            strncpy(dirTemp, dp->name, dp->name_len);
            dirTemp[dp->name_len] = 0;
            
            
            // If at the first two entries, move to next.
            if(strcmp(dirTemp, ".") == 0 || strcmp(dirTemp, "..") == 0)
            {
                cp += dp->rec_len;
                dp = (DIR *)cp;
                continue;
            }
            
            // Fix up the source and destination buffers.
            strcpy(srcBuf, src);
            strcat(srcBuf, "/");
            strcat(srcBuf, dirTemp);
            
            strcpy(destBuf, dest);
            strcat(destBuf, "/");
            strcat(destBuf, dirTemp);
            printf("src = %s, dst = %s\n", srcBuf, destBuf);
            
            // Stat the source.
            stat(srcBuf, &st);
            
            // If the source is a regular file.
            if(S_ISREG(st.st_mode))
            {
                myCp(srcBuf, destBuf);
            }
            
            // Source is a directory?
            if(S_ISDIR(st.st_mode))
            {
                // Make the destination directory.
                mkdir(destBuf);
                cpDirToDir(srcBuf, destBuf);
            }
            
            // Move to the next entry.
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
        
        
    }
    
    close(fd);
}











