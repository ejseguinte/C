#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXLINE 80
#define MAXARGS 20

void process_input(int argc, char **argv) {
  /* Problem 1: perform system call execvp to execute command     */ 
  /*            No special operator(s) detected                   */
  /* Hint: Please be sure to review execvp.c sample program       */
  /* if (........ == -1) {                                        */  
  /*  perror("Shell Program");                                    */
  /*  _exit(-1);                                                  */
  /* }                                                            */
  /* Problem 2: Handle redirection operators: < , or  >, or both  */ 
  int fd;
  int i;
  for (i=0;argv[i]!='\0';i++){
    if(strcmp(argv[i],"<")==0){
      fd=open(argv[i+1],O_RDONLY,0);
      if(i==0){
        fd=-1;
      }
      if (fd<0){
        perror("Input"); 
      }else{
        dup2(fd,0);
        close(fd);
        argv[i]=NULL;
        i++;
      }
    }
    if(strcmp(argv[i],">")==0){
      fd=creat(argv[i+1],0644);
      if(i==0){
        fd=-1;
      }
      if (fd<0){
        perror("Output"); 
      }else{
        dup2(fd,STDOUT_FILENO);
        close(fd);
        argv[i]=NULL;
      }
    }
  }
  if(execvp(*argv,argv)==-1){
    perror("Shell Program"); 
    _exit(-1);
  }
}

/* ----------------------------------------------------------------- */
/*                  parse input line into argc/argv format           */
/* ----------------------------------------------------------------- */
int parseline(char *cmdline, char **argv)
{
  int count = 0;
  char *separator = " \n\t";
  argv[count] = strtok(cmdline, separator);
  while ((argv[count] != NULL) && (count+1 < MAXARGS)) {
   argv[++count] = strtok((char *) 0, separator);
  }
  return count;
}
/* ----------------------------------------------------------------- */
/*                  The main program starts here                     */
/* ----------------------------------------------------------------- */
int main(void)
{
 char cmdline[MAXLINE];
 char *argv[MAXARGS];
 int argc;
 int status;
 pid_t pid;

 /* Loop forever to wait and process commands */
 while (1) {
  /* Step 1: Name your shell: csc60mshell - m for mini shell */ 
  printf("csc60mshell> ");
  fgets(cmdline, MAXLINE, stdin);
  /* Step 1: If user hits enter key without a command, continue to loop again at the beginning */
  /*         Hint: look up for they keyword "continue" in C */
  /* Step 1: Call parseline to build argc/argv: argc/argv parameters declared above */ 
  /* Step 1: Handle build-in command: exit, pwd, or cd - if detect one              */
  /* Step 1: Else, fork off a process */ 
  if (cmdline[0] =='\n'){
    continue;
  }
  argc=parseline(&*cmdline,&*argv);
  if(strcmp(*argv,"exit")==0){
    exit(EXIT_SUCCESS);
  }else if(strcmp(*argv,"cd")==0){
    if(argc==1){
      chdir("..");
    }else{
      chdir(argv[1]);
    }
    continue;
  }else if(strcmp(*argv,"pwd")==0){
    getcwd(cmdline, MAXLINE);
    printf("%s\n",cmdline);
    continue;
  }
  pid = fork();
  if (pid == -1) 
    perror("Shell Program fork error");
  else if (pid == 0) 
    /* I am child process. I will execute the command, call: execvp */
    process_input(argc, argv);
  else 
    /* I am parent process */
    if (wait(&status) == -1)
      perror("Shell Program error");
    else
      printf("Child returned status: %d\n",status);
 }
}
