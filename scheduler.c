//By: Warren Wnuck
//
//CSC 139 - 02
//Scheduler.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
// values for each node
struct Process{
    int pid, arrival, timeLeft, lastActive, waitingTime, response;
};

// link list to create temporary data
struct LinkedList{
    struct Process data;
    struct LinkedList *next;
};

typedef struct LinkedList *node;

node new_n();
node add_n(node n, struct Process p);
node remove_n(node n);
int turnAround(int arrival, int current, int turnAround);
int wait(int current, int lastActive);
int response(int arrival, int current, int response);
node rebuild_n(node oldNode, node new_n, int time);
node insertSort(node n, struct Process p);
node rebuildSort_n(node oldNode, node new_n, int time);
node update(node head, int t);
void fcfs(node n);
void srtf(node n, int c);
void rr(node head, int quantum, int tasks);


int main(int argc, char *argv[]){
    char cpuScheduleType[5];    
    int quantum = -1;
    int runOption = -1;

    if (argc == 3){
        
        sscanf(argv[2], "%s", cpuScheduleType);
        if (strcmp(cpuScheduleType, "FCFS") == 0){
            printf("You have chosen the CPU Scheduling Algorithm: %s\n", 
                   cpuScheduleType);
            runOption = 0;
        }

        else if (strcmp(cpuScheduleType, "SRTF") == 0){
            printf("You have chosen the CPU Scheduling Algorithm: %s\n", 
                   cpuScheduleType);
            runOption = 1;
        }

        else{
            printf("Usage: CPUSchedulingSimulation [File] [FCFS | RR | SRTF] ");
            printf("[Quntum Time]\n");
            return 0;
        }
    }

    else if (argc == 4){
        sscanf(argv[2], "%s", cpuScheduleType);
        sscanf(argv[3], "%d", &quantum);

        if (quantum < 1){
            printf("You need to have a [Quantum] Value >= 1\n");
            return 0;
        }

        if (strcmp(cpuScheduleType, "RR") == 0){
            printf("You have chosen the CPU Scheduling Algorithm: %s\n", 
                   cpuScheduleType);
            runOption = 2;
        }

        else{
             
            printf("Usage: proj2 [File] [FCFS | RR | SRTF] ");
            printf("[Quntum Time]\n");
            return 0;
        }
    }

    else{
        
        printf("Usage: proj2 [File] [FCFS | RR | SRTF] ");
        printf("[Quntum Time]\n");
        return 0;
    }

    
    FILE *proccesses = fopen(argv[1], "r");
    node head = new_n();

    
    int pid, arrival, timeLeft;
    struct Process process;

    int count = 0; 

    
    while (fscanf(proccesses, "%d %d %d", &pid, &arrival, &timeLeft) > 0){
        process.pid = pid;
        process.arrival = arrival;
        process.timeLeft = timeLeft;
        process.response = -1;
        process.waitingTime = 0;
        process.lastActive = process.arrival;
        count++;
        head = add_n(head, process);
    }
    
    head = remove_n(head);
    fclose(proccesses);

    printf("Total %d task have been read from %s. Press 'Enter' to start...", count, argv[1]);
    
    int c = getc(stdin);

    printf("==============================================================================\n");
    if(c == '\n'){
        if(runOption == 0)
            fcfs(head);
        else if (runOption == 1)
            srtf(head, count);
        else
            rr(head, quantum, count);
    }
    return 0; 
}

// this method creates a new node with set values
node new_n(){
    node temp;
    struct Process process;
    
    
    process.pid = -1;
    process.arrival = -1;
    process.timeLeft = -1;
   
    temp = (node)malloc(sizeof(struct LinkedList));
    temp->data = process;
    temp->next = NULL;

    return temp;
}

// this method add the node to a list
node add_n(node n, struct Process p){
    node temp0, temp1;
    temp0 = new_n();
    temp0->data = p;

    if (n == NULL)
        n = temp0;

    else{
        temp1 = n;
        
        while (temp1->next != NULL)
            temp1 = temp1->next;
        
        temp1->next = temp0;
    }

    return n; 
}

// this method removes nodes from list
node remove_n(node n){
    node next;
    next = n->next;

    return next;
}

// method to rebuild the node structure
node rebuild_n(node oldNode, node new_n, int time){
    struct Process p;

    while(oldNode != NULL){
        p = oldNode->data;

        if (p.arrival <= time)
            new_n = add_n(new_n, p);
        oldNode = remove_n(oldNode);
    }
    return new_n;
}

