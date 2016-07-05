// Consumer

# include "helper.h"

int main (int argc, char *argv[])
{

  //Parameter error handling.
  if (argc!=2)
    {
      printf("ERROR -> Incorrect number of consumer arguments. Usage: ./consumer ID.\n");
      return -1;
    }

  int consID = check_arg(argv[1]);
  if (consID<1)
    {
      printf("ERROR -> Consumer(%s): ID must be integer not %s.\n", argv[1], argv[1]);
      return -1;
    }

  int time = 0;
  int countdown;

  //Attach to semaphore.
  const int SEM_ARRAY = sem_attach(SEM_KEY);
  if (SEM_ARRAY==-1)
    {
      printf("Error attaching to semaphore array.\n");
      return -1;
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

  int needToRepeat = 1;
  while(needToRepeat>0)
    {

      //Consume jobs while jobcount>0 
      while (semctl(SEM_ARRAY, JOBCOUNT, GETVAL)>0)
	{
	  //Find the first full slot in queue.
	  int fullSlot=-1;
	  int loop=0;
	  while (fullSlot==-1)
	    {
	      if (qPtr->job[loop].id != 0)
		{
		  fullSlot = loop;
		  break;
		}

	      loop++;

	      if (loop==QUEUE_SIZE)
		{
		  loop = 0;
		}
	    }

	  //Reduce jobcount semaphore by one.
	  sem_wait (SEM_ARRAY, JOBCOUNT);

	  //Block until MUTEX is equal to 1.
	  while (semctl(SEM_ARRAY, MUTEX, GETVAL)==0)
	    {
	      sleep(1);
	      time++;
	    }

	  //Decrement MUTEX.
	  sem_wait (SEM_ARRAY, MUTEX);

	  //Remove job from queue.
	  int consumedJobID = fullSlot+1;
	  qPtr->job[fullSlot].id = 0;
	  int consumedJobDuration = qPtr->job[fullSlot].duration;

	  //Output action to console.
	  printf("Consumer(%d) time %d: Job id %d executing sleep duration %d\n", consID, time, consumedJobID, consumedJobDuration);

	  //Increment MUTEX.
	  sem_signal (SEM_ARRAY, MUTEX);

	  //Increase empties semaphore by one.
	  sem_signal (SEM_ARRAY, EMPTIES);

	  //Make consumer wait and adjust time.
	  int consWait = consumedJobDuration;
	  time += consWait;
	  sleep(consWait);
	  printf("Consumer(%d) time %d: Job id %d completed.\n", consID, time, consumedJobID);

	}

      needToRepeat--;

      //Wait for 10sec if no jobs. Break out of wait and repeat above if job arrives in 10sec.
      countdown = 0;
      while (semctl(SEM_ARRAY, JOBCOUNT, GETVAL)==0 && countdown<=10)
	{
	  sleep(1);
	  countdown++;
	}

      if(countdown<=10)
	{
	  needToRepeat++;
	}

      time += countdown-1;
    }
  
  //displaySemVal(SEM_ARRAY, EMPTIES);


  //sem_wait(SEM_ARRAY, MUTEX);

  printf("Consumer(%d) time %d: No jobs left.\n", consID, time);

  sleep(500);

  //Destroy semaphore array.
  sem_close(SEM_ARRAY);
  printf("Semaphore deleted by consumer %d.\n", consID);

  //Detach program from shared memory.
  shmdt(qPtr);

  //Destroy shared memory.
  shmctl(shmid, IPC_RMID, NULL);
  printf("Shared memory deleted by consumer %d.\n", consID);

  return 0;
}
