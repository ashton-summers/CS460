#include "ucode.c"

char input[256];
char cwd[128];
char file[128];

int main(int argc, char *argv[])
{
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$\n", 0);
    printf("Welcome to Ashton's Shell!\n", 0);
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$\n", 0);
    
    
    while(1)
    {
        getcwd(cwd);
        printf("ash@sh:%s$ ", cwd);
        gets(input);
        
        if(input[0] == 0 || input[0] == ' ')
            continue;
        else
            preExecute(input);
            
        memset(input, 0, 256);
    }
}

/*
 * Looks for trivial commands to execute before anything else.
 * Otherwise, it forks a process and preps for non-trivial commands.
 * @param char *input: The command to execute.
 * @returns: -1 for failure and 0 for success.
*/
int preExecute(char *input)
{
    int pid, status = 0;
    int *r;
    char cmd[256], inputTemp[256], *tokLine;
    
    strcpy(inputTemp, input);
    
    tokLine = strtok(inputTemp, " ");
    
    // Check for trivial commands that we can do ourselves.
    if(strcmp(tokLine, "cd") == 0)
    {
        tokLine = strtok(0, " "); 
        
        // If no directory entered, just go to cwd.
        if(!tokLine)
        {
            printf("cd to cwd. No directory entered\n", 0);
            chdir(cwd);
        }
        else
        {
            printf("Cd command. dir = %s\n", tokLine);
            chdir(tokLine);
        }
    }
    else if(strcmp(tokLine, "logout") == 0)
    {
        exit(1);
    }
    else if(strcmp(tokLine, "pwd") == 0)
    {
        getcwd(cwd);
        printf("%s\n", cwd);
    }
    else
    {
        printf("Forking a child process to execute\n", 0);
        pid = fork();
        
        if(pid < 0)
        {
            printf("Failed to fork a child process\n", 0);
            return -1;
        }
        
        // Parent process.
        if(pid)
        {
            printf("Parent %d forked child %d and waits for child to die\n", getpid(), pid);
            pid = wait(&status);
            printf("Child %d has finished executing\n", pid); 
        }
        else
        {
            // This is the child process.
            
            // If we have found a pipe in the input, do the pipe operations.
            if(hasPipe(input) == 1)
            {
                doPipe(input);
            }
            else
            {
                printf("Executing %s\n", input);
                execute(input);
            }
        }
    }
    
    return 0;
}

/*
 * Removes redirection symbols from the input string.
 * They are not needed for execution of a command once
 * redirection has been handled.
 * @param char *input: The string to remove symbols from
 * @returns: The new string without IO symbols
 * e.g cat f >> file would return cat f file
*/
char* removeIOSymbols(char *input)
{
    int i = 0, len = strlen(input);
    char newInputStr[256], *temp = input;
    char *tok;
    
    tok = strtok(temp, " ");
    strcpy(newInputStr, tok);
    
    
    // Tokenize the input, copy only content we want.
    while(tok = strtok(0, " "))
    {
        
        // If the token string is IO redirection, stop concatenation.
        if(strcmp(tok, "<") == 0 || strcmp(tok, ">") == 0 || strcmp(tok, ">>") == 0)
        {
            break;
        }
        else
        {
            strcat(newInputStr, " ");
            strcat(newInputStr, tok);
        }
    }
    
    // Return address to starting point of new string.
    return &newInputStr[0];
}

/*
 * Executes the specified input after checking for IO redirection
 * @param char *cmd: The command to execute
*/
void execute(char *cmd)
{
    int ioType;
    
    // Do we have redirection?
    ioType = hasRedirection(cmd);
    
    // IO redirection found.
    if(ioType > 0)
    {
        // Remove the IO symbols from the string.
        // We lo longer need them after output is redirected.
        cmd = removeIOSymbols(cmd);
        redirect(file, ioType);
    }
    
    // Execute the input.
    exec(cmd);  
}


