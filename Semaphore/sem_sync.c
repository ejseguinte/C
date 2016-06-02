
/*
 * Filename: sem_sync.c
 * Name: Erwin Seguinte
 * Date:December 06, 2015
 * This program simulates multiple programs access the same data using semaphores
 * History
 * Date		Initials		Action
 *=============================================
 * 12/06/2015	ES			Wrote Program/Compiled
 */

#include "curr_time.h"                      /* Declaration of currTime() */
#include <sys/stat.h>
#include <sys/types.h>
#include "tlpi_hdr.h"
#include <sys/sem.h>
#include "semun.h"
int
main(int argc, char *argv[])
{
    int j;          // Counter variable for the creation of children
    int semID;      // Holds the semaphore ID when it is first created
    struct sembuf sem_lock   = { 0, -(argc-1)}; // Buffer to lock the semaphore 
    struct sembuf sem_unlock = { 0,  1 }; // Buffer to unlock the semaphore
    union semun arg; // Used to initilitze the semaphore 
    
    printf("%s  Parent started\n", currTime("%T"));
    semID = semget(IPC_PRIVATE,1 ,0600|IPC_CREAT); //Creates the semaphore
    if(semID == -1){
      errExit("semID"); //Exits on error when creating the semaphore
    }
    printf("Semaphore ID = %d\n",semID); //Prints ID for semaphore
    arg.val = 0; //Changes the value in semun
    semctl(semID, 0, SETVAL, arg); //Initializes to zero 
    for (j = 1; j < argc; j++) {
        switch (fork()) {
        case -1:              //Error check if child is not created
            errExit("fork %d", j);

        case 0: /* Child */
            /* Child does some work, and lets parent know it's done */
            sleep(getInt(argv[j], GN_NONNEG, "sleep-time"));
                                            /* Simulate processing */
            printf("%s  Child %d (PID=%ld) unlocking\n",
                    currTime("%T"), j, (long) getpid());
            semop(semID,&sem_unlock,1); //Increaments the semaphore by one
            /* Child now carries on to do other things... */
            _exit(EXIT_SUCCESS);

        default: /* Parent loops to create next child */
            break;
        }
    }
    semop(semID,&sem_lock,1); //Locks the parent with -(argc-1)
    printf("%s  All obstacles removed, parent proceeds(PID=%ld)\n",currTime("%T"), (long) getpid());
    semctl(semID,0,IPC_RMID); //Removes the finished semaphore
    printf("sem id (%ld) successfully removed\n",semID);
    exit(EXIT_SUCCESS);
}   
