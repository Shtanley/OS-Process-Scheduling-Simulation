#include "commands.h"
#include "PCB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

int main(){
    printf("Booting system...\n");
    start_simulator();  // Initialize simulator
    read_cmd();         // Read user inputs and execute the commands

    // Free all queues and semaphores
    FREE_FN free_fn = &free_item;
    List_free(highQueue, free_fn);
    List_free(medQueue, free_fn);
    List_free(lowQueue, free_fn);
    List_free(recvQueue, free_fn);
    List_free(sendQueue, free_fn);
    for(int i = 0; i < 5; i++) {
        List_free(sem[i].semQueue, free_fn);
    }

    printf("Shutting down...\n");
    return 0;
}

void start_simulator(){
    printf("Starting simulator...\n");
    // 3 ready queues of different priorities
    highQueue = List_create();
    medQueue = List_create();
    lowQueue = List_create();
    
    // 2 wait queues for blocked processes
    // List of blocked processes waiting for a message to be sent
    recvQueue = List_create();
    // List of blocked processes that sent a message and are waiting for a reply
    sendQueue = List_create();

    if(!(highQueue && medQueue && lowQueue && recvQueue && sendQueue)) {
        printf("Queue creation error.\n");
        return;
    } else
        printf("Success: 3 ready queues, 2 wait queues created\n");

    // Create 5 semaphores with initial value -1
    for(int i = 0; i < 5; i++) {
        sem[i].value = -1;
        sem[i].active = false;
        sem[i].semQueue = List_create();
        if(sem[i].semQueue == NULL) {
            printf("Error: Failed to create semaphore\n");
            return;
        } else
            printf("Success: Created semaphore %d\n", i);
    }

    // Init process only runs when no other processes are ready to execute, but it never blocks
    // Init process cannot be killed or exited unless it is the last process in the system
    // after which the simulation terminates
    init = PCB_create(-1);  // -1 = no priority, init passes control to process next on the ready queue
    if(!init) {
        printf("Error: Failied to create init process\n");
        return;
    }
    init->PID = 0;
    init->pState = RUNNING;
    curr = init;
    printf("Running: Process init\n");
}

