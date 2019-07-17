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

void internal_semClose(){
	//I'm doing stuff :)
	/*Cosa fare:
	 1)Verificare se il semaforo che si richiede di chiudere esiste
	 2)Verificare se il passaggio precedente avviene senza errori
	 Modificare i descrittori del semaforo corrente
	 3)Devo chiudere tutti i processi collegati con il semaforo(utilizzo i puntatori)
	 4)Rimuovere il descrittore del semaforo e controllare se va a buon fine
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
			SemDescriptor* sdes=SemDescriptorList_byFd(&(running->sem_descriptors), semnum);
			if(sdes==NULL){
				printf("Il semaforo che hai scelto non è presente fra quelli del processo\n");
				running->syscall_retvalue=SEMNUMINVALID;
				return;
			}
			else{
				SemDescriptor* removed= List_detach(&(running->descriptors), (ListItem*)sdes);
				if(removed==NULL){
					printf("Ci sono stati problemi con la rimozione, tocca chiamare il carro attrezzi\n");
					running->syscall_retvalue=REMOVEERROR;
					return;
				}
				running->last_sem_fd--;
				disastrOS_debug("Tutto andato correttamente\n");
			}
		}
	}
}
