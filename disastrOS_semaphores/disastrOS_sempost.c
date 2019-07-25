#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "disastrOS_globals.h"
#include "myConst.h"

void internal_semPost(){
	printf("/////////////////////// INVOCAZIONE DELLA SEMWAIT ///////////////////////// \n");
	int semnum=running->syscall_args[0];

	if(semaphores_list.size==0){
		printf("Non ci sono semafori\n");
		running->syscall_retvalue=TOOFEWSEM;
		return;
	}
	else if(semnum<0){
		printf("Numero del semaforo negativo\n");
		running->syscall_retvalue=SEMNUMINVALID;
		return;
	}
	else{
		Semaphore* s=SemaphoreList_byId(&(semaphores_list), semnum);
		if(s==NULL){
			printf("Il numero  del semaforo non c'è, è andato via\n");
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
				printf("Non trovo il semaforo da chiudere fra quelli che appartengono al processo\n");
				running->syscall_retvalue=SEMNUMINVALID;
				return;
			}

			//printf("/////////////////////// SONO AL 50% ///////////////////////// \n");
			int att_fd=tmpD->fd;
			SemDescriptorPtr* tmpP=(SemDescriptorPtr*)(s->descriptors.first);
			while(tmpP!=NULL){
				if(tmpP->descriptor->fd==att_fd) break;
				else tmpP=(SemDescriptorPtr*)tmpP->list.next;
			}
			if(tmpP==NULL){
				printf("Non trovo il semDescriptorPtr da chiudere fra quelli che appartengono al semaforo\n");
				running->syscall_retvalue=SEMNUMINVALID;
				return;
			}
			//printf("/////////////////////// SONO AL 80% ///////////////////////// \n");
			if(s->count==0 && s->waiting_descriptors.size>0){
				s->count++;
				PCB* att_proc=(PCB*)s->waiting_descriptors.first;
				while(att_proc!=NULL){
					//Esegui il processo
					//Rimuovi il processo
					//Cambio dello status
				}
			}
			else{

			}
			running->syscall_retvalue=0;
			return;
		}
	}
}
