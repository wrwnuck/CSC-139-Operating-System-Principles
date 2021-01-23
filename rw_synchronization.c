//By: Warren Wnuck
//
//CSC 139 - 02
//rw_synchronization.c

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

void* reader(void*);
void* writer(void*);
 
sem_t mutex;    
sem_t db;       
sem_t q;        
int rc = 0;     
int sleepval = 10;
int readerCount = 0;
int writerCount = 0;
clock_t before;
int msec = 0, trig = 3000; // 30 seconds 

int main(int argc, char *argv[]){
  
  int i = 0;
  int n = 0;
  int readers = 0;
  int writers = 0;

  before = clock();
  // checks if correct amount of input
  if(argc != 3){
	  printf("Error need to type: ./a.out [Number of Readers] [Number of Writers]\n");
	  exit(EXIT_FAILURE);
  }

  sscanf(argv[1], "%d", &readers);
  sscanf(argv[2], "%d", &writers);

  // checks the inputs are in the correct range inclusively
  if(writers > 10 || writers < 1 || readers > 100 || readers < 1){
	  printf("Error need to have 1-10 writers and 1-100 readers\n");
	  exit(EXIT_FAILURE);
  }else{

    pthread_t allreader[readers],writerTh;
    int dbase[readers];
    int ids[readers];
    int ids2[writers];

    sem_init(&mutex,0,1);
    sem_init(&db,0,1);
    sem_init (&q,0,1);
    
    // creates reader threads
    for(i = 0; i < readers; i++){
        
        ids[i]=i+1;  
        if(pthread_create(&allreader[i],0,reader,&ids[i])!=0){
	        perror("Cannot create reader\n");
	        exit(1);                            
        }
    }

    // creates writer threads
    for(i = 0; i < writers; i++){
      ids2[i]=i+1;
      if(pthread_create(&writerTh,0,writer,&ids2[i])!=0){
        perror("Cannot create writer\n");     
        exit(1);
      }
      
    }
    
    pthread_join(writerTh,0); 

    printf("\nNumber of Reads: %d\tNumber of Writes: %d\n\n", (readerCount-1), (writerCount-1)); 
    sem_destroy(&mutex);
    sem_destroy(&db);
    sem_destroy (&q);
  }
  return 0;
}

// reader method to establish readers
void* reader(void*arg){
  int i = *(int*)arg;
  int value;
  double current = 0;
  clock_t diff = 0;
  do{
    sem_wait(&q);
    sem_wait(&mutex);
    rc++;
    if(rc == 1) sem_wait(&db);
    sem_post(&mutex);
    sem_post (&q);
    printf("Reader %d reads %d\n",readerCount, sleepval);
    readerCount+=1;
    usleep(rand()% (500-100+1)+100);
    sem_wait(&mutex);
    rc--;
    if(rc == 0) 
      sem_post(&db);
    sem_post(&mutex);   
    current = clock();
    diff = current - before;
    msec = ((double)diff)*1000 / CLOCKS_PER_SEC;
  }while(msec <= trig); // timer for amount of seconds (30 seconds)
  return 0;
}

// writer method to establish writers
void* writer(void*arg){
  double current = 0;
  clock_t diff = 0;
  do{

    int f = *(int*)arg;
    sem_wait (&q);
    sem_wait(&db);
    
    usleep(rand()% (1000-100+1)+100);
    sleepval = (rand()% (50-10+1)+10);
    sem_post (&q);
    printf("Writer %d writes %d\n", writerCount, sleepval);
    writerCount+=1;
    usleep(sleepval);
    sem_post(&db);
    
    current = clock();
    diff = current - before;
    msec = ((double)diff)*1000 / CLOCKS_PER_SEC;
  }while(msec <= trig); // timer for amount of seconds (30 seconds)
  
  return 0;
}
