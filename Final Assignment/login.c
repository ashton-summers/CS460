#include "ucode.c"

char *ttyx, *fileName, *filePasswd, *fullName, *program, *homeDir;
char enteredName[64], passwd[64], line[64];

int gid, uid;

int main(int argc, char *argv[])
{
    
    // Close file descriptors 0, 1 inherited from init
    close(0);
    close(1);
    close(2);
    ttyx = argv[1];
    
    // Open argv[1] as in(0), out(1) and err(2)
    open(ttyx, O_RDONLY);
    open(ttyx, O_WRONLY);
    open(ttyx, O_WRONLY);
    
    // Set current tty;
    settty(ttyx);
   
    
    // Check the credentials of the user.
    login();
       
}

int login()
{
    char *fileLine;
    while(1)
    {
        // Open the password file for reading.
        int fd = open("/etc/passwd", O_RDONLY);
        
        // Get the login name.
        printf("ASH login: "); gets(enteredName);
        printf("password: "); gets(passwd);
        
        // Loop through the lines in the file.
        while(fileLine = fgets(line, 64, fd))
        {
               
            // Tokenize the username and password from file.
            fileName = strtok(fileLine, ":");
            filePasswd = strtok(0, ":");
            
            printf("\n--------------- Parsing Info -----------------%c \n", 0);
            printf("Current file line: %s\n", fileLine);
            printf("Name from file: %s\n", fileName);
            printf("Password from file is:%s\n", filePasswd);
            printf("Entered name is:%s\n", enteredName);
            printf("Entered password if:%s\n", passwd);
            printf("--------------- Parsing Info -----------------%c \n", 0);
            
            // Compare credentials with file information.
            if(strcmp(fileName, enteredName) == 0)
            {
                if(strcmp(filePasswd, passwd) == 0)
                {
                    // Successful login. Load the rest of the user data.
                    loadUser(fd); 
                }
            }
        }
        
    }
}

int loadUser(int fd)
{
    printf("\n\n--------------- Loading User Info -----------------%c\n", 0); 
    char *gidStr = strtok(0, ":");
    char *uidStr = strtok(0, ":");
    
    gid = atoi(gidStr);
    uid = atoi(uidStr);
    fullName = strtok(0, ":");
    homeDir = strtok(0, ":");
    program = strtok(0, "\n");
    
    printf("gid: %d\n", gid);
    printf("uid: %d\n", uid); 
    printf("Full name: %s\n", fullName);
    printf("Home Directory: %s\n", homeDir);
    printf("Program to run: %s\n", program);
    
    // Change uid, uid to user's uid gid.
    chuid(uid, gid);
    
    // Change cwd to user's home dir.
    chdir(homeDir);
    
    // Close the password file, file descriptor.
    close(fd);
    
    printf("--------------- Loading Done -----------------%c\n", 0);
    
    // Execute the user's program.
    exec(program);
    
  
}



