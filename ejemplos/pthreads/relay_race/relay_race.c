#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

//#define INVERTED_TEAM_ORDER 1

typedef struct{
	size_t team_count;
	sem_t* batons;
	pthread_barrier_t start_barrier;
	useconds_t stage1_duration; 
	useconds_t stage2_duration;
	size_t position;
	pthread_mutex_t position_mutex;
} shared_data_t;

typedef struct{
	size_t my_team_number;// convencion de usar my en atributos privados 
	shared_data_t* shared_data;
} private_data_t;

int analyze_arguments(int argc, char* argv[],shared_data_t* shared_data);
int create_threads(shared_data_t* shared_data);
void* start_race(void* data);
void* finish_race(void* data);
void random_sleep(useconds_t min_milliseconds, useconds_t max_milliseconds);

int main(int argc, char* argv[]){
	shared_data_t* shared_data = calloc(1, sizeof(shared_data_t));
	if(shared_data == NULL){
		return (void)fprintf(stderr, "ERROR: could not allocate shared memory\n") , 2;
	}
		
	int error = analyze_arguments(argc,argv, shared_data);
	if (error == 0){
		
		shared_data->batons = (sem_t*) calloc(shared_data->team_count, sizeof(sem_t));
		if(shared_data->batons){
			for( size_t index=0; index< shared_data->team_count; ++index){
				sem_init(&shared_data->batons[index], 0 /*pshared*/ , 0);
			}
			
			pthread_barrier_init(&shared_data->start_barrier,NULL,shared_data->team_count);
				
			shared_data->position=0;	
			
			pthread_mutex_init(&shared_data->position_mutex, /*attr*/ NULL);

			struct timespec start_time;
			clock_gettime(CLOCK_MONOTONIC, &start_time);
	
			error = create_threads(shared_data);
			if(error == 0){
				struct timespec finish_time;
				clock_gettime(CLOCK_MONOTONIC, &finish_time);
		
				double elapsed_seconds = finish_time.tv_sec - start_time.tv_sec	+ 1e-9 * (finish_time.tv_nsec - start_time.tv_nsec);
	
				printf("Simulation time %.9lf s\n", elapsed_seconds);
	
			}
			
			pthread_mutex_destroy(&shared_data->position_mutex);
			pthread_barrier_destroy(&shared_data->start_barrier);
			free(shared_data->batons);
		}
		
		else{
			fprintf(stderr, "error: could not allocate memory for %zu batons\n", shared_data->team_count);
			error = 2;
		}
	
	}
	
	free(shared_data);
	return error;
}

int analyze_arguments(int argc, char* argv[],shared_data_t* shared_data){
	
	if( argc != 4){
		fprintf(stderr, "usage: relay_race teams stage1_duration stage2_duration\n");
		return 1;
	}
	
	if(sscanf(argv[1], 	"%zu", &shared_data->team_count) != 1 || shared_data->team_count == 0){
		return (void)fprintf(stderr,"invalid team count: %s\n", argv[1]), 1;	
	}
	
	if(sscanf(argv[2], 	"%u", &shared_data->stage1_duration) != 1){
		return (void)fprintf(stderr,"invalid stage 1 duration: %s\n", argv[2]), 2;	
	}
	
	if(sscanf(argv[3], 	"%u", &shared_data->stage2_duration) != 1){
		return (void)fprintf(stderr,"invalid stage 2 duration: %s\n", argv[3]), 3;	
	}
	
	return EXIT_SUCCESS;
}

int create_threads(shared_data_t* shared_data){
	size_t thread_count = 2 * shared_data->team_count;
	
	pthread_t* threads = malloc(thread_count * sizeof(pthread_t));
	
	if(threads == NULL){
		return (void)fprintf(stderr, "ERROR: could not allocate memory for %zu threads\n", thread_count) , 1; 
	}
	
	private_data_t* private_data = calloc(thread_count, sizeof(private_data_t));
	
	if(private_data == NULL){
		return (void)fprintf(stderr, "ERROR: could not allocate private memory for %zu threads\n", thread_count) , 3;
	}
	
	#if INVERTED_TEAM_ORDER
	for(size_t index = shared_data->team_count - 1; index < shared_data->team_count; --index){	
	#else
	for(size_t index=0; index<shared_data->team_count ;++index){
	#endif			
		private_data[index].my_team_number = index;
		private_data[index].shared_data = shared_data;
		pthread_create(&threads[index], NULL, start_race, &private_data[index]);
	}
	#if INVERTED_TEAM_ORDER
	for(size_t index = thread_count - 1; index >= shared_data->team_count; --index){
	#else
	for(size_t index= shared_data->team_count; index < thread_count ;++index){
	#endif
		private_data[index].my_team_number = index - shared_data->team_count;
		private_data[index].shared_data = shared_data;
		pthread_create(&threads[index], NULL, finish_race, &private_data[index]);
	}
	
	for(size_t index=0; index<thread_count ;++index){
		pthread_join(threads[index], NULL);
	}	
		
	free(private_data);
	free(threads);
	
	return 0;
}

void* start_race(void* data){
	private_data_t * private_data = (private_data_t*)data;
 	shared_data_t* shared_data = private_data->shared_data;
 	
 	
 	pthread_barrier_wait(&shared_data->start_barrier);
	usleep(1000 * shared_data->stage1_duration);
	sem_post(&shared_data->batons[private_data->my_team_number]);
	 	
	return NULL;
}

void* finish_race(void* data){
	private_data_t * private_data = (private_data_t*)data;
 	shared_data_t* shared_data = private_data->shared_data;
 	
 	sem_wait(&shared_data->batons[private_data->my_team_number]);
	
	usleep(1000 * shared_data->stage2_duration);
	
	pthread_mutex_lock(&shared_data->position_mutex);
 	if(shared_data->position < 3){
		printf("Place %zu: team %zu\n", ++shared_data->position, private_data->my_team_number + 1);
	}
 	pthread_mutex_unlock(&shared_data->position_mutex);
 	
	return NULL;
}
