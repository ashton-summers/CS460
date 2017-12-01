#include "ucode.c"
int serial1, serial2, i;
int children[3];

int main()
{
    int in, out;
    
    printf("Ashton's INIT: form a login proc on console\n %c", 0);
    
    for(i = 0; i < 3; i++)
    {
        children[i] = fork();
        
        if(i == 0)
        {
            open("/dev/ttyS0", O_RDONLY);
            open("/dev/ttyS0", O_WRONLY);
        }
        else if(i == 1)
        {
             open("/dev/ttyS1", O_RDONLY);
            open("/dev/ttyS1", O_WRONLY);
        }
        else if(i == 2)
        {
            open("/dev/tty0", O_RDONLY);
            open("/dev/tty0", O_WRONLY);
        }
        
        if(!children[i])
        {
            login();
        }
    }
    
    parent();
             
}

int login()
{
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

int parent()
{
    int pid, status;
    while(1)
    {
        printf("Ashton's INIT: wait for ZOMBIE child\n");
        pid = wait(&status);
        
        for(i = 0; i < 3; i++)
        {
            if(pid == children[i])
            {
                
                children[i] = fork();
                
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








