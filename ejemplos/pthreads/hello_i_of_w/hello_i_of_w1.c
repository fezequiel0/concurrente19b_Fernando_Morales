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

int create_threads(shared_data_t* shared_data);
void* run(void* data);
	
int main(int argc, char* argv[]){
	
	shared_data_t* shared_data = calloc(1, sizeof(shared_data_t));
	
	if(shared_data == NULL){
		return (void)fprintf(stderr, "ERROR: could not allocate shared memory\n") , 2;
	}
	
	if(argc>1){
		shared_data-> numberOfThreads = atoi(argv[1]);
	}
	else{
		shared_data-> numberOfThreads = sysconf(_SC_NPROCESSORS_ONLN);
	}
	
	struct timespec start_time;
	clock_gettime(CLOCK_MONOTONIC, &start_time);
	
	int error = create_threads(shared_data);
	if(error){
			return error;
	}
	
	struct timespec finish_time;
	clock_gettime(CLOCK_MONOTONIC, &finish_time);
	
	//struct timespec{
	//		time_t tv_sec;
	//		long tv_nsec;
	//}
	
	double elapsed_seconds = finish_time.tv_sec - start_time.tv_sec	+ 1e-9 * (finish_time.tv_nsec - start_time.tv_nsec);
	
	printf("Hello execution time %.9lf s\n", elapsed_seconds);
	
	free(shared_data);
			
	return 0;
}

int create_threads(shared_data_t* shared_data){
	pthread_t* thread_v = malloc(shared_data->numberOfThreads * sizeof(pthread_t));
	
	if(thread_v == NULL){
		return (void)fprintf(stderr, "ERROR: could not allocate memory for %zu threads\n", shared_data-> numberOfThreads) , 1; // MULTIPLES RETURNS
		//exit(2) MUY AGRESIVO			
	}
	
	private_data_t* private_data = calloc(shared_data-> numberOfThreads, sizeof(private_data_t));
	
	if(private_data == NULL){
		return (void)fprintf(stderr, "ERROR: could not allocate private memory for %zu threads\n", shared_data->numberOfThreads) , 3;
	}
				
	for(size_t index=0; index<shared_data->numberOfThreads ;++index){																																																																													
		private_data[index].threadCounter = index;
		private_data[index].shared_data = shared_data;
		pthread_create(&thread_v[index], NULL, run, &private_data[index]);
	}
	
	printf("Hello world from main thread\n");
	
	for(size_t index=0; index<shared_data->numberOfThreads ;++index){
		pthread_join(thread_v[index], NULL);
	}
	
	free(private_data);
	free(thread_v);
	
	return 0;
}

void* run(void* data){
	private_data_t * private_data = (private_data_t*)data;
 	shared_data_t* shared_data = private_data->shared_data;
 	
 	size_t threadCounter = private_data->threadCounter;
 	size_t numberOfThreads = shared_data->numberOfThreads; 
 	// size_t threadCounter = (*private_data).threadCounter es lo mismo que ->
 	printf("Hello world from thread %zu of %zu\n", threadCounter, numberOfThreads);
	return NULL;
}


//Todo proceso tiene varias partes:
//Data segment
//Heap segment
//Stack segment
//Código
//Thread: conjunto de valores con registros
//main thread: program counter.... -> todo thread tiene esta parte
//valgrind ./hello_i_of_w ELNUMEROQUESEA
