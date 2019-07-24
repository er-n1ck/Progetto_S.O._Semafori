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
			if(s->count>0){
				s->count--;
			}
			else{
				SemDescriptor* check=(SemDescriptor*)List_insert(&s->waiting_descriptors, (ListItem*)s->waiting_descriptors.last,(ListItem*)tmpP);
				if(check==NULL){
					printf("Inserimento del semaforo avvenuto in modo sbagliato\n");
					running->syscall_retvalue=-1;
					return;

				}
				running->status=Waiting;
				PCB* p=(PCB*)List_detach(&ready_list, (ListItem*)running);
				PCB* pp=(PCB*)List_insert(&waiting_list,(ListItem*)waiting_list.last, (ListItem*)running);

				running=(PCB*)List_detach(&waiting_list, waiting_list.first);

				if(p==NULL || pp==NULL){
					printf("Errore con la detach o con la insert\n");
					running->syscall_retvalue=DETACHERROR;
					return;
				}

				if(running==NULL){
					printf("Ho avuto problemi quando hai preso il primo processo in waiting");
					running->syscall_retvalue=DETACHERROR;
					return;
				}
			}
			running->syscall_retvalue=0;
			return;
		}
	}
}
