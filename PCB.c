#include "PCB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int  PIDcount = 0;

// Creates a new PCB with given priority, returns pointer to new PCB or NULL if failed
PCB* PCB_create(int priority){
    PCB* new_PCB = malloc(sizeof(PCB));
    if (new_PCB == NULL){
        return NULL;
    }

    new_PCB->PID = PIDcount;
    PIDcount++;
    new_PCB->priority = priority;
    new_PCB->pState = READY;
    memset(new_PCB->recv_msg, '\0', MAX_MSG);
    new_PCB->senderPID = -1;
    memset(new_PCB->send_msg, '\0', MAX_PROCESS_MSG);
    return new_PCB;
}

// Frees the memory allocated for the PCB
void PCB_free(PCB* process){
    if (process == NULL){
        printf("ERROR: Failed to delete process, process is NULL.\n");
        return;
    }
    free(process);
    process = NULL; 
}