#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct{
	size_t thread_count;
	pthread_mutex_t* mutexes;
} shared_data_t;

typedef struct{
	size_t thread_num;
	shared_data_t* shared_data;
} private_data_t;

int create_threads(shared_data_t* shared_data);
void* run(void* data);

int main(int argc, char* argv[]){
	shared_data_t* shared_data = (shared_data_t*) calloc(1, sizeof(shared_data_t));
	if ( shared_data == NULL ){
		return (void)fprintf(stderr, "error: could not allocate shared memory\n"), 1;
    }
	shared_data->thread_count = sysconf(_SC_NPROCESSORS_ONLN);
	if ( argc >= 2 ){
		shared_data->thread_count = strtoull(argv[1], NULL, 10);
    }
    shared_data->mutexes = (pthread_mutex_t*) calloc(shared_data->thread_count, sizeof(pthread_mutex_t));
		
    for(size_t index = 0; index< shared_data->thread_count; ++index){
        pthread_mutex_init(&shared_data->mutexes[index], /*attr*/ NULL);
        if(index != 0){
            pthread_mutex_lock(&shared_data->mutexes[index]);
        }
    }

	struct timespec start_time;
	clock_gettime(CLOCK_MONOTONIC, &start_time);

	int error = create_threads(shared_data);
	if ( error ){
		return error;
    }
	struct timespec finish_time;
	clock_gettime(CLOCK_MONOTONIC, &finish_time);

	double elapsed_seconds = finish_time.tv_sec - start_time.tv_sec + 1e-9 * (finish_time.tv_nsec - start_time.tv_nsec);

	printf("Hello execution time %.9lfs\n", elapsed_seconds);
    free(shared_data->mutexes);
	free(shared_data);
	return 0;
}

int create_threads(shared_data_t* shared_data)
{
	pthread_t* threads = (pthread_t*) malloc(shared_data->thread_count * sizeof(pthread_t));
	if ( threads == NULL ){
		return (void)fprintf(stderr, "error: could not allocate memory for %zu threads\n", shared_data->thread_count), 2;
    }
	private_data_t* private_data = (private_data_t*) calloc(shared_data->thread_count, sizeof(private_data_t));
	if ( private_data == NULL ){
		return (void)fprintf(stderr, "error: could not allocate private memory for %zu threads\n", shared_data->thread_count), 3;
    }

	for ( size_t index = 0; index < shared_data->thread_count; ++index )
	{
		private_data[index].thread_num = index;
		private_data[index].shared_data = shared_data;
		pthread_create(&threads[index], NULL, run, &private_data[index]);
        
	}

	printf("Hello world from main thread\n");

	for ( size_t index = 0; index < shared_data->thread_count; ++index ){
		pthread_join(threads[index], NULL);
    }
    for(size_t index = 0; index<shared_data->thread_count; ++index){
		pthread_mutex_destroy(&shared_data->mutexes[index]);
    }
    
	free(private_data);
	free(threads);
	return 0;
}

void* run(void* data)
{
	private_data_t* private_data = (private_data_t*)data;
	shared_data_t* shared_data = private_data->shared_data;

	size_t thread_num = (*private_data).thread_num;
	size_t thread_count = shared_data->thread_count;
	
	while(pthread_mutex_trylock(&shared_data->mutexes[thread_num]) != 0){
    }

    printf("Hello world from secondary thread %zu of %zu\n", thread_num, thread_count);
    if(thread_num!=thread_count){
        pthread_mutex_unlock(&shared_data->mutexes[++thread_num]);
    }
	return NULL;
}