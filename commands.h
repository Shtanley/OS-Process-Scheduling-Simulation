#ifndef _COMMANDS_H_
#define _COMMANDS_H_
#include "list.h"
#include "PCB.h"
#include "semaphore.h"
#include <stdbool.h>

struct semaphore{
    int value;
    List* semQueue;
    bool active;
};
typedef struct semaphore semaphore;

static List* highQueue;
static List* medQueue;
static List* lowQueue;
static List* recvQueue;
static List* sendQueue;
static semaphore sem[5];
static PCB* init;
static PCB* curr;
static bool sysRunning = true;

// Initialize the simulator (ready/blocked queues, semaphores, etc.)
void start_simulator();

// Read user inputs and execute the commands
void read_cmd();

// (C) Create a process with the given priority and add it to the appropriate ready Q
// Report success/failure and return the PID of the created process
// If no other process is running, except the init process, run the new process
void Create(int priority);

// (F) Copy current process and put it into the ready Q with original process' priority
// Report success/failure and return the PID of the created process
// Forking init process should fail
void Fork();

// (K) Kill the named process and remove it from the system
// Report success or failure
// Allows you to kill a process even when it is not currently executing (e.g. in blocked queue)
void Kill(int pid);

// (E) Only Kill the current process
// Report process scheduling info (e.g. which process now takes over the CPU)
void Exit();

// (Q) Time quantum for currently running process expires
// Report action taken (e.g. process scheduling info)
// ONLY way to signal that the time quantum for round robin scheduling has expired
//  - when this occurs we must choose the next process to execute from the appropriate ready queue
//      (or just the init process if no processes are ready)
//  - when a new process becomes ready of a higher priority than the currently executing process, you
//      do NOT need to pre-empt the currently executing process; just wait until quantum expires
void Quantum();

// (S) Sends a message to specified process, place in blocked queue till reply is received
// Report success/failure, scheduling info, and reply message and sender's PID (once received)
// sends the message to the named process and places the sender on a blocked queue
//  - you must put the message somewhere so that the named process will be able to receive it when it
//      is next executed
void Send(int pid, char* msg);

// (R) Receive a message, place in blocked queue till message is received 
// Report scheduling info and message and sender's PID (once received)
// Else, put process into the blocked queue to wait for a "Send".
//  -checks if there is a message waiting for the currently executing process, if there is, it
//      receives it, otherwise it gets blocked
void Receive();

// (Y) Delivers reply to sender (works similar to Send) and unblocks the sender
// Report success/failure
void Reply(int pid, char* msg);

// (N) Initializes the named semaphore with the value given.
// ID's can take a value from 0 to 4. Can only be done once per semaphore.
// Report success/failure and action taken (e.g. scheduling info)
void New_sem(int semID, int value);

// (P) Executes semaphore P(block) operation on the named semaphore
// Report action taken(blocked/unblocked) and success/failure
void Sem_P(int sem_ID);

// (V) Executes semaphore V(unblock) operation on the named semaphore
// Report action taken(weather/which process was readied) and success/failure
void Sem_V(int sem_ID);

// (I) Prints the complete state info of process to the screen
// Report action
void Procinfo(int pid);

// (T) Displays all process queues and their contents
void Totalinfo();

// -------------------------------------- Helper Functions --------------------------------------

// Function to compare integers, used by search_process().
bool compare_int(void* pItem, void* pComp);

// Search all queues for the process with the given pid
// Returns a pointer to the queue with process as the current item return NULL if not found
List* search_process(int pid);

// Selects a process from the ready queues based on priority and sets it to running
// If there is no process ready, switch to "init" process
// Print process_msg of the new process if it exists
void switch_process();

// Function to print the processes of a given queue, used by Totalinfo()
void print_queue(List* queue);

// Callback function to free a PCB, used by List_free()
void free_item(void *pItem);

#endif