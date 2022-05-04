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
struct element *file_info;
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

        type_time.type = file_info[initial].type;
        type_time.time = file_info[initial].time;
	
	
        /*initialize the mutex*/
        if (pthread_mutex_lock(&mutex)<0){
        perror("Error lock mutex");
        exit(-1); 
        }
        /* CRITICAL SECTION */
        /* We check if the circular buffer is full and if it is, we block the 
        thread producer  with cond_wait so that it doesn´t add more elemetns to the buffer*/
        while (queue_full(circularbuffer)==1){  	
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
	//printf("EL elemento que se añade es %d\n", type_time.type);
	//printf("EL elemento que se añade es %d\n", type_time.time);
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
	int *result;
	result=malloc(sizeof(int));
    struct element consumer_operands;
    for (i=0; i< num_operands; i++) {

    /*Obtain the elements inserted in the queue and returns the partial cost calculated one by one*/
        if (pthread_mutex_lock(&mutex)<0){
        perror("Error lock mutex");
        exit(-1); 
        }
        //CONDITION
        while (queue_empty(circularbuffer)==1){
            if(pthread_cond_wait(&non_empty, &mutex) < 0){ //no_empty is now suspended
        			perror("Error cond wait no lleno");
        			exit(-1);
      			}  
        }

        /* CRITICAL SECTION */
        printf("HOLA");
        //variables for the operation
        struct element *consumer_operands = queue_get(circularbuffer);
        if (consumer_operands->time <0){
			printf("Error in time format\n");
        }
        else if (consumer_operands->type==1){
			*result=*result+((consumer_operands->time)*3);
		}
		else if (consumer_operands->type==2){
			*result=*result+((consumer_operands->time)*6);
		}
		else if (consumer_operands->type==3){
			*result=*result+((consumer_operands->time)*15);
		}
		else{
			printf("Error in type format\n");
			//we dont exit cause we need to keep processing data
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
    free(result);
    pthread_exit((void *)result);
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

   
    file_info =malloc(num_operands * sizeof(struct element)); // reserve space in memory (TYPE and TIME are stored) 
    int dummy_var;
    struct element element_1[num_operands];
    //printf("The size used for the file_info is %ld \n", sizeof(file_info));
    int i=0;
    while (i < num_operands+1) {
        fscanf(output, "%d %d %d", &dummy_var, &element_1[i].type, &element_1[i].time);
	
        //esa estructura asignarla a file_info cero
	file_info[i]=element_1[i];
	printf("%d, %d\n", file_info[i].type, file_info[i].time );
	i = i+1;
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
    pthread_t consumer[numConsumers]; 
 
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
	//printf("EMPIEZO EN EL ID %d\n",id); 
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
        if(pthread_create(&consumer[e], NULL, (void *)consumers, &num_operands) < 0){
        perror("Error creating thread");
        return -1;
        }
	e = e+1;
}

    printf("\nESTAS AQUÍ\n");
    int *result_main;
    result_main = malloc(sizeof(int));
	//we wait for all the threads to finish 
	
	for (int i = 0; i < numConsumers; i++) {
    		if(pthread_join(consumer[i], (void **)&result_main) < 0){
      		perror("Error waiting for producer thread");
      		return -1;
    		}
    }

	result_main=(int *)result_main;
	for (int i = 0; i < numProducers; i++) {
    		if(pthread_join(producer[i], NULL) < 0){
      		perror("Error waiting for producer thread");
      		return -1;
    		}
  	}





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
    free(result_main);



    printf("Total: %i €.\n", *result_main);
    return 0;

}
