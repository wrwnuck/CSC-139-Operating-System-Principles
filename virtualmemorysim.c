// By: Warren Wnuck
//
// CSC 139 - 02
// virtualmemorysim.c

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 

struct Frame{
    int last;
    int next;
    int value;
};

void fifo(int pages, int frames, int requests, struct Frame *list);
void lru(int pages, int frames, int requests, struct Frame *list);
void opt(int pages, int frames, int requests, struct Frame *list);
int fileExists(const char * filename);

int main(int argc, char const *argv[]){
    int i = 0;
    int requests;
    int pages;
    int flag;
    int frames;
    int request_page;
    char schedule_alg[5];
    

    if(argc != 3){
        printf("ERROR INPUT: Incorrect Number of Arguments\n");
	printf("Example Input: ./proj4 [input file] [FIFO/LRU/OPT]\n");
        return 0;
    }

    sscanf(argv[2], "%s", schedule_alg);

    if(strcmp(schedule_alg, "FIFO") == 0){ 
        flag = 0;
    }else if(strcmp(schedule_alg, "LRU") == 0){ 
        flag = 1;
    }else if(strcmp(schedule_alg, "OPT") == 0){ 
        flag = 2;
    }else {
        printf("ERROR INPUT: Example Input: ./proj4 [input file] [FIFO/LRU/OPT]\n");
        return 0;
    }
    
    int exists = fileExists(argv[1]);
    if (exists == 0){
	printf("ERROR INPUT: File does not exist.\nCheck if file is in directory or its spelling\n");
	return 0;
   }

    FILE *input = fopen(argv[1], "r");
    fscanf(input, "%d %d %d", &pages, &frames, &requests);

    struct Frame list[requests];

    for(i = 0; fscanf(input, "%d", &request_page) > 0; i++){
        list[i].value = request_page;
        list[i].last = -2;
        list[i].next = -2; 
    }
    
    fclose(input);
    if(flag == 0){
	printf("(%s)\n",schedule_alg);
        fifo(pages, frames, requests, list);
    }else if(flag == 1){
	printf("    (%s)\n",schedule_alg);
        lru(pages, frames, requests, list);
    }else if(flag == 2){
	printf("    (%s)\n",schedule_alg);
        opt(pages, frames, requests, list);
    }

    return 0;
}

// fifo scheduling algorithm for virtual memory machine 
void fifo(int pages, int frames, int requests, struct Frame *list){
    int i;
    int j;
    int f_count = 0;
    int faults = 0;
    int found_flag;
    int cur_Req;

    int queueList[frames];

    for(i = 0; i < frames; i++){
	queueList[i] = 0;
    }

    for(i = 0; i < requests; i++){
 	
        found_flag = 0;
        cur_Req = list[i].value;

        for(j = 0; j < frames; j++){
            if(cur_Req == queueList[j]){
                printf("Page %d already in Frame %d\n", cur_Req, j);
                found_flag = 1;
	
            }
	    
        }

        if(found_flag == 0){
            if((queueList[f_count] > 0) && (queueList[f_count] <= pages)){
                 printf("Page %d unloaded from Frame %d, Page %d loaded into Frame %d\n",
                 queueList[f_count], f_count, cur_Req, f_count);
                 queueList[f_count] = cur_Req;
                 f_count++;
                 faults++;

                 if(f_count >= frames)
                    f_count = 0;
            }
            else {
		
                queueList[f_count] = cur_Req;
                printf("Page %d loaded into frame %d\n", cur_Req, f_count);
                 f_count++;
                 faults++;

                 if(f_count >= frames)
                    f_count = 0;
            }
	   
        }
    }
    printf("%d page faults\n", faults);
}