void read_cmd() {
    // Variables for parameters
    char command;
    int priority;
    int pid;
    int semID = -1;
    int semVal = -1;
    char msg[MAX_MSG];

    printf("\nCommand List:\n"
        "\t(C): Create process\n"
        "\t(F): Fork process\n"
        "\t(K): Kill process\n"
        "\t(E): Exit current process\n"
        "\t(Q): End current time quantum\n"
        "\t(S): Send message to designated process\n"
        "\t(R): Receive message\n"
        "\t(Y): Reply to sender\n"
        "\t(N): Initialize semaphore\n"
        "\t(P): Execute semaphore P() operation\n"
        "\t(V): Execute semaphore V() operation\n"
        "\t(I): Display complete state info of process\n"
        "\t(T): Display all process queues and their info\n");

    while(sysRunning) {    // While system is still running
        printf("\nEnter command: ");
        scanf(" %c", &command);
        switch(command) {
            case 'c':   // Create process
            case 'C':
                printf("Initializing Create process...\n");
                printf("Set priority(low = 0, medium = 1, high = 2): ");
                scanf("%d", &priority);
                printf("\n");
                if (priority == 0 || priority == 1 || priority == 2){
                    Create(priority);
                } else
                    printf("Error: Invalid input. Please try again...\n");
                break;
            
            case 'f':   // Fork process
            case 'F':
                Fork();
                break;

            case 'k':   // Kill process
            case 'K': 
                printf("Initializing Kill process...\n");
                printf("Enter process PID: ");
                scanf("%d", &pid);
                printf("\n");
                Kill(pid);
                break;

            case 'e':   // Exit current process
            case 'E':
                Exit();
                break;

            case 'q':   // End current time quantum
            case 'Q':
                Quantum();
                break;

            case 's':   // Send message to designated process
            case 'S':
                printf("Enter PID of target process: ");
                scanf("%d", &pid);
                printf("\nEnter message: ");
                scanf(" %[^\n]s", msg); // read until newline
                printf("\n");
                // Clear stdin buffer
                fflush(stdin);
                // int c;
                // while ((c = getchar()) != '\n' && c != EOF) { }
                Send(pid, msg);
                break;

            case 'r':   // Receive message
            case 'R':
                Receive();
                break;

            case 'y':   // Reply to sender
            case 'Y':
                printf("Enter PID of target process: ");
                scanf("%d", &pid);
                printf("\nEnter message: ");
                scanf(" %[^\n]s", msg); // read until newline
                printf("\n");
                // Clear stdin buffer
                fflush(stdin);
                // int c;
                // while ((c = getchar()) != '\n' && c != EOF) { }
                Reply(pid, msg);
                break;

            case 'n':   // Initialize semaphore
            case 'N':
                printf("Initializing semaphore...\n");
                while(semID < 0 || semID > 4) {
                    printf("Enter semaphore ID[0 to 4]: ");
                    scanf("%d", &semID);
                }
                while(semVal < 0) {
                    printf("Enter semaphore value[non-negative]: ");
                    scanf("%d", &semVal);
                }
                printf("\n");
                New_sem(semID, semVal);
                break;

            case 'p':   // Execute semaphore P() operation
            case 'P':
                printf("Initializing semaphore P() operation...\n");
                while(semID < 0 || semID > 4) {
                    printf("Enter semaphore ID[0 to 4]: ");
                    scanf("%d", &semID);
                }
                printf("\n");
                Sem_P(semID);
                break;

            case 'v':   // Execute semaphore V() operation
            case 'V':
                printf("Initializing semaphore V() operation...\n");
                while(semID < 0 || semID > 4) {
                    printf("Enter semaphore ID[0 to 4]: ");
                    scanf("%d", &semID);
                }
                printf("\n");
                Sem_V(semID);
                break;

            case 'i':   // Display complete state info of process
            case 'I':
                printf("Initializing Display process info...\n");
                printf("Enter process PID: ");
                scanf("%d", &pid);
                printf("\n");
                Procinfo(pid);
                break;

            case 't':   // Display all process queues and their info
            case 'T':
                Totalinfo();
                break;

            default:
                printf("Error: Invalid input. Please try again...\n");
        }

        // If no process is running, run init
        if(curr == NULL) {
            init->pState = RUNNING;
            curr = init;
            printf("Running: Process init\n");
        }
    }
}

void Create(int priority) {
    PCB* process = PCB_create(priority);
    if(process == NULL) {
        printf("Error: Process creation failed. Returning to Main Menu...\n");
        return;
    }
    printf("Success: Process %d created\n", process->PID);
    // If init is running, make new process the current running process
    if(curr->PID == 0) {
        init->pState = READY;
        process->pState = RUNNING;
        curr = process;
        printf("Process %d is running\n", process->PID);
        return;
    }
    // Place current process in the appropriate ready queue
    if(process->priority == HIGH)
        List_append(highQueue, process);
    else if(process->priority == MED)
        List_append(medQueue, process);
    else
        List_append(lowQueue, process);
}

void Fork() {
    if(curr->PID == 0) {  // Cannot fork init process
        printf("Error: Cannot fork init process. Returning to Main Menu...\n");
        return;
    }
    // Fork process
    PCB* fp = PCB_create(curr->priority);   // Create new process with same priority as current process
    fp->senderPID = curr->senderPID;    // Copy senderPID
    // Copy message buffers
    strncpy(fp->recv_msg, curr->recv_msg, MAX_MSG);
    strncpy(fp->send_msg, curr->send_msg, MAX_PROCESS_MSG);
    // Add forked process to the appropriate ready queue
    if(fp->priority == HIGH)
        List_append(highQueue, fp);
    else if(fp->priority == MED)
        List_append(medQueue, fp);
    else
        List_append(lowQueue, fp);

    printf("Fork: process %d created\n", fp->PID);
}

