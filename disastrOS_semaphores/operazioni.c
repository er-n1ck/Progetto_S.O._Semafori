#include "myConst.h"
#include "disastrOS_globals.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include <stdio.h>



int checkPrel(int semnum){
    if(semaphores_list.size==0){
		printf("////////////////////////////////////////////////////////////////////////////Non ci sono semafori\n");
		running->syscall_retvalue=TOOFEWSEM;
		return 1;
	}
	else if(semnum<0){
		printf("////////////////////////////////////////////////////////////////////////////Numero del semaforo negativo\n");
		running->syscall_retvalue=SEMNUMINVALID;
		return 1;
	}
    return 0;
}

SemDescriptor* getDes(int sem_id){
    SemDescriptor* tmpD=(SemDescriptor*)running->sem_descriptors.first;
    while(tmpD!=NULL){
        if(tmpD->semaphore->id==sem_id) return tmpD;
        else tmpD=(SemDescriptor*)tmpD->list.next;
    }
    return NULL;
}

SemDescriptorPtr* getPtr(int att_pid,Semaphore*s){
    SemDescriptorPtr* tmpP=(SemDescriptorPtr*)(s->descriptors.first);
    while(tmpP!=NULL){
        if(tmpP->descriptor->pcb->pid==att_pid) return tmpP;
        else tmpP=(SemDescriptorPtr*)tmpP->list.next;
    }
    return NULL;
}

int contains(Semaphore* s,PCB* p){
	int ret=0;
	SemDescriptorPtr* ptr=(SemDescriptorPtr*)s->waiting_descriptors.first;
	while(ptr!=NULL){
		if(ptr->descriptor->pcb->pid==p->pid) return 1;
		else ptr=(SemDescriptorPtr*)ptr->list.next;
	}
	return ret;
}