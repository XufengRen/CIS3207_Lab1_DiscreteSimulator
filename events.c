#include <stdio.h>
#include <stdlib.h>

typedef struct event{
	int time;	//time
	int jobNum;
	int type;
	int atDisk; //Specifying at which disk the job finished
}event;

event createEvent(int time, int jobNo, int type){
	event job;
	job.time = time;
	job.jobNum = jobNo;
	job.type = type;
	return job;
};

event xcreateEvent(int time, int jobNo, int type, int at){
	event job;
	job.time = time;
	job.jobNum = jobNo;
	job.type = type;
	job.atDisk = at;
	return job;
};



