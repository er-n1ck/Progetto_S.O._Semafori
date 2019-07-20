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

void internal_semClose(){
	//I'm doing stuff :)
	/*Cosa fare:
	 1)Verificare se il semaforo che si richiede di chiudere esiste
	 2)Verificare se il passaggio precedente avviene senza errori
	 Modificare i descrittori del semaforo corrente
	 3)Devo chiudere tutti i processi collegati con il semaforo(utilizzo i puntatori)
	 4)Rimuovere il descrittore del semaforo e controllo se va a buon fine
	 */
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

			//rimuovo il semaforo dai processi che lo hanno
			while(s->descriptors.first!=NULL){
				SemDescriptorPtr* ptr=(SemDescriptorPtr*)(s->descriptors.first);
				SemDescriptor* semDes=ptr->descriptor;
				PCB* pcb=semDes->pcb;
				int att_fd=semDes->fd;
				//devo liberare i file descriptor con fd uguale a quello del SemDescriptor da cui sono arrivato al processo attuale
				SemDescriptor* to_remove= SemDescriptorList_byFd(&pcb->descriptors, att_fd);
				if(to_remove==NULL){
					printf("Non ho trovato il fd, errore\n");
					running->syscall_retvalue=NOTFOUNDFD;
					return;
				}
				if(List_detach(&pcb->sem_descriptors, (ListItem*)to_remove)==NULL){
					printf("Problemi con la detach #1");
					running->syscall_retvalue=DETACHERROR;
					return;
				}
				//devo cancellare il sem_descriptor
				if(SemDescriptor_free(to_remove)!=0){
					printf("Errori nella SemDescriptor_free\n");
					running->syscall_retvalue=FREEERR;
					return;
				}
				//devo cancellare dai descriptorsPointers del semaforo quello che sto guardando
				if(List_detach(&s->descriptors, (ListItem*)ptr)==NULL){
					printf("Problemi con la detach #2\n");
					running->syscall_retvalue=DETACHERROR;
					return;
				}
				//devo cancellare il descriptorPtr
				if(SemDescriptorPtr_free(ptr)!=0){
					printf("Errori nella SemDescriptorPtr_free\n");
					running->syscall_retvalue=FREEERR;
					return;
				}
			}
			if(List_detach(&semaphores_list, (ListItem*)s)==NULL){
				printf("Problemi con la detach #3\n");
				running->syscall_retvalue=DETACHERROR;
				return;
			}
			if(Semaphore_free(s)!=0){
				printf("Problemi con la free del semaforo\n");
				running->syscall_retvalue=FREEERR;
				return;
			}
			running->syscall_retvalue=0;
			printf("Rimozione del semaforo effettuata correttamente\n");
		}
	}
}
