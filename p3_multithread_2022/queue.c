#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"

 

//To create a queue
queue* queue_init(int size){
	//Initialize the elements of the queue (initially all are 0)
	queue * q = (queue *)malloc(sizeof(queue)); //q es una variable de tipo puntero // reservar memoria con el tamaÃ±o de la queue
	q->size = size; //size of the queue
	q->head = 0;
	q->tail = 0; //sizeof(q) - 1;
	q -> capacity = 0; //number of elements in the queue
	q -> array = malloc(q->size * sizeof(struct element)); // we consider the size of the element (time and type of machine)
	return q;
}


// To Enqueue an element
int queue_put(queue *q, struct element* x) {
	int ret ;
        ret = q->tail+1;

	//In case the queue is full, there is no need to append more elements
        if (ret >= q-> size) {
          ret = 0;
        }
        if ((q->capacity) == q->size) {
          return 1; 
        }
        q->capacity = q->capacity +1;// the number of elements is increased by one
        q->array[q-> tail] = *x; //we enqueue a new element at the tail in the queue (FIFO)
        q->tail = ret;
	return 0;
}


// To Dequeue an element.
/*
try1
struct element* queue_get(queue *q) {
	//In case the queue is full, there is no need to extractelements
        int ret ;
	ret = queue_empty(q);
        if (1 == ret)
            return -1;
	struct element* element;
	element = &(q -> array[q->head]); // element = q.array[tail] we deque the first element in the queue (FIFO)
	q-> head = q->head + 1; //we increase by one the head
	q->capacity = q->capacity -1; // the number of elements is decreased by one
	return element;
}
try2
struct element* queue_get(queue *q) {
        int ret ;
	struct element element;
	ret = queue_empty(q);
        if (1 == ret)
            return -1;
	element = (q -> array[q->head]); // element = q.array[tail] we deque the first element in the queue (FIFO)
	q-> head = q->head + 1; //we increase by one the head
	q->capacity = q->capacity -1; // the number of elements is decreased by one
	return &element;
}
*/

/*
struct element queue_get(queue *q) {
        int ret ;
	struct element element;
	ret = queue_empty(q);
        if (1 == ret)
          return element;
	memcpy(&element, &(q -> array[q->head]), sizeof(struct element));
	//element = (q -> array[q->head]); // element = q.array[tail] we deque the first element in the queue (FIFO)
printf("\n\n EStamos en el queue.c y sacamos:%d\n", element.type);
	q-> head = q->head + 1; //we increase by one the head
	q->capacity = q->capacity -1; // the number of elements is decreased by one
	return element;
}*/






struct element *queue_get(queue *q) {
      struct element *element;
      if (queue_empty(q) == 0) {
            element = &(q->array[q->head]);
        if (q->head+1 == q->size){
            q->head=0;
        }
        else{
            q->head = q->head+1;
        }
            q->capacity = q->capacity - 1;
      }

      return element;
}







//To check queue state
int queue_empty(queue *q){
	if (q->capacity == 0) {
		return 1; // return -1 in case the queue is empty
	}
	
	return 0;
	
	
}

int queue_full(queue *q){
	// si size = sizeof(queue) q->size == q.size
	if (q->size == q -> capacity ){
		return 1; // return 1 in case the queue is full
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
