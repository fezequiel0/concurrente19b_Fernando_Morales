#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

typedef struct{
	size_t buffer_size;  
	double* buffer;
	size_t rounds;
	useconds_t min_producer_delay; 
	useconds_t max_producer_delay;
	useconds_t min_consumer_delay;
	useconds_t max_consumer_delay;
	pthread_mutex_t stdout_mutex;
	sem_t producer_semaphore;
	sem_t consumer_semaphore;
} shared_data_t;

int analyze_arguments(int argc, char* argv[],shared_data_t* shared_data);
int create_threads(shared_data_t* shared_data);
void* produce(void* data);
void* consume(void* data);
void random_sleep(useconds_t min_milliseconds, useconds_t max_milliseconds);

int main(int argc, char* argv[]){
	srand( time(NULL) );
	shared_data_t* shared_data = calloc(1, sizeof(shared_data_t));
	

	if(shared_data == NULL){
		return (void)fprintf(stderr, "ERROR: could not allocate shared memory\n") , 2;
	}
		
	int error = analyze_arguments(argc,argv, shared_data);
	if (error == 0){
		
		shared_data->buffer = (double*) calloc(shared_data->buffer_size, sizeof(double));
		if(shared_data->buffer){
			
			sem_init(&shared_data->producer_semaphore, 0 /*pshared*/ , shared_data->buffer_size);
			sem_init(&shared_data->consumer_semaphore, 0 /*pshared*/ , 0);
			pthread_mutex_init(&shared_data->stdout_mutex, /*attr*/ NULL);

			struct timespec start_time;
			clock_gettime(CLOCK_MONOTONIC, &start_time);
	
			error = create_threads(shared_data);
			if(error == 0){
				struct timespec finish_time;
				clock_gettime(CLOCK_MONOTONIC, &finish_time);
		
				double elapsed_seconds = finish_time.tv_sec - start_time.tv_sec	+ 1e-9 * (finish_time.tv_nsec - start_time.tv_nsec);
	
				printf("Simulation time %.9lf s\n", elapsed_seconds);
	
			}
		
			pthread_mutex_destroy(&shared_data->stdout_mutex);
			free(shared_data->buffer);
		}
		
		else{
			fprintf(stderr, "error: could not allocate memory for %zu products\n", shared_data->buffer_size);
			error = 2;
		}
	
	}
	
	free(shared_data);
	return error;
}

int analyze_arguments(int argc, char* argv[],shared_data_t* shared_data){
	
	if( argc != 7){
		fprintf(stderr, "usage: producer_consumer buffer_size rounds min_producer_delay max_producer_delay min_consumer_delay max_consumer_delay ");
		return 1;
	}
	
	shared_data->buffer_size = strtoull(argv[1],NULL,10);
	if(shared_data->buffer_size == 0){
		return 2;
	}
	
	if(sscanf(argv[2], 	"%zu", &shared_data->rounds) != 1 || shared_data->rounds == 0){
		return (void)fprintf(stderr,"invalid rounds: %s\n", argv[2]), 2;	
	}
	
	if(sscanf(argv[3], 	"%u", &shared_data->min_producer_delay) != 1){
		return (void)fprintf(stderr,"invalid min producer delay: %s\n", argv[3]), 3;	
	}
	
	if(sscanf(argv[4], 	"%u", &shared_data->max_producer_delay) != 1 || shared_data->max_producer_delay < shared_data->min_producer_delay){
		return (void)fprintf(stderr,"invalid max producer delay: %s\n", argv[4]), 4;	
	}
	
	if(sscanf(argv[5], 	"%u", &shared_data->min_consumer_delay) != 1){
		return (void)fprintf(stderr,"invalid min consumer delay: %s\n", argv[5]), 5;	
	}

	if(sscanf(argv[6], 	"%u", &shared_data->max_consumer_delay) != 1 || shared_data->max_consumer_delay < shared_data->min_consumer_delay){
		return (void)fprintf(stderr,"invalid max consumer delay: %s\n", argv[6]), 6;	
	}
	
	return EXIT_SUCCESS;
}

int create_threads(shared_data_t* shared_data){
	
	pthread_t producer_thread;
	pthread_t consumer_thread;
				
	pthread_create(&producer_thread, NULL, produce, shared_data);
	pthread_create(&consumer_thread, NULL, consume, shared_data);
	
	pthread_join(producer_thread, NULL);
	pthread_join(consumer_thread, NULL);
	
	return 0;
}

void* produce(void* data){
	shared_data_t* shared_data = (shared_data_t*)data;
 	
 	for(size_t round =1; round <=shared_data->rounds; ++round){
		for(size_t index = 0; index < shared_data->buffer_size; ++index){
			
			sem_wait(&shared_data->producer_semaphore);
			shared_data->buffer[index] = round + (index+1.0) / 100.0;//CODIGO QUEMADO ALERT: simplemente se usa 100 arbitrariamente
				
			random_sleep(shared_data->min_producer_delay, shared_data->max_producer_delay);
		
			pthread_mutex_lock(&shared_data->stdout_mutex);
			printf("Produced %.2lf\n", shared_data->buffer[index]);
			pthread_mutex_unlock(&shared_data->stdout_mutex);
 	
			sem_post(&shared_data->consumer_semaphore);
		}
	} 	
 	
	return NULL;
}

void* consume(void* data){
	shared_data_t* shared_data = (shared_data_t*)data;

	for(size_t round =1; round <= shared_data->rounds; ++round){
		for(size_t index = 0; index < shared_data->buffer_size; ++index){
		
			sem_wait(&shared_data->consumer_semaphore);
		
			random_sleep(shared_data->min_consumer_delay, shared_data->max_consumer_delay);
		
			pthread_mutex_lock(&shared_data->stdout_mutex);
 			printf("\t\t\tConsumed %.2lf\n", shared_data->buffer[index]);
 			pthread_mutex_unlock(&shared_data->stdout_mutex);
 	
			
			sem_post(&shared_data->producer_semaphore);
		}
	} 	
	return NULL;
}

void random_sleep(useconds_t min_milliseconds, useconds_t max_milliseconds){
	useconds_t duration = min_milliseconds;
	useconds_t range = max_milliseconds - min_milliseconds;
	if( range > 0){
		duration += rand() % range;
	}	
	usleep(1000 * duration);
}

/* 
 Separacion de tareas / separacacion de asuntos / paralelismo de tareas -> mismos datos, diferentes tareas
*/
