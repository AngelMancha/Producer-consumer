
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

void *producer(void * param) {
    /*The purpose of the producer function is to obtain data extracted from the file 
    and insert them in the circular queue*/
    //Reciever thread
}

void *consumer(void * param) {
    /*Obtain the elements inserted in the queue and returns the partial cost calculated one by one*/
}

int main (int argc, const char * argv[] ) {
    //Validate that the number of inputa ia correct
    if (argc < 4) {
        perror("Not enough arguments");
        return -1;
    }
    //Variable definition
    int BUFFSIZE = *argv[4]; // buff_size, indicates the size of the circular queue
    const char *fileName = argv[1]; // file descriptor of the input file
    int numProducers = 0, numConsumers = 0; //num_producers, num_consumers
    int buffer[BUFFSIZE]; //buffer
    int num_operands; // where the output is stored
    int d0;
    int d1;
    int d2;

    //Read the input arugments

    //check the validity of the producers
    if ((atoi(argv[2])<=0 )){
        printf("Invalid number of producers");
    }
    //check the validity of the consumers
    if ((atoi(argv[3])<=0 )){
        printf("Invalid number of consumers");
    }
    //check the validity of the buffersize
    if ((atoi(argv[4])<=0 )){
        printf("Invalid buffer size");
    }

    //The fopen function opens the file whose name is the string pointed to by pathname and associates a stream with it
    FILE * output = fopen(fileName, "r");
    if (NULL == output) {
        printf("fopen: error\n");
        exit(-1);
    }
    //Store the number of operands (first number of the file)
    if (fscanf(output, "%d", &num_operands) < 0){
        perror("Error while executing fscanf");
    }
    int *file_info = (int *)malloc(num_operands * sizeof(int)); // reserve space in memory (TYPE and TIME are stored)
	printf("The size used for the file_info is %ld \n", sizeof(file_info));
    // 
    int i=0;
    while (i < num_operands+1) {
        fscanf(output, "%d %d %d", &d0, &d1, &d2);
	//printf("%d %d %d\n", d0, d1, d2);
        file_info[i]= d1;
        file_info[i+1] = d2;
	
	i = i+1;
    }
	printf("%d \n", num_operands);

    
    
    free(file_info);
    return 0;
}

