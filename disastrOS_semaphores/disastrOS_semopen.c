#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "myConst.h"
#include "disastrOS_constants.h"  //Per avere accesso al numero massimo di processi
#include "disastrOS_pcb.h"        //Per avere l'accesso alle variabili interne al processo

void internal_semOpen(){
	printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
	//I'm doing stuff :)
	/*Gestione degli errori:
	    1)Controllo sul numero massimo dei semafori
		2)Vedo se ci sono troppi descrittori di semafori per il processo corrente
		3)Controllo che il numero del semaforo sia appropriato
		Non occorre fare un controllo sul numero della system call
		4)Se non è già allocato provo ad allocare un nuovo semaforo che è binario e controllo che l'allocazione sia andata a buon fine
		5)altrimenti
	*/
	//Todo: Controllo sui semafori
	if(semaphores_list.size>=MAX_NUM_SEMAPHORES){
		disastrOS_debug("Too many semafori e gli ausiliari del traffico muti!!!!\n");
		running->return_value=TOOMUCHSEM;
		return;
	}
	else if(running->sem_descriptors.size>=MAX_NUM_SEMDESCRIPTORS_PER_PROCESS){
		disastrOS_debug("Calcola stai a sgravà con sti semafori, ne hai assegnati troppi al processo\n Allocazione fallita\n");
		running->return_value=TOOMUCHSEMDES; //Imposto il valore di ritorno per il padre
		return;
	}
	else{
		int semnum=running->syscall_args[0];
		if(semnum<0){
			disastrOS_debug("Il numero del semaforo è negativo, che è sta roba\n");
			running->return_value = SEMNUMINVALID;
			return;
		}
		else{
			if(SemaphoreList_byId(semaphores_list, semnum)){
				disastrOS_debug("Semaforo non trovato, lo devo allocare");
				Semaphore* s=Semaphore_alloc(semnum,GREENLIGHT);
				if(s==NULL){
					disastrOS_debug("Errore di allocazione del semaforo, dannati trattori\n");
					running->return_value=SEMAPHOREALLOCFAILURE;
					return;
				}
				else{
					SemDescriptor* sdes=SemDescriptor_alloc(semnum, s, running);
					List_insert(&(semaphores_list), running->sem_descriptors.last,(ListItem*) sdes);
					disastrOS_debug("Allocazione del semaforo eseguita con successo\n");
				}
			}
		}
	}
}