void Kill(int pid) {
    if(curr->PID == pid) {   // Kill current process in Exit()
        Exit();
        return;
    } else if(pid == 0) {   // Cannot kill init process
        printf("Error: Cannot terminate init process. Returning to Main Menu...\n");
        return;
    } else {    // Search and kill process
        List* searchQueue = search_process(pid);
        if(searchQueue) {
            for(int i = 0; i < 5; i++) {    // Case: Kill process blocked on semaphore and increment semaphore value
                if(searchQueue == sem[i].semQueue) {
                    printf("The process was blocked on Semaphore %d\n", i);
                    printf("Incrementing Semaphore value by 1\n");
                    sem[i].value += 1;
                    printf("Semaphore %d now has value %d\n", i, sem[i].value);
                }
            }

            PCB_free(List_remove(searchQueue)); // Free process
            printf("Success: Terminated Process %d\n", pid);
        } else
            printf("Error: Process %d not found\n", pid);
    }
}

void Exit() {
    // Cannot exit init process if there are other processes in the system
    if(curr->PID == 0) {
        if(List_count(highQueue) > 0 || List_count(medQueue) > 0 || List_count(lowQueue) > 0 || 
            List_count(sem[0].semQueue) > 0 || List_count(sem[1].semQueue) > 0 || List_count(sem[2].semQueue) > 0 ||
            List_count(sem[3].semQueue) > 0 || List_count(sem[4].semQueue) > 0 || List_count(recvQueue) > 0 ||
            List_count(sendQueue) > 0) {
            printf("Error: Cannot exit init process. Returning to Main Menu...\n");
        } else {
            PCB_free(init);
            printf("Success: Init process terminated. Shutting down...\n");
            sysRunning = false;
        }
        return;
    }
    // Free current process
    PCB_free(curr);
    curr = NULL;
    printf("Success: Terminated current process\n");
    switch_process();
}

void Quantum() {
    printf("Time quantum reached...\n");
    if(curr != init) {
        // Lower priority of current process
        // if(curr->priority == HIGH) {
        //     curr->priority = MED;
        //     List_append(medQueue, curr);
        //     printf("Priority changed to medium(MED)\n");
        // } else if(curr->priority == MED) {
        //     curr->priority = LOW;
        //     List_append(lowQueue, curr);
        //     printf("Priority changed to low(LOW)\n");
        // } else
        //     List_append(lowQueue, curr);
        curr->pState = READY;
        // pick next current from ready processes; switch to init if none
        switch_process();
    } else
        printf("Runnig: init\n");
}

void Send(int pid, char* msg) {
    PCB* target = NULL;
    List* searchQueue = NULL;
    // Search for process
    if(pid == curr->PID) {
        printf("Error: Cannot send message to self. Returning to Main Menu...\n");
        return;
    } else if(pid == 0) {
        target = init;
    } else {
        searchQueue = search_process(pid);
        if(searchQueue) {
            target = List_curr(searchQueue);
            if(target->pState == BLOCKED) {
                printf("Error: Process %d is blocked. Returning to Main Menu...\n", pid);
                return;
            }
        } else {
            printf("Error: Process %d not found. Returning to Main Menu...\n", pid);
            return;
        }
    }
    // If current is not init, block current process
    if(curr != init) {
        curr->pState = BLOCKED;
        List_append(sendQueue, curr);
        printf("Success: Process %d sent a message and is now blocked. \nWaiting for reply...\n", curr->PID);
    } else 
        printf("Success: Process %d sent a message. Cannot block init process\n", curr->PID);
    
    // If recipient is in blocked queue, unblock and copy message to display
    if(searchQueue == recvQueue) {
        target->pState = READY; // Unblock process
        strncpy(target->recv_msg, msg, MAX_MSG);
        target->senderPID = curr->PID;
        // sprintf(target->send_msg, "Process %d sent a message: %s\n", curr->PID, target->recv_msg);
        // target->recv_msg[0] = '\0'; // Clear message to prevent duplicates

        List_remove(recvQueue);
        printf("Success: Process %d received a message, process ublocked\n", target->PID);
        if(target->priority == HIGH)
            List_append(highQueue, target);
        else if(target->priority == MED)
            List_append(medQueue, target);
        else
            List_append(lowQueue, target);
    } else {    // If recipient is not in blocked queue, copy message and sender
        strncpy(target->recv_msg, msg, MAX_PROCESS_MSG);
        target->senderPID = curr->PID;
        printf("Success: Process %d received a message\n", target->PID);
        if(curr != init)
            switch_process();
    }
}

