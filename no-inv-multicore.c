#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define LOW 10
#define MID 5
#define HIGH 5


int policy = SCHED_FIFO;
//mutex variables
pthread_mutex_t shared_mutex;
pthread_mutexattr_t mutex_prioinherit;

//function to set cpu affinity for threads
int stick_thread_to_core(int core_id) {
   int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
   if (core_id < 0 || core_id >= num_cores)
      return EINVAL;
   cpu_set_t cpuset;
   CPU_ZERO(&cpuset);
   CPU_SET(core_id, &cpuset);

   pthread_t current_thread = pthread_self();    
   return pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
}

int gettime ()
{
  return time(NULL); //returns the current time
}

//looping for n seconds    
void doSomething (int n, char *str,int pri)
{
   int now = gettime();
   int counter=0;
   while ( ( gettime() - now) < n)
         { 
             printf("%s %d on cpu: %d\n",str,pri,sched_getcpu());
            counter++;
         }
}

void *low()
{    
    struct sched_param the_priority;
    struct sched_param get_prio;
    the_priority.sched_priority  = sched_get_priority_min(SCHED_FIFO);
    if(pthread_setschedparam(pthread_self(), policy, &the_priority) < 0){
        perror(" ");
    }
    if(pthread_getschedparam(pthread_self(),&policy,&get_prio) < 0){
        perror(" ");
    }
    stick_thread_to_core(0);
    if(pthread_mutex_lock(&shared_mutex)){
        perror(" ");
    }
    doSomething(LOW,"L is running with priority",get_prio.sched_priority);
    if(pthread_mutex_unlock(&shared_mutex)){
        perror(" ");
    }
    return NULL;
    
}
void *high()
{   
    sleep(2);   //executes after two seconds
    struct sched_param the_priority;
    struct sched_param get_prio;
    the_priority.sched_priority  = sched_get_priority_max(SCHED_FIFO);
    if(pthread_setschedparam(pthread_self(), policy, &the_priority) < 0){
        perror(" ");
    }
    if(pthread_getschedparam(pthread_self(),&policy,&get_prio) < 0){
        perror(" ");
    }
    stick_thread_to_core(1);
    if(pthread_mutex_lock(&shared_mutex)){
        perror(" ");
    }
    doSomething(HIGH,"H is running with priority",get_prio.sched_priority);
    if(pthread_mutex_unlock(&shared_mutex)){
        perror(" ");
    }
    return NULL;       
}
void *mid()
{   
    sleep(3);  //executes after 3 seconds
    struct sched_param the_priority;
    struct sched_param get_prio;
    the_priority.sched_priority  = (sched_get_priority_max(SCHED_FIFO)-10);
    if(pthread_setschedparam(pthread_self(), policy, &the_priority) < 0){
        perror(" ");
    }
    if(pthread_getschedparam(pthread_self(),&policy,&get_prio) < 0){
        perror(" ");
    }
    
    stick_thread_to_core(2);
    doSomething(MID,"M is running with priority",get_prio.sched_priority);
    return NULL;
    
  }

int main(int argc,char *argv[]){
    
    
    //initialising mutex attribute
    if (pthread_mutexattr_init(&mutex_prioinherit)) 
	{   perror("mutex init");
	    exit(1);
	}
    
    
    //applying attribute to mutex
    if (pthread_mutex_init(&shared_mutex, &mutex_prioinherit))
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
    
    
    if(pthread_mutexattr_destroy (&mutex_prioinherit)){
        perror(" ");
    }
    if(pthread_mutex_destroy(&shared_mutex)){
        perror(" ");
    }
}