// lru scheduling algorithm for virtual memory machine 
void lru(int pages, int frames, int requests, struct Frame *list){
    int k = 0;
    int i;
    int j;
    int f_count = 0;
    int frameFull = 0;
    int faults = 0;
    int found_flag;
    int cur_Req;
    int cng_frame;
    int next_count;
    int next_found;

    struct Frame queueList[frames];
    
    for(i = 0; i < frames; i++){
	    queueList[i].value = 0;
    }

    for(i = 0; i < requests; i++){
        found_flag = 0;
        cur_Req = list[i].value;

        for(j = 0; j < frames; j++){
            if(cur_Req == queueList[j].value){
                printf("Page %d already in Frame %d\n", cur_Req, j);
                queueList[j].last = -1;
                found_flag = 1;
            }
        }
        if(found_flag == 0){
            if(frameFull == 1){
                f_count = 0;
                cng_frame = 0;

                for(f_count = 0; f_count < frames; f_count++){
                    if(queueList[f_count].last > queueList[cng_frame].last)
                        cng_frame = f_count;
                } 

                printf("Page %d unloaded from Frame %d, Page %d loaded into Frame %d\n",
                queueList[cng_frame].value, cng_frame, cur_Req, cng_frame);
                queueList[cng_frame].value = cur_Req;
                queueList[cng_frame].last = -1;
                faults++;
            }

            else {
                queueList[f_count].value = cur_Req;
                printf("Page %d loaded into Frame %d\n", cur_Req, f_count);
                queueList[f_count].last = -1;
                f_count++;
                faults++;
                if(f_count >= frames) 
                    frameFull = 1;
            }
        }
        for(j = 0; j < frames; j++){
            queueList[j].last++;
        } 
            
    }
    printf("%d page faults\n", faults);
}

// opt scheduling algorithm for virtual memory machine 
void opt(int pages, int frames, int requests, struct Frame *list){
    int i = 0;
    int j = 0;
    int k;
    int f_count;
    int faults;
    int frames_full;
    int cur_Req;
    int found_flag;
    int next_found;
    int next_count;
    int cng_frame;

    struct Frame queueList[frames];

    f_count = 0;
    faults = 0;
    frames_full = 0;

    for(i = 0; i < frames; i++){
	    queueList[i].value = 0;
    }

    for(i = 0; i <= (requests-1); i++){
        found_flag = 0;
        cur_Req = list[i].value;

        for (j = 0; j <= (frames-1); j++) {
            if(cur_Req == queueList[j].value){
                printf("Page %d already in frame %d\n", cur_Req, j);
                k = i + 1;
                next_count = 1;
                next_found = 0;
                for (k; k <= requests-1; k++){
                    if(queueList[f_count].value == list[k].value){
                        next_found = 1;
                        break;
                    }
                    next_count++;
                }

                if(next_found == 1){ 
                    next_count = -2;
                }

                queueList[f_count].next = next_count;
                found_flag = 1;
            }
        }

        if(found_flag == 0){
            if(frames_full == 1){
                f_count = 0;
                cng_frame = 0;

                for(f_count; f_count <= (frames-1); f_count++){
                    if(queueList[f_count].next > queueList[cng_frame].next){
                        cng_frame = f_count;
                    }
                }

                printf("Page %d unloaded from frame %d, page %d loaded into frame %d\n", 
                queueList[cng_frame].value, cng_frame, cur_Req, cng_frame);
                queueList[cng_frame].value = cur_Req;
                k = i + 1;
                next_count = 1;
                next_found = 0;

                for(k; k <= requests - 1; k++){
                    if(queueList[f_count].value == list[k].value){
                        next_found = 1;
                        break;
                    }
                    next_count++;
                }

                if(next_found == 1){
                    next_count = -2;
                }

                queueList[f_count].next = next_count;
                faults++;
            }else{
                queueList[f_count].value = cur_Req;
                printf("Page %d loaded into frame %d\n", cur_Req, f_count);
                k = i + 1;
                next_count = 1;
                next_found = 0;

                for(k; k <= requests-1; k++){
                    if(queueList[f_count].value == list[k].value){
                        next_found = 1;
                        break;
                    }
                    next_count++;
                }

                if(next_found == 1){
                    next_count = -2;
                }

                queueList[f_count].next = next_count;
                f_count++;
                faults++;

                if(f_count >= frames) {
                    frames_full = 1;
                }
                    
            }
        }
    }
    printf("%d page faults\n", faults);
}

// checks directory virtualmemorysim.c is in to see if input file
// exists returns 1 if it exists and 0 if the input does not exist
int fileExists(const char * filename){
    FILE *file;
    if (file = fopen(filename, "r")){
	fclose(file);
	return 1;
    }
    return 0;
}
