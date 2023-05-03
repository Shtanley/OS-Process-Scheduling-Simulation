// Processor Control Block (PCB) header file
#ifndef _PCB_H_
#define _PCB_H_
#include <stdbool.h>

#define MAX_MSG 100
#define MAX_PROCESS_MSG 100

enum process_state{
    RUNNING, READY, BLOCKED
};

enum process_priority{ // Process priority levels
    LOW, MED, HIGH
};

struct PCB{
	int PID;    // Process ID
    int priority;   // Process priority
	int pState;  // Process state
    char recv_msg[MAX_MSG]; // Message received from other process
    int senderPID;   // PID of process that sent the message
    char send_msg[MAX_PROCESS_MSG]; // Message to be sent to other process
}; typedef struct PCB PCB;

// Creates a new PCB with given priority, returns pointer to new PCB or NULL if failed
PCB* PCB_create(int priority);

// Frees the memory allocated for the PCB
void PCB_free(PCB* process);

#endif