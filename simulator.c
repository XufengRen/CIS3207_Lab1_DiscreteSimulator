#include <stdio.h>
#include <stdlib.h>
#include "config_reader.c"
#include "queue.c"

#define FILE_NAME "config.txt"
#define ARRIVAL 0
#define FINISH 1
#define DISK_ARRIVAL 2
#define DISK_FINISH 3
#define SIMULATION_END 4
#define IDLE 0
#define BUSY 1


//INITIALIZE INPUT
float QUIT_PROB = 0.2;      //Specified in Assignment 1 Manual to set to 0.2 for simplicity
int SEED = 0;
int INIT_TIME = 0;
int	FIN_TIME = 0;
int ARRIVE_MIN = 0;
int ARRIVE_MAX = 0;
int CPU_MIN = 0;
int CPU_MAX = 0;
int DISK1_MIN = 0;
int DISK1_MAX = 0;
int DISK2_MIN = 0;
int DISK2_MAX = 0;


//FUNCTION IN MAIN SIMULATOR
int randomBetween(int, int);
void process_CPU(event);
void process_DISK(event);
void recordInput();
void recordEvent(char*);
void ComputeQueueSize();
void Utilization_Response();
void Efficiency();



//INITIALIZE ARGUMENTS FOR DISCRETE SIMULATOR
int currentTime = 0, randTime = 0, ran = 0;
node* CPU = NULL, *DISK_1 = NULL, *DISK_2 = NULL; //3 FIFO QUEUES
node* eventQ = NULL; //1 Priority Queue

event task;

int cpu_size = 0, disk1_size = 0, disk2_size = 0, eventQ_size = 0, numJobs = 1;
int cpuState = IDLE, disk1State = IDLE, disk2State = IDLE;


//INITIALIZE SIMULATOR STATISTICS
int EQ_maxSize = 0, CPU_maxSize = 0, disk1_maxSize = 0, disk2_maxSize = 0;
long EQ_totalSize = 0, CPU_totalSize = 0, disk1_totalSize = 0, disk2_totalSize = 0;
int EQ_index = 0, CPU_index = 0, disk1_index = 0, disk2_index = 0;      //record the number of times total size of each queue is updated

int CPU_totalTime = 0, disk1_totalTime = 0, disk2_totalTime = 0;
int EQ_maxTime = 0, CPU_maxTime = 0, disk1_maxTime = 0, disk2_maxTime = 0;
int EQ_indexTime = 0, CPU_indexTime = 0, disk1_indexTime = 0, disk2_indexTime = 0;      //record the number of times total time of each queue is updated

int CPU_jobsCompleted = 0, disk1_jobsCompleted = 0, disk2_jobsCompleted = 0;



//MAIN
int main(){

    getInput(FILE_NAME);


	SEED = conf_vals[0];
	INIT_TIME = conf_vals[1];
	FIN_TIME = conf_vals[2];
	ARRIVE_MIN = conf_vals[3];
	ARRIVE_MAX = conf_vals[4];
	CPU_MIN = conf_vals[5];
	CPU_MAX = conf_vals[6];
	DISK1_MIN = conf_vals[7];
	DISK1_MAX = conf_vals[8];
	DISK2_MIN = conf_vals[9];
	DISK2_MAX = conf_vals[10];

    recordInput();

	srand(SEED);
	currentTime = INIT_TIME;


	//add two events to priorty queue, job1 arrival and simulation finished
	e_push(&eventQ,createEvent(INIT_TIME,1,ARRIVAL),&eventQ_size);
	e_push(&eventQ,createEvent(FIN_TIME,0,SIMULATION_END),&eventQ_size);

	//while queue is not empty

	while(eventQ_size != 0 && currentTime < FIN_TIME){
        EQ_totalSize = EQ_totalSize + eventQ_size;
        EQ_index++;

		//pop events from event queue and place them inside cpu queue

		task = pop(&eventQ,&eventQ_size);
		currentTime = task.time;

		//Event Handler

		if (task.jobNum == 0) {
            recordEvent("At time 3000, SIMULATION ENDED.");
		}

		switch(task.type){
			case ARRIVAL: process_CPU(task); break;
			case FINISH: process_CPU(task); break;
			case DISK_ARRIVAL: process_DISK(task); break;
			case DISK_FINISH: process_DISK(task); break;
			case SIMULATION_END: break;
		}
	}

    puts("");

	ComputeQueueSize();
	Utilization_Response();
	Efficiency();

	return 0;
}



