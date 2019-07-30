#pragma once
#include "disastrOS_semdescriptor.h"

int checkPrel(int semnum);
SemDescriptor* getDes(int sem_id);
SemDescriptorPtr* getPtr(int att_pid,Semaphore*s);