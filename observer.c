// Author: Warren Wnuck
// CSC 139-02
// Assignment #1
// obverver.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#define BUF_SIZE 2048

struct timeval now;
enum TYPE{SHORT, LONG, STANDARD};

void getModelName();
void linuxKernelVer();
void lastBootTime();
void getTimeHost();
void timeSpent();
void standardSet();
void shortSet();
void diskRW();
void contextSwitchCount();
void timeLastBoot();
void processCount();
void totalMemory();
void freeMemory();
void averageLoad();
void averageLoadRep(int interval, int duration);


int main(int argc, char *argv[]){
	char repTypeName[16];
	char c1, c2;
	enum TYPE reportType;
	int interval, duration;
	
	reportType = STANDARD;
	strcpy(repTypeName, "Standard");

	if (argc > 1){
		sscanf(argv[1], "%c%c", &c1, &c2);
		if (c1 != '-'){
			fprintf(stderr, "usage: observer [-s][-1 int dur]\n");
			exit(1);
		}	
		if (c2 == 's'){
			reportType = SHORT;
			strcpy(repTypeName, "Short");
		}
		if (c2 == 'l'){
			reportType = LONG;
			strcpy(repTypeName, "Long");
			interval = atoi(argv[2]);
			duration = atoi(argv[3]);
		}
	}

	if (reportType == STANDARD){
		printf("\nReport Type STANDARD\n\n");
		standardSet();
	}
	if(reportType == SHORT){
		printf("\nReport Type: SHORT\n\n");
		standardSet();
		shortSet();				
	}
	if (reportType == LONG){
		printf("\nReport Type: LONG\n\n");
		standardSet();
		shortSet();
		totalMemory();
		freeMemory();
		averageLoadRep(interval, duration);
	}
	
	exit(0);
}

// All the methods for type STANDARD in one method
void standardSet(){
	getModelName();
	linuxKernelVer();
	lastBootTime();
	getTimeHost();
	
}

// All the methods for type SHORT in one method
void shortSet(){
	timeSpent();
	diskRW();
	contextSwitchCount();
	timeLastBoot();
	processCount();
}

// Recieves the model name and type from /proc then prints on system
void getModelName(){
	FILE *thisProcFile;
	char buf[BUF_SIZE];
	int i = 0;

	thisProcFile = fopen("/proc/cpuinfo", "r");
	for(i = 0; i < 5; i++){
		fgets(buf, BUF_SIZE+1, thisProcFile);
	}

	printf("%s", buf);
	fclose(thisProcFile);
}

// Recieves Linux's Kernel version from /proc then prints on system
void linuxKernelVer(){
	FILE *thisProcFile;
	char buf[BUF_SIZE];
	
	thisProcFile = fopen("/proc/version", "r");
	fgets(buf, BUF_SIZE+1, thisProcFile);
	printf("%s", buf);
	fclose(thisProcFile);
}

// Recieves the seconds from /proc then converts into days,hours,minutes, and seconds  
void lastBootTime(){
	FILE *thisProcFile;
	char buf[BUF_SIZE];
	float seconds;

	thisProcFile = fopen("/proc/uptime", "r");
	fscanf(thisProcFile, "%f", &seconds);
	
	float d = seconds / 86400;	
	int day = (int) d;		
	float h = (d - day) * 24;	
	int hour = (int) h;		
	float m = (h - hour) * 60;
	int min = (int) m;
	float s = (m - min) * 60;
	int sec = (int) s;

	printf("Last System Boot Time: %02d:%02d:%02d:%02d\n", day, hour, min, sec);
		
	fclose(thisProcFile);
}

// Recieves hostname and prints then uses gettimeofday for date report
void getTimeHost(){
	FILE *thisProcFile;
	char buf[BUF_SIZE];
	char repTypeName[16] = "Standard";
	int reportType;
	reportType = STANDARD;	

	thisProcFile = fopen("/proc/sys/kernel/hostname", "r");
	fgets(buf, BUF_SIZE+1, thisProcFile);
	printf("Machine Hostname: %s", buf);
	fclose(thisProcFile);

	gettimeofday(&now, NULL);
	printf("Status report type %s at %s", repTypeName,ctime(&(now.tv_sec)));
	
}