// this is the the fcfs algorithm.
void fcfs(node n){
    float turnAroundTime = 0;
    float waitTime = 0;
    float responseTime = 0;
    float cpuSimulation;
    struct Process process;
    node head = n; 
    node current = n;
    int currentTime = 0; 
    int workTime = 0;
    

    if(current == NULL)
        return; 

    process = current->data;
    responseTime = process.arrival;

    while (current != NULL){
        if (process.arrival <= currentTime){
            if(process.timeLeft == 0){
                printf("<System Time:\t%d> process \t%d has finished......\n",
                       currentTime, process.pid);
                waitTime += process.waitingTime;
                current = remove_n(current);
                turnAroundTime = turnAround(process.arrival, currentTime, turnAroundTime);
                
                if (current != NULL){
                    process = current->data;
                    process.waitingTime += wait(currentTime, process.lastActive);

                    while (process.waitingTime > currentTime){
                        currentTime++;
                        printf("<System time:\t%d> No process is currently running.\n",
                         currentTime);
                    }

                    responseTime = response(process.arrival, currentTime, 
                                                responseTime);
                }

                else{
                    printf("<System time:\t%d> All processes have finished.........................\n", 
                           currentTime);
                    break;
                }
            }
            
            printf("<System time:\t%d> process \t%d is running\n", currentTime, 
                   process.pid);
            process.timeLeft--;
            workTime++;
        }

        currentTime++;
    }

    cpuSimulation = (((float)workTime / (float)currentTime) * 100);
    printf("=============================================================================\n");
    printf("Average CPU usage:\t\t%0.2f %\n", cpuSimulation);
    printf("Average waiting time:\t\t%0.2f\n", waitTime/6);
    printf("Average response time:\t\t%0.2f\n", responseTime/6);
    printf("Average turn around time:\t%0.2f\n", turnAroundTime/6);
    printf("=============================================================================\n");
}

// this is the srtf algorithm
void srtf(node n, int c){
    float response = 0;
    float waitTime = 0;
    struct Process process;
    int currentTime = 0;
    int workTime = 0;
    int turnAroundTime = 0;
    int execution = 0;
    
    node tempNode = new_n();

    tempNode = rebuildSort_n(n, tempNode, currentTime);
    n = update(n, currentTime);
    process = tempNode->data;
    response1 = process.arrival;
    process.response = 1;

    while(c > 0){
        if (process.arrival <= currentTime){
            workTime++;
            printf("<system time\t%d> process\t%d is running\n", currentTime, process.pid);
            process.timeLeft--;
            currentTime++;

            if(process.timeLeft <= 0){
                printf("<system time\t%d> process\t%d is finished.......\n", currentTime, process.pid);
                turnAroundTime = turnAround(process.arrival, currentTime, turnAroundTime);
                waitTime += process.waitingTime;
                tempNode = remove_n(tempNode);
                c--;

                if (c > 0){
                    while (tempNode == NULL){
                        printf("<System time:\t%d> No process is currently running\n", currentTime);
                        currentTime++;
                        tempNode = rebuildSort_n(n, tempNode, currentTime);
                        n = update(n, currentTime);
                    }

                    process = tempNode->data;
                    process.waitingTime += wait(currentTime, process.lastActive);
                    process.lastActive = currentTime;
                    execution = process.timeLeft;

                    if(process.response != 1){
                        response1 = response(process.arrival, currentTime, response1);
                        process.response = 1;
                    }
                }

                else {
                    printf("<System time:\t%d> All processes have finished...................\n", currentTime);
                    break;
                }
            }
            process.lastActive = currentTime;
            tempNode->data = process;
            tempNode = rebuildSort_n(n, tempNode, currentTime);
            n = update(n, currentTime);
            process = tempNode->data;
            process.waitingTime += wait(currentTime, process.lastActive);

            if (process.response != 1){
                response1 = response(process.arrival, currentTime, response1);
                process.response = 1;
            }
        }

        else 
            currentTime++;
    }
    float cpuSimultation = ((float) workTime / (float)currentTime) * 100;
    printf("===============================================================================\n");
    printf("Average CPU usage:\t\t%0.2f %\n", cpuSimultation);
    printf("Average waiting time:\t\t%0.2f\n", waitTime/6);
    printf("Average response time:\t\t%0.2f\n", response1/6);
    printf("Average turn around time:\t%0.2f\n", (float)turnAroundTime/6);
    printf("===============================================================================\n");
}

