#include <sched.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define LOW 8
#define MID 3

pthread_mutex_t shared_mutex;
pthread_mutexattr_t mutex_attr;

int gettime ()
{
  return time(NULL);
}

//looping for n seconds    
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
{   
    struct sched_param the_priority;
    the_priority.sched_priority  = 1;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &the_priority);
    pthread_mutex_lock(&shared_mutex);
    printf("L is taking resources\n");
    doSomething(LOW);
    pthread_mutex_unlock(&shared_mutex);
    printf("L is releasing resources\n");
    return NULL;
    
}
void *high()
{   
    sleep(2);   //executes after two seconds
    struct sched_param the_priority;
    the_priority.sched_priority  = 50;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &the_priority);
    printf("H came\n");
    printf("H is waiting for resources\n");
    pthread_mutex_lock(&shared_mutex);
    printf("H gets resource now\n");
    pthread_mutex_unlock(&shared_mutex);
    printf("H finishes\n");
    return NULL;       
}
void *mid()
{   
    sleep(3);  //executes after 3 seconds 
    struct sched_param the_priority;
    the_priority.sched_priority  = 25;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &the_priority);
    printf("M came preempting L\n");
    doSomething(MID);
    printf("Releasing L\n");
    return NULL;
    
  }

int main(){
    if (pthread_mutexattr_init(&mutex_attr)) 
	{   perror("mutex init");
	    exit(1);
	}
	
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
