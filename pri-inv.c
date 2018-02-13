#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
pthread_mutex_t shared_mutex;
const int seconds = 1;
pthread_mutexattr_t mutex_attr;

#define LOW 2
#define MID 5

int gettime ()
{
  return time(NULL);
}

    
void doSomething (int n)
{
   int now = gettime();
   int counter=0;
   while ( ( gettime() - now) < n)
         { 
            counter++;
         }
}

void *low()
{   int now = gettime();
    struct sched_param the_priority;

    the_priority.sched_priority  = 1;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &the_priority);
    pthread_mutex_lock(&shared_mutex);
    doSomething(LOW*seconds);
    pthread_mutex_unlock(&shared_mutex);
    printf ("low took %d seconds wanted about %d (critical section + mid time)\n",gettime() - now,LOW+MID);
return NULL;
    
}
void *high()
{   int now ;
    struct sched_param the_priority;

    the_priority.sched_priority  = 50;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &the_priority);
    now=gettime();
    pthread_mutex_lock(&shared_mutex);
    pthread_mutex_unlock(&shared_mutex);
    printf ("high took %d seconds wanted about %d (low critical section)\n",gettime() - now,LOW);
return NULL;
        
}
void *mid()
{   struct sched_param the_priority;
    int now;

    the_priority.sched_priority  = 25;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &the_priority);
    now = gettime();
    doSomething(MID*seconds);
    printf ("mid took %d seconds wanted about %d\n",gettime() - now,MID);
return NULL;
    
  }

int main(){
    if (pthread_mutexattr_init(&mutex_attr)) 
	{   perror("mutex init");
	    exit(1);
	}
//	#if defined(_POSIX_THREAD_PRIO_INHERIT) && _POSIX_THREAD_PRIO_INHERIT != -1 && defined(USE_PRI_INHERIT)
/*        printf("Using priority inheritance\n");
        if (pthread_mutexattr_setprotocol(&mutex_attr, PTHREAD_PRIO_INHERIT))
	    {   perror("mutex init");
	        exit(1);
	    }
    #else
        printf("Not Using priority inheritance\n");

    #endif
*/
if (pthread_mutex_init(&shared_mutex, &mutex_attr))
	{   perror("mutex init");
	    exit(1);
	}
    pthread_t t1,t2,t3;
    pthread_create(&t1, NULL, low, NULL);
    pthread_create(&t2, NULL, mid, NULL);
    pthread_create(&t3, NULL, high, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
}
