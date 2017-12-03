#include "ucode.c"

char buffer[1024];
int n, nbytes, i, backSlash = '\r', dstFD, srcFD;

int main(int argc, char *argv[])
{
    char *file1 = argv[1];
    char *file2 = argv[2];
    char tty[128];
    
    //printf("\n$$$$$$$$$$$$$$$$$$$$$$$\n",0);
    //printf("ASHTON'S L2U IN ACTION\n", 0);
    //printf("$$$$$$$$$$$$$$$$$$$$$$$\n", 0);
    
    // If only one file was given, write to stdout.
    if(file1 && !file2)
    {   
        dstFD = open(file1, O_WRONLY);
       
        nbytes = 1;
    
        l2u(dstFD, srcFD);
    }
    else if(file1 && file2)
    {
        // Open the destination file for writing.
        // Create it if it does not exist.
        dstFD = open(file2, O_WRONLY | O_CREAT);
        
        // Open source file for reading.
        srcFD = open(file1, O_RDONLY);
        
        nbytes = 4096;
        
        // Now convert!
        l2u(dstFD, srcFD);
        
        close(dstFD);
        close(srcFD);
    }
    else
    {
        // Use stdin and stdout for conversion.
        int out = 1;
        int in = 0;
        nbytes = 1;
        
        l2u(out, in);
    }
    
}

/*
 * Converts the contents of the destination file
 * from lowercase to uppercase.
 * @param: int dstFD: The destination file descriptor
 * @param: int srcFD: The source file descriptor
 * @returns: -1 for failure.
*/
void l2u(int dstFD, int srcFD)
{
    char *line;
    char lineToConvert[128];
    
    // If the source file does not exist.
    if(srcFD < 0)
    {
        printf("The source file entered does not exist\n", 0);
        printf("Cannot convert from lower to upper\n", 0);
        return -1;
    }
    
    n = read(srcFD, buffer, nbytes);
    
    while(n > 0)
    {
    
        /*Convert buffer to uppercase. 
          This function is in ucode.c */
        toUpper(buffer);
        
        for(i = 0; i < n; i++)
        {
            // If return key was found
            if(buffer[i] == '\r')
            {
                buffer[i] = '\n';
            }
            
            // Write the byte.
            write(dstFD, &buffer[i], 1);
            
            // If new line is found, write return key
            if(buffer[i] == '\n')
            {
                write(2, &backSlash, 1);
            }
        }
        
     
        // Read some more bytes (;
        n = read(srcFD, buffer, nbytes);
    }
   
    exit(0);
}

