void Receive() {
    if(curr->recv_msg[0] != '\0') {
        printf("Process %d sent a message: %s\n", curr->senderPID, curr->recv_msg);
        curr->recv_msg[0] = '\0'; // Clear message to prevent duplicates
        curr->senderPID = -1;   // Clear senderPID
    } else {    // If no message, block process unless it is init
        if(curr != init) {
            curr->pState = BLOCKED;
            List_append(recvQueue, curr);
            printf("No messages: Process %d is now blocked. Waiting for message...\n", curr->PID);
            switch_process();
        } else
            printf("No messages: Cannot block init process. Returning to Main Menu...\n");
    }
}

void Reply(int pid, char* msg) {
    // Search for process using PID
    if(pid == curr->PID) {
        printf("Error: Cannot reply to self. Returning to Main Menu...\n");
        return;
    }

    PCB* sender = NULL;
    if(pid == 0) {
        sender = init;
    } else {
        List* searchQueue = search_process(pid);
        if(searchQueue) {
            sender = List_curr(searchQueue);
            if(sender->pState == BLOCKED) {
                printf("Error: Process %d is blocked. Returning to Main Menu...\n", pid);
                return;
            }
        } else if(searchQueue != sendQueue) {
            printf("Error: Process %d has not sent a message. Returning to Main Menu...\n", pid);
            return;
        } else {
            printf("Error: Process %d not found. Returning to Main Menu...\n", pid);
            return;
        }
    }
    // Unblock sender and copy message to display
    sender->pState = READY; // Unblock process
    strncpy(sender->recv_msg, msg, MAX_MSG);
    // sprintf(sender->send_msg, "Process %d sent a message: %s\n", curr->PID, sender->recv_msg);
    // sender->recv_msg[0] = '\0'; // Clear message to prevent duplicates
    printf("Success: Process %d received a reply\n", sender->PID);
    
    // If current is not init, block current process
    if(sender != init) {
        if(curr == init) {  // If current is init, unblock sender
            switch_process();
        }
        if(sender->priority == HIGH)
            List_append(highQueue, sender);
        else if(sender->priority == MED)
            List_append(medQueue, sender);
        else
            List_append(lowQueue, sender);
    }
}

void New_sem(int semID, int value) {
    if(semID < 0 || semID > 4) {
        printf("Error: Invalid semaphore ID [Valid ID = 0 to 4]. Returning to Main Menu...\n");
        return;
    }
    if(sem[semID].active) {
        printf("Error: Semaphore %d already exists. Returning to Main Menu...\n", semID);
        return;
    }
    if(value < 0) {
        printf("Error: Invalid semaphore value, value must be positive. Returning to Main Menu...\n");
        return;
    }
    sem[semID].value = value;
    sem[semID].active = true;
    printf("Success: Semaphore %d created with value %d\n", semID, value);
}

void Sem_P(int semID) {
    if(semID < 0 || semID > 4) {
        printf("Error: Invalid semaphore ID [Valid ID = 0 to 4]. Returning to Main Menu...\n");
        return;
    }
    if(!sem[semID].active) {
        printf("Error: Semaphore %d does not exist. Returning to Main Menu...\n", semID);
        return;
    }
    // Decrement semaphore value
    sem[semID].value -= 1;
    if(sem[semID].value > 0) {
        printf("Success: Process %d did P() on Semaphore %d (value: %d). Process is not blocked\n", curr->PID, semID, sem[semID].value);
    } else {    // Semaphore value is < 0, block process
        if(curr->PID == 0) {
            printf("Success: Process %d did P() on Semaphore %d (value: %d). Unable to block process init\n", curr->PID, semID, sem[semID].value);
            // return;
        } else {
            printf("Success: Process %d did P() on Semaphore %d (value %d). Process blocked\n", curr->PID, semID, sem[semID].value);
            curr->pState = BLOCKED;
            List_append(sem[semID].semQueue, curr);
        }
    }
    // If current process is blocked on semaphore, switch to next process
    if(curr->pState == BLOCKED)
        switch_process();
}

