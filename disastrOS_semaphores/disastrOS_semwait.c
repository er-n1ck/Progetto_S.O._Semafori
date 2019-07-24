#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
	//I'm doing stuff :)
	/*Controlli:
	 1)Controllo se il semaforo è sul processo
	 2)Controllo se il processo è sul semaforo
	 3)Controllo se count è >0
	 VERO: vuol dire che il processo attuale può essere eseguito subito
	 	 Devo decrementare il count
	 FALSO:
	 	 Decremento
	 	 Aggiungo alla lista di waiting
	 	 Metto il running in stato di waiting
	 */
}
