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
		printf("Non ci sono semafori, non puoi rimuovere un semaforo inesistente\n");
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
			while(s->descriptors.first!=NULL){
				SemDescriptorPtr* ptr=(SemDescriptorPtr*)(s->descriptors.first);
				SemDescriptor* semDes=ptr->descriptor;
				PCB* pcb=semDes->pcb;
				int att_fd=semDes->fd;
				//devo liberare i file descriptor con fd uguale a quello del SemDescriptor da cui sono arrivato al processo attuale
				SemDescriptor* to_remove= SemDescriptorList_byFd(&pcb->descriptors, att_fd);
				List_detach(&pcb->descriptors, (ListItem*)to_remove);
				//devo cancellare dai sem_descriptors del processo il semaforo considerato
				List_detach(&pcb->sem_descriptors, (ListItem*)s);
				//devo cancellare il sem_descriptor
				SemDescriptor_free(to_remove);
				//devo cancellare dai descriptorsPointers del semaforo quello che sto guardando
				List_detach(&s->descriptors, (ListItem*)ptr);
				//devo cancellare il descriptorPtr
				SemDescriptorPtr_free(ptr);
			}
			while(s->waiting_descriptors.first!=NULL){
							SemDescriptorPtr* ptr=(SemDescriptorPtr*)(s->descriptors.first);
							SemDescriptor* semDes=ptr->descriptor;
							PCB* pcb=semDes->pcb;
							int att_fd=semDes->fd;
							//devo liberare i file descriptor con fd uguale a quello del SemDescriptor da cui sono arrivato al processo attuale
							SemDescriptor* to_remove= SemDescriptorList_byFd(&pcb->descriptors, att_fd);
							List_detach(&pcb->descriptors, (ListItem*)to_remove);
							//devo cancellare dai sem_descriptors del processo il semaforo considerato
							List_detach(&pcb->sem_descriptors, (ListItem*)s);
							//devo cancellare il sem_descriptor
							SemDescriptor_free(to_remove);
							//devo cancellare dai descriptorsPointers del semaforo quello che sto guardando
							List_detach(&s->descriptors, (ListItem*)ptr);
							//devo cancellare il descriptorPtr
							SemDescriptorPtr_free(ptr);
						}
			SemDescriptor* des=SemDescriptorList_byFd(&(running->sem_descriptors), semnum);
			if(des==NULL){
				printf("Il semaforo che hai scelto non è presente fra quelli del processo\n");
				running->syscall_retvalue=SEMNUMINVALID;
				return;
			}
			else{
				SemDescriptor* removed= (SemDescriptor*)List_detach(&(running->descriptors), (ListItem*)des);
				if(remove==NULL){
					printf("Ci sono stati problemi con la rimozione, tocca chiamare il carro attrezzi\n");
					running->syscall_retvalue=REMOVEERROR;
					return;
				}
				SemDescriptorPtr* sdesptr=(SemDescriptorPtr*)List_detach(&s->descriptors, (ListItem*)(removed->ptr));;
				assert(sdesptr);
				SemDescriptor_free(des);
				SemDescriptorPtr_free(sdesptr);
				running->syscall_retvalue=0;
				printf("Tutto andato correttamente\n");
			}
		}
	}
}
