#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#define MAXLINE 80
#define MAXARGS 20
  
 struct job_array{
  int process_id;     //Process Id
  char command[80];   //Previous command with '&' removed
  int job_number;     //Job number
  struct job_array* next;     //Next Job
 };
 struct job_array *root; //Structure initialization
 struct job_array *curr; //Structure initialization
 int counter;
 int max;
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
/*                 Prints the information in the Job Array           */
/* ----------------------------------------------------------------- */
void printJobs(struct job_array* curr,int counter){
  if (counter == 0 ){
    printf("No jobs are currently running in the background\n");
  }else {
    for(int i=0;i<counter;i++){
    printf("[%03d] (PID: %7d) Running     %s\n",i+1,curr->process_id ,curr->command);
    curr=curr->next;
    }
  }
}
/* ----------------------------------------------------------------- */
/*                Deletes the information in the Job Array           */
/* ----------------------------------------------------------------- */
void delete(struct job_array* temp,struct job_array* curr,int counter){ //struct job_array* root,struct job_array* curr,
  if(temp == root && counter ==1){ //case of one item in the list
    printf("\n[%03d] (PID: %7d) Done        %c\n",temp->job_number,temp->process_id,temp->command[80]);
    root=NULL;
  }else if(temp == root&&curr==root){          // case of root replacement
    printf("\n[%03d] (PID: %7d) Done        %c\n",temp->job_number,temp->process_id,temp->command[80]);
    *temp=*temp->next;
  } else{                          //Normal case 
    printf("\n[%03d] (PID: %7d) Done        %c\n",curr->job_number,curr->process_id ,curr->command[80]);
    if(counter<max ){
      *temp->next=*temp->next->next;
    }else{
     temp->next=malloc( sizeof(struct job_array) );
     temp->next=NULL;
    }
  }
 max--;
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
/*                  Handles killing child processes                  */
/* ----------------------------------------------------------------- */
static void sigHandler(int sig){
  kill(0,SIGINT); //kills all of the process in the process group
}
/* ----------------------------------------------------------------- */
/*                  Handles killing child processes                  */
/* ----------------------------------------------------------------- */
static void parentHandler(int sig){
  switch(sig){
    case SIGCHLD:{
     struct job_array* temp; //Structure initialization
     curr=root;
     temp = curr;
     //for (int i=0; i<counter; i++){
     int pid = waitpid((pid_t)(-1), 0, WNOHANG);
  
     while(pid > 0 ){
       int i = 0;
       curr = root;
       
       while(curr->next != NULL && curr->process_id != pid){
         temp = curr;
         curr = curr->next;
         i++;
       }if(curr->process_id == pid){
        delete(temp,curr,counter); //root,curr,
        counter--;
       }else{
         printf("Process ID does not exist");
       }
       pid = waitpid((pid_t)(-1), 0, WNOHANG);
     }
       //if(curr->next != NULL){
        //temp=curr;
        //curr=curr->next;
       //}
      //}   
      break;
    }
  }
}
/* ----------------------------------------------------------------- */
/*                  The main program starts here                     */
/* ----------------------------------------------------------------- */
int main(void)
{
 /* Create a linked list for jobs Array */
 
 
 root = NULL;  
 curr=root;
 
 /* Initialize the Variables */
 char cmdline[MAXLINE];
 char *argv[MAXARGS];
 int argc;     
 int status;
 char* home; 
 pid_t pid;
 counter = 0;     //Keeps track of the amount of background proccesses
 max = 0;         //Keeps track of the amount of background proccesses   
 bool background;
 background = false;

 /* Sets up signal ignore */
 struct sigaction handler, reset;
 handler.sa_handler = parentHandler;
 reset.sa_handler = sigHandler;
 handler.sa_flags = SA_NOCLDSTOP | SA_RESTART;
 sigemptyset(&handler.sa_mask);

 /* Loop forever to wait and process commands */
 while (1) { // for(int i=0;i<10;i++){ 
   sigaction(SIGINT, &handler, NULL); 
  /*Check to see if any children are complete*/
   sigaction(SIGCHLD, &handler, NULL); 
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
    if(counter == 0)
      exit(EXIT_SUCCESS);
    else
      printf("There are still background processes running please wait for them to finish or kill them before exiting\n");
      continue;
  }else if(strcmp(*argv,"cd")==0){
     home = getenv("HOME");
        if (argc == 2)
        {
            strcpy(home,argv[1]);
            //printf("%s\n",home);
        }
        chdir(home);
    continue; 
  }else if(strcmp(*argv,"pwd")==0){
    getcwd(cmdline, MAXLINE);
    printf("%s\n",cmdline);
    continue;
  }else if(strcmp(*argv,"jobs")==0){
    printJobs(root,counter );
    continue;
  }else if (strcmp(argv[argc-1],"&")==0){
    background = true;
    argv[argc - 1]=NULL;
  }
  pid = fork();
  if (pid == -1) 
    perror("Shell Program fork error");
  else if (pid == 0) {
    /*
     * Restore signals to their original dispositions,
     * and restore the signal mask.
     */
     sigaction(SIGINT, &reset, NULL);
    /* I am child process. I will execute the command, call: execvp */
    if (background==true)
      setpgid(0,0);
    process_input(argc, argv);
  }else {
    /* I am parent process */
    if(background == true){
    /* Loads the new background command into the linked list */
      curr=root;
      while(counter>0 && curr->next != NULL){
        curr=curr->next;
      }
      if (root == NULL){
        root = malloc( sizeof(struct job_array) );   //Creates new sapce for the linked list
        curr=root;
      }else {
        curr->next=malloc( sizeof(struct job_array) ); //makes curr->next redirect to new space for the linked list
        curr = curr->next;
      }
      curr->next = NULL;
      curr->process_id=pid;
      int i = 1;
      strcpy(curr->command,*argv);
      /* Combines all the command line arguments to create a string*/
      while (argv[i]!=NULL){
        strcat(curr->command," ");
        strcat(curr->command,argv[i++]);
      }
      counter = counter + 1;
      curr->job_number=counter;
      background = false;
      max++;
      printJobs(root,counter);
    }else{
      int id = waitpid(pid, &status, 0);
      if (id == -1)
        perror("Shell Program error");
      else if (WIFSIGNALED(status)) //intercepts the signal sent from kill to tell the user the process was killed
        printf("\nChild exited via signal %d\n",WTERMSIG(status));
      else
        printf("Child returned status: %d\n",status);
    }
   }
 }
}
