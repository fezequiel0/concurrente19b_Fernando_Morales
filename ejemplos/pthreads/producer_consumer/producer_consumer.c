#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

typedef struct{
	size_t numberOfThreads;
	size_t position;  
	pthread_mutex_t position_mutex;
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
	
	pthread_mutex_init(&shared_data->position_mutex, /*attr*/ NULL);
	shared_data-> position = 0;
	
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
	
	pthread_mutex_destroy(&shared_data->position_mutex);
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
	
	pthread_mutex_lock(&shared_data->position_mutex);
	
	printf("Hello world from main thread\n");
	
	pthread_mutex_unlock(&shared_data->position_mutex);
 	
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
 	
 	pthread_mutex_lock(&shared_data->position_mutex);
 	
 	++shared_data->position;
	printf("Thread %zu / %zu : I arrived at position %zu\n", threadCounter, numberOfThreads, shared_data->position);
 	
 	pthread_mutex_unlock(&shared_data->position_mutex);
 	
	return NULL;
}

/* 
 Condicion de carrera: cuando dos procesos intentan modificar el mismo archivo al mismo tiempo
 Deben ocurrir tres cosas:
 1. Codigo concurrente
 2. Memoria compartida
 3. Modificacion y escritura, nunca lectura;
 Codigo que genera condiciones de carrera: region critica
 valgrind --tool=helgrind ./position_race
 La memoria en el heap puede ser compartida o privada, a la privada  solo vamos a poder accesarlas con punteros


 Mecanismos de control de flujo -> while, if, for....
 Mecanismos de control de concurrencia = mecanismos de sincronizacion  
	Mutex(lock): mut-ual -ex-clusion -> cuello de botella, unicamente deja pasar a un proceso -> se interpreta como un valor booleano
	Semaphore: 
	Barrier:
	Condition variable:
	RW locks:
*/
