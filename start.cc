/******************************************************************
 * Program for setting up semaphores/shared memory before running
 * the Producer and Consumer 
 ******************************************************************/

# include "helper.h"

int main (int argc, char **argv)
{

  if (argc!=2)
    {
      printf("ERROR -> Incorrect number of start arguments. Usage: ./start QueueSize.\n");
      return -1;
    }

  //Check and take queue size parameter in command line.
  const int QUEUE_SIZE = check_arg(argv[1]);  

  if (QUEUE_SIZE<1)
    {
      printf("ERROR -> Start queue size must be positive integer.\n");
      return -1;
    }

  //Create semaphore array.
  int SEM_ARRAY = sem_create(SEM_KEY, 3);
  if (SEM_ARRAY==-1)
    {
      //Semaphore left over previously. Destroy it.
      SEM_ARRAY = sem_attach(SEM_KEY);
      sem_close(SEM_ARRAY);

      //Create new.
      SEM_ARRAY = sem_create(SEM_KEY, 3);
      printf("Overwrote previous semaphore array.\n");

    }

  //printf("Semaphore array id: %d.\n", SEM_ARRAY);
	     
  //Initialise semaphores for jobcount, empty spaces and mutex.
  sem_init(SEM_ARRAY, JOBCOUNT, 0);
  sem_init(SEM_ARRAY, EMPTIES, QUEUE_SIZE);
  sem_init(SEM_ARRAY, MUTEX, 1);

  //Create queue in shared memory.
  key_t key = 123456789; //arbitrary key
  int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
  queue* qPtr;

  qPtr = (queue*)shmat(shmid, (void*)SHM_KEY, 0);
  if (qPtr== (queue *)(-1))
    {
      printf("shmat error.\n");
      exit(1);
    }

  qPtr->size = QUEUE_SIZE;

  //printf("Queue size: %d\nqueue front: %d\nqueue end: %d\n", qPtr->size, qPtr->front, qPtr->end);

  for (int x=0; x<QUEUE_SIZE; x++)
    {
      qPtr->job[x].id = 0;
      //printf("Job %d id is %d\n", x+1, qPtr->job[x].id);
    }

  //Detach program from shared memory.
  shmdt(qPtr);

  return 0;
}
