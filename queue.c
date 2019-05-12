#include <stdio.h>
#include <stdlib.h>
#include "events.c"


typedef struct node{
	struct event job;
	struct node *next;
}node;


void push(node** head, event data, int* size){
	node* new_node = (node*) malloc(sizeof(node));
	new_node->job = data;
	new_node->next = NULL;

	//start at beginning of list
	node *last = *head;
	(*size)++;

	if (*head == NULL){
		*head = new_node;
		return;
	}
	while (last->next != NULL)
		last = last->next;

	last->next = new_node;
	return;
};


event pop(node** head, int* size){
	node *temp = *head;
	*head = (*head)->next;
	event job = temp->job;
	free(temp);
	(*size)--;
	return job;
};



//PUSH for PRIORITY QUEUE
void e_push(node** head, event data, int* size){
	node* temp = (node*) malloc(sizeof(node));
	temp->job = data;
	temp->next = NULL;

	int priority = temp->job.time; //time of event will be the priorty

	printf("Job Time: %d, Job Num: %d\n",temp->job.time,temp->job.jobNum);


	(*size)++;
	//start at beginning of list
	node *last = *head;

	if (*head == NULL){

		*head = temp;
		return;

	} else if(priority < last->job.time){

		temp->next = last;
		*head = temp; // have the pointer to the linkedlist point to the very start of the node which would be temp
		return;

	}

	//traverse till we get to the end of list OR if we find the correct position where the node belongs
	while (last->next != NULL && priority >= last->next->job.time)

		last = last->next;

	temp->next = last->next;
	last->next = temp;
	return;
};
