#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <pthread.h>
#include "queue.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "queue.h"


/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */

//we init the mutex and the condition variables associated to the mutex
pthread_mutex_t mutex; /*mutex to access shared buffer */
pthread_cond_t non_full; /* check if we can add more elements*/
pthread_cond_t non_empty; /* check if we can remove more elements*/
const char *fileName;
int *file_info;
struct queue *circularbuffer; //declaration of the circular buffer


//create a structure that contains the initial id and the final id
struct param_producer{
    int init_id;
    int final_id;
};

/**** Producer Thread ****/

void *producers(struct param_producer *argv) {
    /*The purpose of the producer function is to obtain data extracted from the file 
    and insert them in the circular queue*/
    //Reciever thread
    struct element type_time;
    //
    int initial = argv->init_id;
    int j=0;
    while (initial<argv->final_id){ 
	j=initial; /*adjust the index */
	j=j*2;
        type_time.type = file_info[j];
        type_time.time = file_info[j+1];
	
	
        /*initialize the mutex*/
        if (pthread_mutex_lock(&mutex)<0){
        perror("Error lock mutex");
        exit(-1); 
        }
        /* CRITICAL SECTION */
        /* We check if the circular buffer is full and if it is, we block the 
        thread producer  with cond_wait so that it doesn´t add more elemetns to the buffer*/
        while (queue_full(circularbuffer)==-1){  	
			if(pthread_cond_wait(&non_full, &mutex) < 0){ //no_empty is now suspended
        			perror("Error cond wait no lleno");
        			exit(-1);
    
      			}       
		}//while

        /*Insert the elements in the circular buffer*/
        if(queue_put(circularbuffer, &type_time) < 0){
        perror("Error queue put");
        exit(-1);
        }
	printf("EL elemento que se añade es %d\n", type_time.type);
	printf("EL elemento que se añade es %d\n", type_time.time);
        /* END CRITICAL SECTION */

        /*We unlock the thread producer suspended in the conditional variable
        no_emtpy and the mutex is ready to be 
        acquired again*/
        if (pthread_cond_signal(&non_empty)<0){ 
			perror("Error cond signal no vacio");
        		exit(-1);		
		}

        /*We unlock the mutex*/
		if (pthread_mutex_unlock(&mutex)<0){
			perror("Error unlock mutex");
        		exit(-1);		
		}
    initial = initial + 1;
    }
    pthread_exit(0);

}

/**** Producer Thread ****/

void *consumers(int num_operands) {
    int i;
    int result = 0; //result of the process
    for (i=0; i<num_operands; i++) {
    /*Obtain the elements inserted in the queue and returns the partial cost calculated one by one*/
        if (pthread_mutex_lock(&mutex)<0){
        perror("Error lock mutex");
        exit(-1); 
        }
        //CONDITION
        while (queue_empty(circularbuffer)==-1){
            if(pthread_cond_wait(&non_empty, &mutex) < 0){ //no_empty is now suspended
        			perror("Error cond wait no lleno");
        			exit(-1);
      			}  
        }

        /* CRITICAL SECTION */
        
        //variables for the operation
        int type_variable = circularbuffer[i].array->type;
        int time_variable = circularbuffer[i].array->time;
        int mult = 0;

        mult = type_variable * time_variable;
        result = result + mult;
        
        
        if(queue_get(circularbuffer) < 0){
        perror("Error queue dequeue");
        exit(-1);
        }
                

        /*We unlock the thread producer suspended in the conditional variable
        no_full and the mutex is ready to be 
        acquired again*/
        if (pthread_cond_signal(&non_full)<0){ 
			perror("Error cond signal no vacio");
        		exit(-1);		
		}
        /* Unlock the mutex */
        if (pthread_mutex_unlock(&mutex)<0){
			perror("Error unlock mutex");
        		exit(-1);		
		}
    }
    pthread_exit(0);
}


