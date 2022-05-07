#ifndef HEADER_FILE
#define HEADER_FILE
 

struct element {
	int type; //Machine type
	int time; //Using time
};

typedef struct queue {
	// Define the struct yourself
	int size; //size of the queue
    int head; // head of the queue
    int tail; //tail of the queue
    int capacity; 
    struct element *array; // vector that forms the queue
}queue;

queue*         queue_init (int size);
int            queue_destroy (queue *q);
int            queue_put (queue *q, struct element* elem);
struct element *queue_get (queue *q);
int            queue_empty (queue *q);
int            queue_full   (queue *q);

#endif