int randomBetween(int min, int max){
	return (rand() % (max - min + 1)) + min;
}



void process_CPU(event task){
	char str[80];
	if (task.type == ARRIVAL) {
		sprintf(str, "At time %d, Job%d arrives.",currentTime,task.jobNum);
		recordEvent(str);

		randTime = randomBetween(ARRIVE_MIN,ARRIVE_MAX) + currentTime; numJobs++;

		e_push(&eventQ,createEvent(randTime,numJobs,ARRIVAL),&eventQ_size);
		if (eventQ_size > EQ_maxSize) {
            EQ_maxSize = eventQ_size;
		};

		push(&CPU,task,&cpu_size);  // job sent to the CPU
		if (cpu_size > CPU_maxSize) {
            CPU_maxSize = cpu_size;
		};


		if (cpuState == IDLE){
			task = pop(&CPU,&cpu_size);

			ran = randomBetween(CPU_MIN,CPU_MAX);
            CPU_totalTime = CPU_totalTime + ran;
			CPU_indexTime++;
			randTime = ran + currentTime;
			if (ran > CPU_maxTime) {
                CPU_maxTime = ran;
			}

			e_push(&eventQ,createEvent(randTime,task.jobNum,FINISH),&eventQ_size); // adds event to priorty queue about when cpu finishes
            if (eventQ_size > EQ_maxSize) {
                EQ_maxSize = eventQ_size;
            };
			cpuState = BUSY;
		}
	} else{
		sprintf(str, "At time %d, Job%d finishes at CPU.",currentTime,task.jobNum);
		recordEvent(str);
        CPU_jobsCompleted++;

		cpuState = IDLE;

		int quit = rand() <  QUIT_PROB * ((double)RAND_MAX + 1.0);

		if (quit){

			sprintf(str, "At time %d, Job%d exits.",currentTime,task.jobNum);
			recordEvent(str);
		} else {

			randTime = randomBetween(ARRIVE_MIN,ARRIVE_MAX) + currentTime;
			e_push(&eventQ,createEvent(randTime,task.jobNum,DISK_ARRIVAL),&eventQ_size);
            if (eventQ_size > EQ_maxSize) {
                EQ_maxSize = eventQ_size;
            };
		}
	}
	CPU_totalSize = CPU_totalSize + cpu_size;
	CPU_index++;
}



void process_DISK (event task){
	char str[80];
	int sendTo = 0;  // holder that determines what disk the event will be sent to

	if (task.type == DISK_ARRIVAL){

		sprintf(str, "At time %d, Job%d arrives at Disk.",currentTime,task.jobNum);
		recordEvent(str);

		if(disk1_size <= disk2_size){

			sendTo = 1;

		} else if(disk1_size > disk2_size){

			sendTo = 2;

		} else {

			sendTo = (rand() <  0.5 * ((double)RAND_MAX + 1.0)) + 1;
		}


		if (sendTo == 1){

			push(&DISK_1,task,&disk1_size);
            if (disk1_size > disk1_maxSize) {
                disk1_maxSize = disk1_size;
            };

			if(disk1State == IDLE){
				event job = pop(&DISK_1,&disk1_size);

                ran = randomBetween(DISK1_MIN,DISK1_MAX);
                disk1_totalTime = disk1_totalTime + ran;
                disk1_indexTime++;
                randTime = ran + currentTime;
                if (ran > disk1_maxTime) {
                    disk1_maxTime = ran;
                }


				e_push(&eventQ,xcreateEvent(randTime,job.jobNum,DISK_FINISH,1),&eventQ_size);

                if (eventQ_size > EQ_maxSize) {
                    EQ_maxSize = eventQ_size;
                };

				disk1State = BUSY;

			}

		} else if (sendTo == 2){
			push(&DISK_2,task,&disk2_size);

            if (disk2_size > disk2_maxSize) {
                disk2_maxSize = disk2_size;
            };


			if(disk2State == IDLE){

				event job = pop(&DISK_2,&disk2_size);


                ran = randomBetween(DISK2_MIN,DISK2_MAX);
                disk2_totalTime = disk2_totalTime + ran;
                disk2_indexTime++;
                randTime = ran + currentTime;
                if (ran > disk2_maxTime) {
                    disk2_maxTime = ran;
                }


				e_push(&eventQ,xcreateEvent(randTime,job.jobNum,DISK_FINISH,2),&eventQ_size);

                if (eventQ_size > EQ_maxSize) {
                    EQ_maxSize = eventQ_size;
                };

				disk2State = BUSY;

			}
		}
	} else {

		randTime = randomBetween(ARRIVE_MIN,ARRIVE_MAX) + currentTime;
		e_push(&eventQ,createEvent(randTime,task.jobNum,ARRIVAL),&eventQ_size);

        if (eventQ_size > EQ_maxSize) {
            EQ_maxSize = eventQ_size;
        };


		if (task.atDisk == 1){
			sprintf(str, "At time %d, Job%d finished I/O at Disk 1.",currentTime,task.jobNum);
			recordEvent(str);
            disk1_jobsCompleted++;
			disk1State = IDLE;

		} else if (task.atDisk == 2){
			sprintf(str, "At time %d, Job%d finished I/O at Disk 2.",currentTime,task.jobNum);
			recordEvent(str);
            disk2_jobsCompleted++;
			disk1State = IDLE;
		}



	}

	disk1_totalSize = disk1_totalSize + disk1_size;
	disk2_totalSize = disk2_totalSize + disk2_size;
	disk1_index++;
	disk2_index++;
}