//this is the rr algorithm
void rr(node head, int quantum, int tasks){
    int currentTime = 0;
    int workTime = 0;
    int turnAroundTime = 0;
    int waitTime = 0;
    int maxQuantum = quantum;
    int currentQuatum = quantum;
    node tempNode = new_n();
    tempNode = rebuild_n(head, tempNode, currentTime);
    tempNode = remove_n(tempNode);
    head = update(head, currentTime);
    struct Process p = tempNode->data;
    
    int responseTime = p.arrival;
    p.response = 1;
    int executionTime = p.timeLeft;

    while (tasks > 0){
        if (p.arrival <= currentTime)
        {
            workTime++;
            printf("<system time\t%d> process \t%d is running\n", currentTime, p.pid);
            p.timeLeft--;
            currentTime++;
            currentQuatum--;

            if (p.timeLeft <= 0){
                printf("<system time\t%d> process\t%d is finished.......\n", currentTime, p.pid);
                turnAroundTime = turnAround(p.arrival, currentTime, turnAroundTime);
                waitTime += p.waitingTime;
                tempNode = remove_n(tempNode);
                tasks--;

                if(tasks > 0){
                    while (tempNode == NULL){
                        printf("<system time\t%d> no process is running\n", currentTime);
                        currentTime++;
                        tempNode = rebuild_n(head, tempNode, currentTime);
                        head = update(head, currentTime);
                    }

                    p = tempNode->data;
                    p.waitingTime = p.waitingTime + wait(currentTime, p.lastActive);
                    p.lastActive = currentTime;
                    executionTime = p.timeLeft;

                    if(p.response != 1){
                        responseTime = response(currentTime, p.arrival, responseTime);
                        p.response = 1;
                    }
                }
                else {
                    printf("<system time\t%d> All processes finish ....................\n", currentTime);
                    break;
                }
                currentQuatum = maxQuantum;
            }
            tempNode = rebuild_n(head, tempNode, currentTime);
            head = update(head, currentTime);

            if (currentQuatum == 0){
                p.lastActive = currentTime;
                tempNode = remove_n(tempNode);
                tempNode = add_n(tempNode, p);
                p = tempNode->data;
                p.waitingTime += wait(currentTime, p.lastActive);
                executionTime = p.timeLeft;

                if (p.response != 1){
                    responseTime = response(currentTime, p.arrival, responseTime);
                    p.response = 1;
                }
                currentQuatum = maxQuantum;
            }
        }
        else{
            currentTime++;
            tempNode = rebuild_n(head, tempNode, currentTime);
            head = update(head, currentTime);
        }
    }

    responseTime = 0 - responseTime;
    float cpu = ((float)workTime/(float)currentTime)*100;
    printf("=================================================================================\n");
    printf("Average Cpu Usage          : %0.2f %\n", cpu);
    printf("Average Waiting Time       : %0.2f\n", (float)waitTime/6);
    printf("Average Response Time      : %0.2f\n", (float)responseTime/6);
    printf("Average Turnaround Time    : %0.2f\n", (float)turnAroundTime/6);
    printf("=================================================================================\n");
}

// method to calculate turn around 
int turnAround(int arrival, int current, int turnAround){
    int difference = 0;

    difference = current - arrival;
    turnAround = turnAround + difference;
    
    return turnAround;
}

//method to calculate the wait time
int wait(int current, int lastActive){
    int difference = 0;

    difference = current - lastActive;

    return difference;
}

// method to calculate the response
int response(int arrival, int current, int response){
    int difference = 0;

    difference = current - arrival;
    response = response + difference;

    return response;
}

// this is a sorting method to sorth node structure
node insertSort(node n, struct Process p){
    node this = n; 
    node next = this->next;
    node new;
    node head;
    struct Process newP, nextP;
    new = new_n();
    new = add_n(new, p);
    new = remove_n(new);
    newP = this->data;

    if (p.timeLeft < newP.timeLeft) {
        new->next = this;
        return new;
    }

    if (this->next == NULL)  {
        this->next = new;
        return this;
    }

    while (this != NULL){
        newP = this->data;
        nextP = next->data;
        if( p.timeLeft < nextP.timeLeft) {
            new->next = next;
            this->next = new;
            return n;
        }
        if(next->next == NULL){
            next->next = new;
            return head;
        }
        next = next->next;
        this = this->next;
    }
    return new;
}

// this method sorts structure and rebuilds node structure
node rebuildSort_n(node oldNode, node new_n, int time){
    struct Process p0, p1;

    while (oldNode != NULL){
        p0 = oldNode->data;

        if (p0.arrival <= time){
            if (new_n == NULL)
                new_n = add_n(new_n, p0);

            p1 = new_n->data;
            
            if (p1.pid == -1){
                new_n = add_n(new_n, p0);
                new_n = remove_n(new_n);
            }
            else
                new_n = sortedInsert(new_n, p0);
        }
        oldNode = remove_n(oldNode);
    }
    return new_n;
}

// this method is to update the node structure
node update(node head, int t){
    struct Process p;

    while (head != NULL){
        p = head->data;

        if (p.arrival <= t)
            head = remove_n(head);
        else
            break;
    }

    return head; 
}
