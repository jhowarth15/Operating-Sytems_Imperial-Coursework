/******************************************************************
 * Header file for the helper functions. This file includes the
 * required header files, as well as the structures and the shared
 * memory/semaphore values (which are to be changed as needed).
 ******************************************************************/


# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/sem.h>
# include <sys/time.h>
# include <math.h>
# include <errno.h>
# include <string.h>

# define SHM_KEY 0 // Change this number as needed. changed from 0x50.
# define SHM_W 0200
# define SHM_R 0400
# define SHM_MODE (SHM_R | SHM_W)
# define SHM_SIZE 10000 // Change this number as needed
# define SEM_KEY 0x600 // Change this number as needed

union semun {
    int val;               /* used for SETVAL only */
    struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
    ushort *array;         /* used for GETALL and SETALL */
};

typedef struct jobtype
{
  int id;
  int duration;
} JOBTYPE;

typedef struct queue 
{
  int size; 
  //int front; 
  //int end; 
  JOBTYPE job[500]; // Can assume this to be maximum queue size
} QUEUE;

int check_arg (char *);
int sem_create (key_t, int);
int sem_attach (key_t);
int sem_init (int, int, int);
void sem_wait (int, short unsigned int);
//int sem_timewait (int, short unsigned int, int);
void sem_signal (int, short unsigned int);
int sem_close (int);

const int JOBCOUNT = 0;
const int EMPTIES = 1;
const int MUTEX = 2;

void displaySemVal(int semArrayID, int semID);

