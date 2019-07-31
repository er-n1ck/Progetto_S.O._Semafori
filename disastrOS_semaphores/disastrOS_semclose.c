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

void staccaCiStannoTracciando(Semaphore* s){
	Semaphore* res=(Semaphore*)List_detach(&semaphores_list, (ListItem*)s);
	if(res==NULL){
		printf("C'è stato un'errore nella rimozione del semaforo dalla lista globale dei semafori\n");
		running->syscall_retvalue=DETACHERROR;
		return;
	}
	Semaphore_free(res);
}

void internal_semClose(){
	//I'm doing stuff :)
	/*Cosa fare:
	 1)Verificare se il semaforo che si richiede di chiudere esiste
	 2)Verificare se il passaggio precedente avviene senza errori
	 Modificare i descrittori del semaforo corrente
	 3)Devo chiudere tutti i processi collegati con il semaforo(utilizzo i puntatori)
	 4)Rimuovere il descrittore del semaforo e controllo se va a buon fine
	 */
	printf("/////////////////////// INVOCAZIONE DELLA SEMCLOSE ///////////////////////// \n");
	int semnum=running->syscall_args[0];
	printf("/////////////////////// ricevuto semnum: %d ///////////////////////// \n",semnum);
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
				printf("Non trovo il semaforo da chiudere fra qeulli che appartengono al processo\n");
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
				printf("Non trovo il semDescriptorPtr da chiudere fra quelli che appartengono al semaforo\n");
				running->syscall_retvalue=SEMNUMINVALID;
				return;
			}
			SemDescriptor* ctrl1=(SemDescriptor*)List_detach(&running->sem_descriptors, (ListItem*)tmpD);
			SemDescriptorPtr* ctrl2=(SemDescriptorPtr*)List_detach(&s->descriptors, (ListItem*)tmpP);
			if(ctrl1==NULL || ctrl2==NULL){
				printf("Problemi con la detach\n");
				running->syscall_retvalue=DETACHERROR;
				return;
			}
			//finalmente possiamo effettuare le rimozioni
			SemDescriptor_free(tmpD);
			SemDescriptorPtr_free(tmpP);

			if(s->descriptors.size==0) staccaCiStannoTracciando(s);
			printf("/////////////////////////////////////////////////////////Rimozione del semaforo effettuata correttamente\n");
			running->syscall_retvalue=0;
		}
	}
}