void Sem_V(int semID) {
    if(semID < 0 || semID > 4) {
        printf("Error: Invalid semaphore ID [Valid ID = 0 to 4]. Returning to Main Menu...\n");
        return;
    }
    if(!sem[semID].active) {
        printf("Error: Semaphore %d does not exist. Returning to Main Menu...\n", semID);
        return;
    }
    // Increment semaphore value
    sem[semID].value += 1;
    // Unblock process if semaphore value is <= 0
    if(sem[semID].value <= 0) {
        List_first(sem[semID].semQueue);
        PCB* process = List_remove(sem[semID].semQueue);
        if(!process) {
            printf("Error: Semaphore %d queue is empty. Returning to Main Menu...\n", semID);
            return;   
        }
        printf("Success: Process %d did V() on Semaphore %d (value: %d). Process unblocked\n", process->PID, semID, sem[semID].value);
        process->pState = READY;
        
        // Place process in ready queue, if init is running switch to new process
        if(curr->PID == 0) {
            switch_process();
        } else {
            if(process->priority == HIGH)
                List_append(highQueue, process);
            else if(process->priority == MED)
                List_append(medQueue, process);
            else
                List_append(lowQueue, process);
        }
        process = NULL;
    } else {
        printf("Success: Process %d did V() on Semaphore %d (value: %d). Process is not blocked\n", curr->PID, semID, sem[semID].value);
    }
    
}

void Procinfo(int pid) {
    if(pid == 0) {  // Display info for init process
        printf("Process ID: %d\n", init->PID);
        printf("Priority: %d\n", init->priority);
        printf("State: %d\n", init->pState);
        printf("Sender PID: %d\n", init->senderPID);
        printf("Receive Message: %s\n", init->recv_msg);
        printf("Send Message: %s\n", init->send_msg);
        return;
    }
    PCB* process = NULL;
    List* searchQueue = NULL;
    if(curr->PID == pid) {  // If current process is the one being searched for
        printf("Running: Process %d\n", curr->PID);
        process = curr;
    } else {    // If process is in a queue
        searchQueue = search_process(pid);
        if(searchQueue) {
            process = List_curr(searchQueue);
            printf("Process %d is not running\n", pid);
        } else {
            printf("Error: Process %d does not exist. Returning to Main Menu...\n", pid);
            return;
        }
    }

    printf("Process ID: %d\n", process->PID);
    printf("Priority: %d\n", process->priority);
    printf("Sender PID: %d\n", process->senderPID);
    printf("Receive Message: %s\n", process->recv_msg);
    printf("Send Message: %s\n", process->send_msg);
    printf("State: %d\n", process->pState);
    if(process->pState == BLOCKED) {
        if(searchQueue == sendQueue)
            printf("\tProcess is blocked on send, waiting for reply\n");
        else if(searchQueue == recvQueue)
            printf("\tProcess is blocked on receive, waiting for message\n");
        else if (searchQueue == sem[0].semQueue)
            printf("\tProcess is blocked on semaphore 1\n");
        else if (searchQueue == sem[1].semQueue)
            printf("\tProcess is blocked on semaphore 2\n");
        else if (searchQueue == sem[2].semQueue)
            printf("\tProcess is blocked on semaphore 3\n");
        else if (searchQueue == sem[3].semQueue)
            printf("\tProcess is blocked on semaphore 4\n");
        else if (searchQueue == sem[4].semQueue)
            printf("\tProcess is blocked on semaphore 5\n");
        else 
            printf("\t Error: Process is blocked but not on a queue");
    }
}

void Totalinfo() {
    printf("Displaying all process queues and their info...\n\n");
    printf("High Priority Queue: ");
    print_queue(highQueue);
    printf("Medium Priority Queue: ");
    print_queue(medQueue);
    printf("Low Priority Queue: ");
    print_queue(lowQueue);
    printf("\nReceive Queue: ");
    print_queue(recvQueue);
    printf("Send Queue: ");
    print_queue(sendQueue);
    printf("\nSemaphore 0 Queue: ");
    print_queue(sem[0].semQueue);
    printf("Semaphore 1 Queue: ");
    print_queue(sem[1].semQueue);
    printf("Semaphore 2 Queue: ");
    print_queue(sem[2].semQueue);
    printf("Semaphore 3 Queue: ");
    print_queue(sem[3].semQueue);
    printf("Semaphore 4 Queue: ");
    print_queue(sem[4].semQueue);
}

