#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

typedef struct{
	size_t numberOfThreads;
	size_t order;  
} shared_data_t;

typedef struct{
	size_t threadCounter; 
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
	
	shared_data-> order = 0;
	
	struct timespec start_time;
	clock_gettime(CLOCK_MONOTONIC, &start_time);
	
	int error = create_threads(shared_data);
	if(error){
			return error;
	}
	
	struct timespec finish_time;
	clock_gettime(CLOCK_MONOTONIC, &finish_time);
		
	double elapsed_seconds = finish_time.tv_sec - start_time.tv_sec	+ 1e-9 * (finish_time.tv_nsec - start_time.tv_nsec);
	
	printf("Hello execution time %.9lf s\n", elapsed_seconds);
	
	free(shared_data);
			
	return 0;
}

int create_threads(shared_data_t* shared_data){
	pthread_t* thread_v = malloc(shared_data->numberOfThreads * sizeof(pthread_t));
	
	if(thread_v == NULL){
		return (void)fprintf(stderr, "ERROR: could not allocate memory for %zu threads\n", shared_data-> numberOfThreads) , 1; 
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
	while((int)shared_data->order != (int)shared_data->numberOfThreads)
		;
		//Espera pero fea :(
		// Se puede hacer con llaves también
		// busy wait
		// NO SE USA 
	
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
 	
 	while((int)shared_data->order < (int)threadCounter)
		; // NUNCA SE USA ESPERA ACTIVA, SE USA ESPERA PASIVA 
	
	printf("Hello world from thread %zu of %zu\n", threadCounter, numberOfThreads);
	
	shared_data->order++;
	
	return NULL;
}
