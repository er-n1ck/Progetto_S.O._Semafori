#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "myConst.h"
#include "disastrOS_pcb.h"
#include "disastrOS_globals.h"
#include "myConst.h"
#include "operazioni.h"

void internal_semWait(){
	//I'm doing stuff :)
	int semnum=running->syscall_args[0];

	if(checkPrel(semnum)==1){
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
			SemDescriptor* tmpD=getDes(semnum);
			if(tmpD==NULL){
				printf("Errore: Non trovo il semaforo da chiudere fra quelli che appartengono al processo\n");
				running->syscall_retvalue=SEMNUMINVALID;
				return;
			}

			int att_pid=tmpD->pcb->pid;
			SemDescriptorPtr* tmpP=getPtr(att_pid,s);
			if(tmpP==NULL){
				printf("Errore: Non trovo il semDescriptorPtr da chiudere fra quelli che appartengono al semaforo\n");
				running->syscall_retvalue=SEMNUMINVALID;
				return;
			}

			
			s->count--;
			if(s->count<0){
				SemDescriptorPtr* newElem=SemDescriptorPtr_alloc(tmpD);
				assert(List_insert(&s->waiting_descriptors,(ListItem*)s->waiting_descriptors.last,(ListItem*)newElem)!=NULL);
				assert(List_insert(&waiting_list,(ListItem*)waiting_list.last,(ListItem*)running)!=NULL);
				running->status=Waiting;
				PCB* new_proc=(PCB*)List_detach(&ready_list,ready_list.first);
				if(new_proc==NULL){
					running->syscall_retvalue=NEWPROCERROR;
					return;
				}
				running=new_proc;
			}
			
			
			running->syscall_retvalue=semnum;
			return;
		}

	}
}