// -------------------------------------- Helper Functions --------------------------------------

// Function to compare integers, used by search_process().
bool compare_int(void* pItem, void* pComp){
    if (((PCB*)pItem)->PID == *((int*)(pComp))){
        return true;
    }
    return false;
}

// Search all queues for the process with the given pid
List* search_process(int pid) {
    List* returnQueue = NULL;
    if (curr->PID == pid){
        printf("Current process is the target of search\n");
        return NULL;
    }

    COMPARATOR_FN compare = &compare_int;
    List_first(highQueue);
    if(List_search(highQueue, compare, &pid)) { // Search high priority queue
        // printf("Target process is in the high priority queue\n");
        returnQueue = highQueue;
    }

    List_first(medQueue);
    if(List_search(medQueue, compare, &pid)) {  // Search medium priority queue
        // printf("Target process is in the medium priority queue\n");
        returnQueue = medQueue;
    }

    List_first(lowQueue);
    if(List_search(lowQueue, compare, &pid)) {  // Search low priority queue
        // printf("Target process is in the low priority queue\n");
        returnQueue = lowQueue;
    }

    List_first(recvQueue);
    if(List_search(recvQueue, compare, &pid)) { // Search receive queue
        // printf("Target process is in the receive queue\n");
        returnQueue = recvQueue;
    }

    List_first(sendQueue);
    if(List_search(sendQueue, compare, &pid)) { // Search send queue
        // printf("Target process is in the send queue\n");
        returnQueue = sendQueue;
    }

    for(int i = 0; i < 5; i++) {    // Search all semaphore queues
        List_first(sem[i].semQueue);
        if(List_search(sem[i].semQueue, compare, &pid)) {
            // printf("Target process is in the semaphore %d queue\n", i);
            returnQueue = sem[i].semQueue;
        }
    }

    return returnQueue;
}

// Function to switch to the next process in the ready queue or init if no processes in ready queue
void switch_process() {
    printf("Switching to next process...\n");
    // Place current process in temp and set to ready
    // if(curr != init) {
    //     PCB* temp = curr;
    //     temp->pState = READY;
    //     // Add current process to appropriate queue
    //     if(temp->priority == 0) {
    //         List_append(highQueue, temp);
    //     } else if(temp->priority == 1) {
    //         List_append(medQueue, temp);
    //     } else if(temp->priority == 2) {
    //         List_append(lowQueue, temp);
    //     }
    // }

    // Set current process to next process in ready queue based on priority
    if(List_count(highQueue)) {
        List_first(highQueue);
        curr = List_remove(highQueue);
        curr->pState = RUNNING;
    } else if(List_count(medQueue)) {
        List_first(medQueue);
        curr = List_remove(medQueue);
        curr->pState = RUNNING;
    } else if(List_count(lowQueue)) {
        List_first(lowQueue);
        curr = List_remove(lowQueue);
        curr->pState = RUNNING;
    } else {
        curr = init;
        curr->pState = RUNNING;
    }

    // Print process message
    if(curr->PID == 0)    // Case: Init process
        printf("Running: Init process\n");
    // else if (*curr->send_msg != '\0') { // Case: Process has a message to send, print message and clear message
    //     printf("Running: Process %d contains a message to send\n", curr->PID);
    //     printf("%s\n", curr->send_msg);
    //     *curr->send_msg = '\0';
    // } 
    else  // Case: Process has no message to send
        printf("Running: Process %d\n", curr->PID);
}

// Function to print the processes of a given queue, used by Totalinfo()
void print_queue(List* queue) {
    if(!queue) {    // Case: Queue does not exist
        printf("Error: queue does not exist\n");
        return;
    }

    if(List_count(queue) == 0) {    // Case: Queue is empty
        printf("\n");
        return;
    }

    PCB* proc = List_first(queue);
    while (proc != NULL){   // Print all processes in queue
        printf("%d | ", proc->PID);
        proc = List_next(queue);
    }
    printf("\n");

    free(proc);
}

// Free function for PCB, does nothing
void free_item(void *pItem) {}