int main (int argc, const char * argv[] ) {
    //Validate that the number of inputa ia correct
    if (argc < 4) {
        perror("Not enough arguments");
        return -1;
    }
    //Variable definition
    int BUFFSIZE; // buff_size, indicates the size of the circular queue
    fileName = argv[1]; // file descriptor of the input file
    int numProducers = 0, numConsumers = 0; //num_producers, num_consumers
    //int buffer[BUFFSIZE]; //buffer
    int num_operands; // where the output is stored
    int d0;
    int d1;
    int d2;

    //Read the input arugments

    //check the validity of the producers
    if ((atoi(argv[2])<=0 )){
        perror("Invalid number of producers");
	return -1;
    }
    //check the validity of the consumers
    if ((atoi(argv[3])<=0 )){
        perror("Invalid number of consumers");
	return -1;
    }
    //check the validity of the buffersize
    if ((atoi(argv[4])<=0 )){
        perror("Invalid buffer size");
	return -1;
    }
//Once the arguments have been checked, we assign the values to its variables
	numProducers = atoi(argv[2]);
	numConsumers = atoi(argv[3]);
	BUFFSIZE = atoi(argv[4]);
	printf("%d\n %d\n %d\n", BUFFSIZE, numProducers, numConsumers);
    //The fopen function opens the file whose name is the string pointed to by pathname and associates a stream with it
    FILE * output = fopen(fileName, "r");
    if (NULL == output) {
        printf("fopen: error\n");
        exit(-1);
    }

    //Store the number of operands in num_operands(first number of the file)
    if (fscanf(output, "%d", &num_operands) < 0){
        perror("Error while executing fscanf");
    }

   file_info = (int *)malloc(num_operands * sizeof(int)); // reserve space in memory (TYPE and TIME are stored)
	printf("The size used for the file_info is %ld \n", sizeof(file_info));
    int i=0;
    while (i < num_operands+1) {
        fscanf(output, "%d %d %d", &d0, &d1, &d2);
	//printf("%d %d %d\n", d0, d1, d2);
        file_info[i]= d1;
        file_info[i+1] = d2;
	
	i = i+2;
    }

    if (fclose(output)<0){
	    perror("Error closing desc");
	    return -1;
	}

   //Initialize the mutex
	if (pthread_mutex_init(&mutex, NULL)<0){
		perror("init mutex error");
    		return -1;
	}
	if (pthread_cond_init(&non_full, NULL)<0){
		perror("init cond error");
    		return -1;
	}
	if (pthread_cond_init(&non_empty, NULL)<0){
		perror("init cond error");
    		return -1;
	}

    /*create an array of param_producers that contains as many 
    structures as the num producers obtained in the input*/
    struct param_producer array_producer[numProducers];
    pthread_t producer[numProducers]; //thread for the producers
    pthread_t consumer; 
 
    /*In order to know how many operations are made by the producer we divide the
    num of operations (500)/numProducer*/
    /*we check with remainder if the number is decimal with %, in which case the result 
    of the division is increased*/

    int num_operations_producer = num_operands / numProducers;
    printf("number of op is %d \nthe num of prod is %d\nnext %d\n", num_operands, numProducers, num_operations_producer);
    int remainder = num_operands % numProducers;
	printf("the remainder is %d \n", remainder);
    if (remainder !=0){
        num_operations_producer = num_operations_producer + 1;
    }

    /* Calculate the number of operations that each consumer wants to execute */

    
    /* Initialize the circular buffer (queue) */
    circularbuffer = queue_init(BUFFSIZE);
printf("EStoy aqui\n");
    /* Consumer- Producer*/
int a=0;
int id=0;
    while (a < numProducers) {
	printf("EMPIEZO EN EL ID %d\n",id); 
        array_producer[a].init_id = id;
        array_producer[a].final_id = id + num_operations_producer;
        id =  id + num_operations_producer;

        /* Producer call */
        if(pthread_create(&producer[a], NULL, (void *)producers, &array_producer[a]) < 0){
        perror("Error creating thread");
        return -1;
	}

	a = a+1;

    }
	int e = 0;
	while (e < numConsumers){
        /* Consumer call */
        if(pthread_create(&consumer, NULL, (void *)consumers, &num_operands) < 0){
        perror("Error creating thread");
        return -1;
        }
	e = e+1;
}
    pthread_join(*producer, NULL);
    pthread_join(consumer, NULL);

    /* DESTROY THE MUTEX */
    
    if (pthread_mutex_destroy(&mutex)<0){
		perror("destroy mutex error");	
    		return -1;
	}
	if (pthread_cond_destroy(&non_full)<0){
		perror("Destroy the non full cond");	
    		return -1;
	}
	if (pthread_cond_destroy(&non_empty)<0){
		perror("destroy the non empty cond");	
    		return -1;
	}
    //Destroy the circular buffer
    queue_destroy(circularbuffer);
    free(file_info);
    printf("The total number of consumers is \n");
    return 0;

}
