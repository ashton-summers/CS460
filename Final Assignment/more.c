#include "ucode.c"

char buffer[1024];
int writeToStdOut = 0;
int in, out;

int main(int argc, char *argv[])
{
    int fd;
    char *line, tty[128];
    //printf("$$$$$$$$$$$$$$$$$$$$$$ %c\n",0);
    //printf("ASHTON'S MORE IN ACTION %c\n", 0);
    //printf("$$$$$$$$$$$$$$$$$$$$$$ %c\n",0);
    
    // If a filename was provided, open the file and "do more" lol.
    if(argv[1] != 0)
    {
        fd = open(argv[1], O_RDONLY);
        //printf("argv[0] = %s, argv[1] = %s\n", argv[0], argv[1]);
        moreFile(fd);
    }
    else // No file provided.
    {
        fd = dup(0);
        //fd = 0;
        close(0);
        gettty(tty);
        open(tty, O_RDONLY);
        moreFile(fd);
        close(fd);
    }
}

int moreFile(int fd)
{
    char *line, lines[1024], *begin = lines;
    char c;
    int i;
    
    if(fd < 0)
    {
        printf("Cannot execute more. File does not exist or has not been opened%c\n", 0);
        return 0;
    }
    
    // Display 15 lines to start.
    for(i = 0; i < 20; i++)
    {
        line = fgets(buffer, 1024, fd);
        
        if(line == 0)
        {
            break;
        }
        
        printf("%s", line); 
    }
    
    printf("%s", lines);
    
    while(line = fgets(buffer, 1024, fd))
    {
        // Finished with the file contents.
        if(line == 0)
            return 1;
            
        c = getc();
        
        // Display the next line.
        if(c == '\r')
        {
            printf("%s", line);
        }
        else if(c == ' ') // Space char was entered.
        {
            
            memset(lines, 0, 1024);
            strcat(lines, line);
            
            // Print 15 more lines.
            for(i = 0; i < 20; i++)
            {   
                line = fgets(buffer, 1024, fd);
    
                // Make sure there is still data to print.
                if(line == 0)
                {
                    printf("%s", lines);
                    return 1;
                }
                 
                 // Append text to our bulk lines variable.
                 strcat(lines, line);
            }
            
            printf("%s", lines);
        }
        else if(c == 'q') // Quit execution of the program.
        {
            break;
        }
        else
        {
            printc(0);
        }
          
    }
    
    return 1;
}




