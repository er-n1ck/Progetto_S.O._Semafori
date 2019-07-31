#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "myConst.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "disastrOS_constants.h"
#include "disastrOS_pcb.h"
#include "disastrOS_descriptor.h"
#include "operazioni.h"

void DeleteSem(Semaphore* s){
	Semaphore* res=(Semaphore*)List_detach(&semaphores_list, (ListItem*)s);
	if(res==NULL){
		printf("Errore: Rimozione del semaforo dalla lista globale dei semafori avvenuta in modo sbagliato\n");
		running->syscall_retvalue=DETACHERROR;
		return;
	}
	Semaphore_free(res);
}

void internal_semClose(){
	//I'm doing stuff :)

	printf("/////////////////////// INVOCAZIONE DELLA SEMCLOSE ///////////////////////// \n");
	int semnum=running->syscall_args[0];
	printf("/////////////////////// ricevuto semnum: %d ///////////////////////// \n",semnum);
	if(semaphores_list.size==0){
		printf("Errore: Non ci sono semafori\n");
		running->syscall_retvalue=TOOFEWSEM;
		return;
	}
	else if(semnum<0){
		printf("Errore: Numero del semaforo negativo\n");
		running->syscall_retvalue=SEMNUMINVALID;
		return;
	}
	else{
		Semaphore* s=SemaphoreList_byId(&(semaphores_list), semnum);
		if(s==NULL){
			printf("Errore: Il numero  del semaforo non c'è tra quelli aperti\n");
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
				printf("Errore: Non trovo il semaforo da chiudere fra qeulli che appartengono al processo\n");
				running->syscall_retvalue=SEMNUMINVALID;
				return;
			}
			//rimuovo il semaforo dai processi che lo hanno
			int att_fd=tmpD->fd;
			SemDescriptorPtr* tmpP=(SemDescriptorPtr*)(s->descriptors.first);
			while(tmpP!=NULL){
				if(tmpP->descriptor->fd==att_fd) break;
				else tmpP=(SemDescriptorPtr*)tmpP->list.next;
			}
			if(tmpP==NULL){
				printf("Errore: Non trovo il semDescriptorPtr da chiudere fra quelli che appartengono al semaforo\n");
				running->syscall_retvalue=SEMNUMINVALID;
				return;
			}
			SemDescriptor* ctrl1=(SemDescriptor*)List_detach(&running->sem_descriptors, (ListItem*)tmpD);
			SemDescriptorPtr* ctrl2=(SemDescriptorPtr*)List_detach(&s->descriptors, (ListItem*)tmpP);
			if(ctrl1==NULL || ctrl2==NULL){
				printf("Errore: Problemi con la detach\n");
				running->syscall_retvalue=DETACHERROR;
				return;
			}
			//finalmente possiamo effettuare le rimozioni
			SemDescriptor_free(tmpD);
			SemDescriptorPtr_free(tmpP);

			running->syscall_retvalue=0;

			if(s->descriptors.size==0) DeleteSem(s);
			printf("/////////////////////////////////////////////////////////Rimozione del semaforo effettuata correttamente\n");
			
		}
	}
}
