#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

//typedef unsigned long size_t; // definir una variable como tipo de datos
//typedef struct { fields } type; //C
//type z;
//struct type z; //C++

typedef struct{
	size_t numberOfThreads;
	//double** user_data; no se utiliza para este programa
} shared_data_t;

typedef struct{
	size_t threadCounter; // si se incializa aca, sería C++
	shared_data_t* shared_data;
} private_data_t;

void* run(void* data){
	private_data_t * private_data = (private_data_t*)data;
 	shared_data_t* shared_data = private_data->shared_data;
 	
 	size_t threadCounter = private_data->threadCounter;
 	size_t numberOfThreads = shared_data->numberOfThreads; 
 	// size_t threadCounter = (*private_data).threadCounter es lo mismo que ->
 	printf("Hello world from thread %zu of %zu\n", threadCounter, numberOfThreads);
	return NULL;
}

int main(int argc, char* argv[]){
	
	size_t numberOfThreads;
	
	if(argc>1){
		numberOfThreads = atoi(argv[1]);
	}
	else{
		numberOfThreads = sysconf(_SC_NPROCESSORS_ONLN);
	}
	
	pthread_t* thread_v = malloc(numberOfThreads * sizeof(pthread_t));
	
	if(thread_v == NULL){
		return (void)fprintf(stderr, "ERROR: could not allocate memory for %zu threads\n", numberOfThreads) , 1; // MULTIPLES RETURNS
		//exit(2) MUY AGRESIVO			
	}
	
	shared_data_t* shared_data = calloc(1, sizeof(shared_data_t));
	
	if(shared_data == NULL){
		return (void)fprintf(stderr, "ERROR: could not allocate shared memory for %zu threads\n", numberOfThreads) , 2;
	}
	
	shared_data-> numberOfThreads = numberOfThreads;
	
	private_data_t* private_data = calloc(numberOfThreads, sizeof(private_data_t));
	
	if(private_data == NULL){
		return (void)fprintf(stderr, "ERROR: could not allocate private memory for %zu threads\n", numberOfThreads) , 3;
	}
	
	struct timespec start_time;
	clock_gettime(CLOCK_MONOTONIC, &start_time);
	
				
	for(size_t index=0; index<numberOfThreads ;++index){																																																																													
		private_data[index].threadCounter = index;
		private_data[index].shared_data = shared_data;
		pthread_create(&thread_v[index], NULL, run, &private_data[index]);
	}
	
	printf("Hello world from main thread\n");
	
	for(size_t index=0; index<numberOfThreads ;++index){
		pthread_join(thread_v[index], NULL);
	}
	
	
	struct timespec finish_time;
	clock_gettime(CLOCK_MONOTONIC, &finish_time);
	
	//struct timespec{
	//		time_t tv_sec;
	//		long tv_nsec;
	//}
	
	double elapsed_seconds = finish_time.tv_sec - start_time.tv_sec	+ 1e-9 * (finish_time.tv_nsec - start_time.tv_nsec);
	
	printf("Hello execution time %.9lf s\n", elapsed_seconds);
	
	free(private_data);
	free(shared_data);
	free(thread_v);
	
	return 0;
}

//Todo proceso tiene varias partes:
//Data segment
//Heap segment
//Stack segment
//Código
//Thread: conjunto de valores con registros
//main thread: program counter.... -> todo thread tiene esta parte
//valgrind ./hello_i_of_w ELNUMEROQUESEA
