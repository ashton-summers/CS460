/********** test.c file *************/
#include "ucode.c"

int main(int argc, char *argv[ ])
{
  int i;


  int pid = getpid();
  printf("proc %d running ASHTON'S TEST\n", pid);
  char test[] = "this is a string";
  
  
  char *strr = strtok(test, " ");
  
  printf("argc = %d\n", argc);
  for (i=0; i<argc; i++)
    printf("argv[%d] = %s\n", i, argv[i]);
  printf("exit\n");
}

