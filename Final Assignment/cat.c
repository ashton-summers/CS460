
#include "ucode.c"

char file[64];

int main(int argc, char *argv[])
{
    int fd, n, nbytes, i, returnKey = '\r';
    char buffer[4096];

    // No file entered, cat from stdin.
    if (argc < 2)
    {
        fd = 0; 
        nbytes = 1;
    }
    else // File entered so open it.
    {
        strcpy(file, argv[1]);

        fd = open(file, O_RDONLY);   
       
        if (fd < 0)
        {
            exit(1);
        }
       
        nbytes = 1024;

    }

    n = read(fd, buffer, nbytes);

    while(n > 0)
    {
       for (i = 0; i < n; i++)
       {
           // Cat from stdin.
           if (fd == 0)
           {
                // Return key found, replace with new line.
	            if (buffer[i]=='\r')
	                buffer[i]='\n';     

                 // Write the byte.
                 write(1, &buffer[i], 1);
                 
                 // If newline is found, write return key.
                 if ((buffer[i]=='\n'))
                 {
                    write(2, &returnKey, 1);
	             }

             }
             else // Not stdin, so write from normal file.
             {
                write(1, &buffer[i], 1);
                
                // Write return key to return to start of line.
                if (buffer[i]=='\n')
	                write(2, &returnKey, 1);
	         }
        }

       n = read(fd, buffer, nbytes);
    }
    exit(0);
}

             