/*
 * Executed when we have found a pipe in the input.
 * Breaks apart input and executes pipe operations accordingly.
 * @param char *input: The input string.
*/
int doPipe(char *input)
{
    int pd[2], pid;
    
    char *head, tail[256], *tok;
    
    //printf("CREATING PIPE\n", 0);
    head = strtok(input, "|");
    strcpy(tail, strtok(0, " \n"));

    
    // Get any leftlovers that may have been missed.
    if(tok = strtok(0, "\n"))
    {
        // Append a space to separate commands.
        strcat(tail, " ");
        strcat(tail, tok);
    }
    
    //printf("Head command is %s\n", head);
    //printf("Tail command is %s\n", tail);
    
    pipe(pd);
    pid = fork();
    
    // This is the parent process.
    if(pid)
    {
        // Have the parent execute the tail part of command.
        close(0);
        dup2(pd[0], 0);
        close(pd[1]);
        
        // If the tail has more pipes, recurse on it.
        if(hasPipe(tail) == 1)
        {
            doPipe(tail);
        }
        else
        {
            execute(tail);
        }     
    }
    else // Child runs the head portion
    {
        close(1);
        dup2(pd[1], 1);
        close(pd[0]);
        execute(head);
    }
}

/*
 * Searches for '|' char to see if a pipe is found.
 * @param char *input: The input string to iterate over.
 * @returns: 0 for not found and 1 for found.
*/
int hasPipe(char *input)
{
    char *temp = input;
    
    
    while(*temp++)
    {
        if(*temp == '|')
        {
            return 1;
        }
    }
    
    return 0;
}

/*
 * Searches for IO redirection chars in input string
 * >> for append, < for read, > for write.
 * @param char *input: The input string
 * @returns: 1 for >>, 2 for > and 3 for <, -1 for does not exist.
*/
int hasRedirection(char *input)
{
    int i = 0, retVal = -1;
    char *temp = input;
    
    while(*temp++ != 0)
    {
        if(*temp == '>')
        {
            if(*(temp + 1) == '>') // Append redirection.
            {
                retVal = 1;
                break;
            }
            else // Write io redirection
            {
                retVal = 2;
                break;
            }
        }
        else if(*temp == '<') // stdin io redirection.
        {
            retVal = 3;
            break;
        }
        
    }
    
    // If IO redirection was found, search for filename.
    if(retVal > 0)
    {
        getFileName(input);
    }
   
    return retVal;
}

/*
 * Finds the filename in a string to use for IO redirection.
 * e.g. cat file > fileToFind: fileToFind would be the found filename
 * @param char *input: The input string to iterate over.
 * @returns: 0 for success.
*/
int getFileName(char *input)
{
    char tempInput[256];
    char *tok, *temp;
    
    // Copy to temp buffer. 
    strcpy(tempInput, input);
    
    
    // temp is used to iterate over chars.
    temp = tempInput;
    
    while(*temp++ != 0)
    {
        
        if(*temp == '>' || *temp == '<')
        {
            // If next char is '>' then move past it
            if(*(temp + 1) == '>') 
            {
                temp += 2;
            }
            
            temp++;
            
            // Grab the file name.
            tok = strtok(temp, " ");
            
            // Break out of loop; filename found.
            break;
        }
    }
    
    // Copy file name to global file variable.
    strcpy(file, tok);
    
    return 0; 
}

/*
 * Given an int to the type of redirection to do,
 * and a file name, the function redirects IO accordingly.
 * @param char *file: The name of the file to open. 
 * @param int redirectionType: Type of IO redirection to do.
 * @returns: 0 for success.
*/
int redirect(char *file, int redirectionType)
{
    switch(redirectionType)
    {
        // Append IO redirection
        case 1:
            close(1);
            open(file, O_APPEND | O_WRONLY | O_CREAT);
            break;
        // Write IO redirection.
        case 2:
            close(1);
            open(file, O_WRONLY | O_CREAT);
            break;
        // Read from stdin IO redirection.
        case 3:
            close(0);
            open(file, O_RDONLY);
            break;
    }
    
    return 0;
}





















