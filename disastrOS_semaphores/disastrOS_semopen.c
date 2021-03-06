#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS_globals.h"
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "myConst.h"              //variabili ulteriori per avere la descrizione degli errori
#include "disastrOS_constants.h"  //Per avere accesso al numero massimo di processi

void internal_semOpen(){
	//I'm doing stuff :)
	/*Gestione degli errori:
	    1)Controllo sul numero massimo dei semafori
		2)Vedo se ci sono troppi descrittori di semafori per il processo corrente
		3)Controllo che il numero del semaforo sia appropriato
		Non occorre fare un controllo sul numero della system call
		4)Se non è già allocato provo ad allocare un nuovo semaforo e controllo che l'allocazione sia andata a buon fine
		Poi devo modificare i descrittori dei semafori del processo corrente
		5)Controllo sul descrittore
	*/
	if(semaphores_list.size>=MAX_NUM_SEMAPHORES){
		printf("Errore:Ci sono troppi semafori\n");
		running->syscall_retvalue=TOOMUCHSEM;
		return;
	}
	else if(running->sem_descriptors.size>=MAX_NUM_SEMDESCRIPTORS_PER_PROCESS){
		printf("Errore:Ci sono troppi semafori sul processo\n");
		running->syscall_retvalue=TOOMUCHSEMDES;
		return;
	}
	else{
		int semnum=running->syscall_args[0];
		int c=running->syscall_args[1];
		if(semnum<0){
			printf("Errore:Il numero del semaforo è negativo\n");
			running->syscall_retvalue = SEMNUMINVALID;
			return;
		}
		else{
			Semaphore* s=SemaphoreList_byId(&(semaphores_list), semnum);
			if(s==NULL){
				printf("Semaforo non trovato, lo devo allocare\n");
				s=Semaphore_alloc(semnum,c);
				if(s==NULL){
					printf("Errore di allocazione del semaforo\n");
					running->syscall_retvalue=SEMAPHOREALLOCFAILURE;
					return;
				}
				else{
					List_insert(&(semaphores_list),semaphores_list.last , (ListItem*)s);
					printf("Allocazione del semaforo eseguita con successo\n");
				}
			}
			//se arrivo qui vuol dire che ho creato un semaforo o che il semaforo cercato era già presente
			SemDescriptor* sdes=SemDescriptor_alloc(running->last_sem_fd, s, running);
			if(sdes==NULL){
				printf("Errore: Problemi con l'allocazione dei SemDescriptor\n");
				running->syscall_retvalue=DESERROR;
				return;
			}
			running->last_sem_fd++;
			SemDescriptorPtr* desdes= SemDescriptorPtr_alloc(sdes);
			if(desdes==NULL){
				printf("Errore: Problemi con l'allocazione dei SemDescriptorPtr\n");
				running->syscall_retvalue=DESERROR;
				return;
			}
			assert(List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*)sdes));

			sdes->ptr=desdes;
			List_insert(&s->descriptors, s->descriptors.last, (ListItem*)desdes);
			running->syscall_retvalue=semnum;
		}
	}
}