// Recieves the time spent in Modes User, System, and Idle then prints to system
void timeSpent(){
	FILE *thisProcFile;
	char buf[BUF_SIZE];
	char *l;
	long userTime, systemTime, fill;
	long long idleTime;	

	thisProcFile = fopen("/proc/stat", "r");
	fscanf(thisProcFile, "%s %lu %lu %lu %llu \n", &l, &userTime, &fill, &systemTime, &idleTime);
	printf("User Mode Time: %lu\nSystem Mode Time: %lu\nIdle Mode Time: %llu\n", userTime, systemTime, idleTime);
	fclose(thisProcFile);

}

// Recieves the values for Disk read/write from /proc/diskstats then prints
void diskRW(){
	FILE *thisProcFile;
	char buf[BUF_SIZE];
	int i = 0;
	
	thisProcFile = fopen("/proc/diskstats", "r");
	for (i = 0; i < 24; i++){
		fgets(buf, BUF_SIZE, thisProcFile);	
	}
	fgets(buf, BUF_SIZE, thisProcFile);
	
	for(i = 0; i < 6; i++){
		fscanf(thisProcFile, "%s", buf);
	}
	printf("Disk Read Requests: %s\n", buf);

	for (i = 0; i < 4; i++){
		fscanf(thisProcFile, "%s", buf);
	}
	printf("Disk Write Requests: %s\n", buf);

	fclose(thisProcFile);  	
}

// Recieves the value for the amount of context switches from /proc then prints
void contextSwitchCount(){
	FILE *thisProcFile;
	char buf[BUF_SIZE];
	int i = 0;

	thisProcFile = fopen("/proc/stat", "r");
	for (i = 0; i < 6; i++){
		fgets(buf, BUF_SIZE, thisProcFile);
	}
	
	fscanf(thisProcFile, "%s", buf);
	fscanf(thisProcFile, "%s", buf);
	printf("Context Switches: %s\n", buf);
	fclose(thisProcFile);
}

// Recieves the value for the last system boot time then prints
void timeLastBoot(){
	FILE *thisProcFile;
	char buf[BUF_SIZE];
	char buf1[BUF_SIZE];
	char *str = "who -b";
	int i = 0;
	
	thisProcFile = popen(str, "r");
	
	for (i = 0; i < 3; i++){
		fscanf(thisProcFile, "%s", buf);
	}
	fscanf(thisProcFile, "%s", buf1);
	
	printf("Last System Reboot Time: %s %s\n", buf, buf1);
	fclose(thisProcFile);	
}

// Recieves the number of processes after last boot from /proc then prints 
void processCount(){
	FILE *thisProcFile;
	char buf[BUF_SIZE];
	int i = 0;
	
	thisProcFile = fopen("/proc/stat", "r");
	
	for (i = 0; i < 8; i++){
		fgets(buf, BUF_SIZE, thisProcFile);
	}

	fscanf(thisProcFile, "%s", buf);
	fscanf(thisProcFile, "%s", buf);
	printf("Process After Last Boot: %s\n", buf);

	fclose(thisProcFile);
}

// Recieves the amount of total memory from /proc then prints
void totalMemory(){
	FILE *thisProcFile;
	char buf[BUF_SIZE];

	thisProcFile = fopen("/proc/meminfo", "r");

	fgets(buf, BUF_SIZE+1, thisProcFile);
	
	printf("%s", buf);
	fclose(thisProcFile);
}

// Recieves the amount of free memory from /proc then prints
void freeMemory(){
	FILE *thisProcFile;
	char buf[BUF_SIZE];

	thisProcFile = fopen("/proc/meminfo", "r");

	fgets(buf, BUF_SIZE+1, thisProcFile);
	fgets(buf, BUF_SIZE+1, thisProcFile);

	printf("%s", buf);
	fclose(thisProcFile);
}

// Recieves the average load from /proc then prints
void averageLoad(){
	FILE *thisProcFile;
	char buf[BUF_SIZE];
	float avg;

	thisProcFile = fopen("/proc/loadavg", "r");

	fscanf(thisProcFile, "%f\n", &avg);

	printf("Load Average: %0.2f\n", avg);
	fclose(thisProcFile);

}

// Takes an int named interval for how often the average load is read to be read 
// and an int named duration for the amount of time it should be read
// This method waits for a certain amount of time(interval) and repeats averageLoad()
// for a certain amount of time(duration) 
void averageLoadRep(int interval, int duration){
	int i = 0;

	while (i < duration){
		sleep(interval);
		averageLoad();
		i += interval;
	}

}
