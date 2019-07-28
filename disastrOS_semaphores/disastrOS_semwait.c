#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "myConst.h"
#include "disastrOS_pcb.h"
#include "disastrOS_globals.h"
#include "myConst.h"

int contains(Semaphore* s,PCB* p){
	int ret=0;
	SemDescriptorPtr* ptr=(SemDescriptorPtr*)s->descriptors.first;
	while(ptr!=NULL){
		if(ptr->descriptor->pcb->pid==p->pid) return 1;
		else ptr=(SemDescriptorPtr*)ptr->list.next;
	}
	return ret;
}

void internal_semWait(){
	//I'm doing stuff :)
	/*Controlli:
	 1)Controllo se il semaforo è sul processo
	 2)Controllo se il processo è sul semaforo
	 3)Controllo se count è >0
	 VERO: vuol dire che il processo attuale può essere eseguito subito
	 	 Devo decrementare il count
	 	 Operazione da definire
	 FALSO:
	 	 Decremento
	 	 Aggiungo alla lista di waiting
	 	 Metto il running in stato di waiting
	*/
	printf("INVOCAZIONE DELLA SEMWAIT su proc:%d \n",disastrOS_getpid());
	int semnum=running->syscall_args[0];

	if(semaphores_list.size==0){
		printf("////////////////////////////////////////////////////////////////////////////Non ci sono semafori\n");
		running->syscall_retvalue=TOOFEWSEM;
		return;
	}
	else if(semnum<0){
		printf("////////////////////////////////////////////////////////////////////////////Numero del semaforo negativo\n");
		running->syscall_retvalue=SEMNUMINVALID;
		return;
	}
	else{
		Semaphore* s=SemaphoreList_byId(&(semaphores_list), semnum);
		if(s==NULL){
			printf("////////////////////////////////////////////////////////////////////////////Il numero  del semaforo non c'è, è andato via\n");
			running->syscall_retvalue=SEMNUMINVALID;
			return;
		}
		else{
			SemDescriptor* tmpD=(SemDescriptor*)running->sem_descriptors.first;
			while(tmpD!=NULL){
				if(tmpD->semaphore->id==s->id) break;
				else tmpD=(SemDescriptor*)tmpD->list.next;
			}
			if(tmpD==NULL){
				printf("////////////////////////////////////////////////////////////////////////////Non trovo il semaforo da chiudere fra quelli che appartengono al processo\n");
				running->syscall_retvalue=SEMNUMINVALID;
				return;
			}

			//printf("/////////////////////// SONO AL 50% ///////////////////////// \n");
			int att_pid=tmpD->pcb->pid;
			SemDescriptorPtr* tmpP=(SemDescriptorPtr*)(s->descriptors.first);
			while(tmpP!=NULL){
				if(tmpP->descriptor->pcb->pid==att_pid) break;
				else tmpP=(SemDescriptorPtr*)tmpP->list.next;
			}
			if(tmpP==NULL){
				printf("////////////////////////////////////////////////////////////////////////////Non trovo il semDescriptorPtr da chiudere fra quelli che appartengono al semaforo\n");
				running->syscall_retvalue=SEMNUMINVALID;
				return;
			}



			if(contains(s,tmpD->pcb)==1){
				//Il processo non è già nella lista dei waiting
				//printf("YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY Il processo NON è già nella lista di waiting del semaforo\n");

				s->count--;
				SemDescriptorPtr* newElem=SemDescriptorPtr_alloc(tmpD);
				assert(List_insert(&s->waiting_descriptors,(ListItem*)s->waiting_descriptors.last,(ListItem*)newElem)!=NULL);
				PCB* res=(PCB*)List_detach(&ready_list,(ListItem*)running);
				assert(List_insert(&waiting_list,(ListItem*)waiting_list.last,(ListItem*)res)!=NULL);
				running->status=Waiting;
			}
			else{
				//Il processo è già in waiting, che devo fare??
				printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Il processo è già nella lista di waiting del semaforo\n");
			}
			running->syscall_retvalue=semnum;
			return;
		}

	}
}
