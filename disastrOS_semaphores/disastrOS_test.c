#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "disastrOS.h"
#include "myConst.h"
#include "disastrOS_globals.h"

void producer(int sem_c,int sem_p){
  disastrOS_semWait(sem_p);
  printf("----------------------------------------------------------------------------------------------Proc: %d ha prodotto\n",disastrOS_getpid());
  disastrOS_semPost(sem_c);
}

void consumer(int sem_c,int sem_p){
  disastrOS_semWait(sem_c);
  printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++Proc: %d ha consumato\n",disastrOS_getpid());
  disastrOS_semPost(sem_p);
}

// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");
  int type=0;
  int mode=0;
  int fd=disastrOS_openResource(disastrOS_getpid(),type,mode);
  printf("fd=%d\n", fd);
  printf("PID: %d, terminating\n", disastrOS_getpid());

  for (int i=0; i<(disastrOS_getpid()+1); ++i){
    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep((20-disastrOS_getpid())*5);
  }
  disastrOS_exit(disastrOS_getpid()+1);
}

void childFunction_2_laVendetta(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");

  int fd_P=disastrOS_semOpen(SEM_P,GREENLIGHT);
  int fd_C=disastrOS_semOpen(SEM_C,REDLIGHT);
  if(fd_P<0 || fd_C<0) return;
  
  printf("PID: %d, terminating\n", disastrOS_getpid());

  if(disastrOS_getpid()==2){
    for (int i=0; i<HowManyTimes; ++i){
      printf("PID: %d, iterate as CONSUMER %d times\n", disastrOS_getpid(), i);
      consumer(SEM_C,SEM_P);
      disastrOS_sleep((10-disastrOS_getpid())*5);
    }
  }

  if(disastrOS_getpid()==3){
    for (int j=0; j<HowManyTimes; ++j){
      printf("PID: %d, iterate as PRODUCER %d times\n", disastrOS_getpid(), j);
      producer(SEM_C,SEM_P);
      disastrOS_sleep((10-disastrOS_getpid())*5);
    }
  }
  
  //printf("Chiusura del semaforo con id: %d e fd: %d\nsul processo con pid:%d\n",semnum,fd,disastrOS_getpid());
  
  int retval_p=disastrOS_semClose(SEM_P);
  int retval_c=disastrOS_semClose(SEM_C);
  if(retval_p || retval_c) return;

  disastrOS_exit(disastrOS_getpid()+1);
}

void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);

  printf("I feel like to spawn 2 nice threads\n");
  int alive_children=0;
  for (int i=0; i<2; ++i) {
    int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource (and creating if necessary)\n");
    int fd=disastrOS_openResource(i,type,mode);
    printf("fd=%d\n", fd);
    disastrOS_spawn(childFunction_2_laVendetta, 0);
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    --alive_children;
  }
  printf("shutdown!");
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction_2_laVendetta);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}