void recordEvent(char *line){
	FILE * fp;

	/* open the file for writing*/
	fp = fopen ("log.txt","a+");
	fprintf (fp,"%s\n",line);

   /* close the file*/
   fclose (fp);
}

void recordInput() {
    FILE * fp;
    fp = fopen ("log.txt","a+");

    fprintf(fp, "INPUT: \n");
    fprintf(fp,"PROB_QUIT          0.2\n");

    for (int i=1; i<NUM_PARAMS; i++) {
        fprintf(fp, "%s         %d \n", conf_types[i], conf_vals[i]);
    }

    fprintf(fp,"\n");
    fclose (fp);
};

void ComputeQueueSize() {
    puts("MAX SIZE AND AVERAGE SIZE OF EACH QUEUE: ");
    printf("\nEvent Queue - Max Size: %d         Average Size: %d ",EQ_maxSize,(int)(EQ_totalSize/EQ_index));
    printf("\nCPU Queue  - Max Size: %d         Average Size: %d ",CPU_maxSize,(int)(CPU_totalSize/CPU_index));
    printf("\nDisk 1 Queue - Max Size: %d         Average Size: %d ",disk1_maxSize,(int)(disk1_totalSize/disk1_index));
    printf("\nDisk 2 Queue - Max Size: %d         Average Size: %d ",disk2_maxSize,(int)(disk2_totalSize/disk2_index));
    puts("\n");
};

void Utilization_Response() {
    puts("UTILIZATION OF EACH SERVER: ");
    printf("\nUtilization of CPU: %.4f",(CPU_totalTime/(float)(FIN_TIME-INIT_TIME)));
    printf("\nUtilization of Disk 1: %.4f",(disk1_totalTime)/(float)(FIN_TIME-INIT_TIME));
    printf("\nUtilization of Disk 2: %.4f",(disk2_totalTime)/(float)(FIN_TIME-INIT_TIME));
    puts("\n");

    puts("MAX AND AVERAGE RESPONSE OF EACH SERVER: ");
    printf("\nCPU - Max Response Time: %d         Average Response Time: %d ",CPU_maxTime,(int)(CPU_totalTime/CPU_indexTime));
    printf("\nDisk 1 - Max Response Time: %d         Average Response Time: %d ",disk1_maxTime,(int)(disk1_totalTime/disk1_indexTime));
    printf("\nDisk 2 - Max Response Time: %d         Average Response Time: %d ",disk2_maxTime,(int)(disk2_totalTime/disk2_indexTime));
    puts("\n");
};

void Efficiency() {
    puts("THROUGHPUT (JOBS COMPLETED PER UNIT OF TIME) OF EACH SERVER: ");
    printf("\nEfficiency (throughput) of CPU: %.4f per unit of time ",(CPU_jobsCompleted/(float)(FIN_TIME-INIT_TIME)));
    printf("\nEfficiency (throughput) of Disk 1: %.4f per unit of time ",(disk1_jobsCompleted/(float)(FIN_TIME-INIT_TIME)));
    printf("\nEfficiency (throughput) of Disk 2: %.4f per unit of time ",(disk2_jobsCompleted/(float)(FIN_TIME-INIT_TIME)));
    puts("");
};

