// Producer

#include "helper.h"

int main (int argc, char *argv[])
{

  //Parameter error handling.
  if (argc!=3)
    {
      printf("ERROR -> Incorrect number of producer arguments. Usage: ./producer ID NoJobs.\n");
      return -1;
    }

  int prodID = check_arg(argv[1]);
  if (prodID<1)
    {
      printf("ERROR -> Producer(%s): ID must be positive integer not %s.\n", argv[1], argv[1]);
      return -1;
    }
  int jobsToCreate = check_arg(argv[2]);
  if (jobsToCreate==-1)
    {
      printf("ERROR -> Producer(%d): Amount of jobs must be positive integer not %s.\n", prodID, argv[2]);
      return -1;
    }

  //Start time clock.
  int time = 0;

  //printf("Producer ID is %d and jobs to create is %d.\n", prodID, jobsToCreate);

  //Attach to semaphore.
  const int SEM_ARRAY = sem_attach(SEM_KEY);
  if (SEM_ARRAY==-1)
    {
      printf("Error attaching to semaphore array.\n");
      exit(1);
    }

  //Attach to shared memory.
  key_t key = 123456789; //arbitrary key
  int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
  queue* qPtr;

  qPtr = (queue*)shmat(shmid, (void*)SHM_KEY, 0);
  if (qPtr== (queue *)(-1))
    {
      printf("Shared memory connection error.\n");
      exit(1);
    }

  const int QUEUE_SIZE = qPtr->size;
  //printf("queue size is %d\n", QUEUE_SIZE);

  while (jobsToCreate > 0)
    {

      //Block if no empty spaces in queue until one becomes available.
      while (semctl(SEM_ARRAY, EMPTIES, GETVAL)==0)
	{
	  sleep(1);
	  time++;
	}

      //Find the first empty slot in the queue.
      //Cycle through job[] until hit a zero and then replace with position to activate job.
      int emptySlot=-1;
      int loop=0;
      while (emptySlot==-1)
	{
	  if (qPtr->job[loop].id == 0)
	    {
	      emptySlot = loop;
	      break;
	    }

	  loop++;

	  if (loop==QUEUE_SIZE)
	    {
	      loop = 0;
	    }
	}
      
      //Reduce empties semaphore by one.
      sem_wait (SEM_ARRAY, EMPTIES);

      //Block until MUTEX is equal to 1.
      while (semctl(SEM_ARRAY, MUTEX, GETVAL)==0)
	{
	  sleep(1);
	  time++;
	}

      //Decrement MUTEX.
      sem_wait (SEM_ARRAY, MUTEX);

      //Insert job into queue.
      int newJobID = emptySlot+1;
      qPtr->job[emptySlot].id = newJobID;
     
      //Random time between 2 and 7 seconds.
      srand (time+prodID);
      int r1 = rand() % 6;
      int newJobDuration = r1 + 2;

      qPtr->job[emptySlot].duration = newJobDuration;

      //Increment MUTEX.
      sem_signal (SEM_ARRAY, MUTEX);

      //Increase jobcount semaphore by one.
      sem_signal (SEM_ARRAY, JOBCOUNT);

      //Output action to console.
      printf("Producer(%d) time %d: Job id %d duration %d\n", prodID, time, newJobID, newJobDuration);
      --jobsToCreate;

      //Make producer wait between 2 and 4 seconds, and adjust time.
      int r2 = rand() % 3;
      int prodWait = r2 + 2;
      time += prodWait;
      sleep(prodWait);

      //displaySemVal(SEM_ARRAY, EMPTIES);
      //displaySemVal(SEM_ARRAY, JOBCOUNT);

    }

  
  //displaySemVal(SEM_ARRAY, EMPTIES);

  if (jobsToCreate == 0)
    {
      printf("Producer(%d) time %d: No more jobs to generate.\n", prodID, time);
    }
  else
    {
      printf("Still jobs to create but unknown error.\n");
    }

  //Detach program from shared memory.
  shmdt(qPtr);

  sleep(500);


  return 0;
}
