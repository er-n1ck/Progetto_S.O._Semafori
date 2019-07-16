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
		Vedo se ci sono troppi descrittori di semafori per il processo corrente
		Controllo che il numero di parametri della systemcall sia appropriato
		Controllo il numero della system call
		Se non è già allocato provo ad allocare un nuovo semaforo e controllo che l'allocazione sia andata a buon fine
	*/

	if(running->sem_descriptors.size>MAX_NUM_SEMDESCRIPTORS_PER_PROCESS){
		disastrOS_debug("Calcola stai a sgravà con sti semafori, ne hai assegnati troppi al processo\n Allocazione fallita\n");
		running->return_value=TooMuchSemDes; //Imposto il valore di ritorno per il padre
		return;
	}
	else{
		int num_param=running->syscall_args[0];
		int num_syscall=running->syscall_num;
		if(num_param<0){
			disastrOS_debug("Il numero di argomenti della syscall è negativo, che è sta roba\n");
			running->return_value = SysCallArgsInvalid;
			return;
		}
		else if(num_syscall<0 || num_syscall>DSOS_MAX_SYSCALLS){
			disastrOS_debug("Il numero della syscall è negativo, che è sta roba\n");
			running->return_value = SysCallNumInvalid;
			return;
		}
	}
}
