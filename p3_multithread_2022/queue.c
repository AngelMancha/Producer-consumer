//QUEUE C

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"



//To create a queue
queue* queue_init(int size){
	//Initialize the elements of the queue (initially all are 0)
	
	queue * q = (queue *)malloc(sizeof(queue)); //q es una variable de tipo puntero // reservar memoria con el tamaño de la queue
	q->size = size; //size of the queue
	q->head = 0;
	q->tail = 0; // q->tail = sizeof(queue) - 1;
	q -> capacity = 0; //number of elements in the queue
	q -> array = malloc(q->size * sizeof(struct element)); // we consider the size of the element (time and type of machine)
	return q;
}


// To Enqueue an element
int queue_put(queue *q, struct element* x) {
	//In case the queue is full, there is no need to append more elements
	if (queue_full(q)) {
		return -1;
	}

	q->array[q-> head] = *x; //the head is the new element to be inserted
	q-> head = q-> head +1; // increment by one the size of the array
	q->capacity = q->capacity + 1; // the number of elements is increased by one
	return 0;
}


// To Dequeue an element.
struct element* queue_get(queue *q) {
	struct element* element;
	
	return element;
}

//To check queue state
int queue_empty(queue *q){
	if (q->size != 0) {
		printf("Queue not empty");
		return -1; // return -1 in case the queue is not empty
	}
	return 0;
}

int queue_full(queue *q){
	// si size = sizeof(queue) q->size == q.size
	if (q->size == q -> capacity ){
		printf("Queue is full");
		return -1; // return -1 in case the queue is full
	}
	return 0;
	
	
}

//To destroy the queue and free the resources
int queue_destroy(queue *q){
	//free the memory space we had reserved with the malloc
	free(q->array);
	free(q);
	return 0;
}
