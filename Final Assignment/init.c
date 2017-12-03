#include "ucode.c"
int serial1, serial2, i;
int children[3];

int main()
{
    int in, out;
    
    printf("Ashton's INIT: form a login proc on console\n %c", 0);
    
    // Fork 3 child processes
    for(i = 0; i < 3; i++)
    {
        children[i] = fork();
        
        // If child process is executing.
        if(!children[i])
        {
            login();
        }
    }
    
    parent();
             
}

/*
 * Executes the login command to the specified
 * serial port.
*/
void login()
{
    // Call login from respective child processes.
    if(i == 0)
    {
        exec("login /dev/ttyS0");
    }
    else if(i == 1)
    {
        exec("login /dev/ttyS1");
    }
    else if(i == 2)
    {
        exec("login /dev/tty0");
    }
}

/*
 * When a child process dies, this function is called
 * to fork another login process.
*/
void parent()
{
    int pid, status;
    while(1)
    {
        printf("Ashton's INIT: wait for ZOMBIE child\n");
        pid = wait(&status);
        
        for(i = 0; i < 3; i++)
        {
            // Check the dead process's pid
            if(pid == children[i])
            {
                // Fork another child process.
                children[i] = fork();
                
                // Parent process. 
                if(children[i])
                    continue;
                else
                {
                    if(i == 0)
                        exec("login /dev/ttyS0");
                    else if(i == 1)
                        exec("login /dev/ttyS1");
                    else
                        exec("login /dev/tty0");
                }
                
            }
        }
        
        printf("Ashton's INIT: I just buried an orphan child proc %d\n", pid);
    }
}








