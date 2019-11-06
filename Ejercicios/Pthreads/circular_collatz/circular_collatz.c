// En el ejercicio anterior, ocurría que se sobreescribían los valores en el arreglo numbers, lo cual impedía el funcionamiento correcto de la intencionalidad del código.  

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
	int finished;
	size_t worker_count;
	size_t number_count;
	size_t* pre_numbers;
	size_t* post_numbers;
	size_t current_step;
	size_t max_steps;
	pthread_barrier_t barrier;

}shared_data_t;

typedef struct {
    shared_data_t* shared_data;
    size_t thread_num; 
} private_data_t;

void* calculate(void* data);

int main(){
	int error = 0;
    shared_data_t * shared_data = calloc(1, sizeof(shared_data_t));
    if(shared_data == NULL){
		return (void)fprintf(stderr, "ERROR: could not allocate shared memory\n") , 1;
	}

    scanf("%zu %zu %zu\n", &shared_data->number_count, &shared_data->max_steps, &shared_data->worker_count);
	if(shared_data->number_count && shared_data->max_steps && shared_data->worker_count){
	
		shared_data->pre_numbers = calloc( shared_data->number_count, sizeof(size_t) );
		shared_data->post_numbers = calloc( shared_data->number_count, sizeof(size_t) );
		for ( size_t index = 0; index < shared_data->number_count; ++index ){
			scanf("%zu", &shared_data->pre_numbers[index]);
		}
		shared_data->finished = 0;
				
		pthread_barrier_init(&shared_data->barrier, NULL, shared_data->worker_count);	
		
		pthread_t * threads = calloc(shared_data->worker_count,sizeof(pthread_t));
		if(threads){
			private_data_t * private_data = calloc(shared_data->worker_count, sizeof(private_data_t));
        	if(private_data){
            	for(size_t index = 0;index<shared_data->worker_count; ++index ){
            	    private_data[index].thread_num = index;
	    		    private_data[index].shared_data = shared_data;
    			    pthread_create(&threads[index], NULL, calculate, &private_data[index]);
            	}

				for(size_t index=0; index<shared_data->worker_count;++index){
		      		pthread_join(threads[index], NULL);
	       	 	}	
				pthread_barrier_destroy(&shared_data->barrier);

				if (!shared_data->finished){
					printf("No converge in %zu steps", shared_data->max_steps);
				}
				else{
					printf("Converged in %zu steps", shared_data->current_step);
				}
	        	free(private_data);
	        	free(threads);
	
        	}
	        else{
            	fprintf(stderr, "ERROR: could not allocate private memory for %zu threads\n", shared_data->worker_count);
            	error = 2;    
        	}
    	}
    	else{
        	fprintf(stderr, "ERROR: could not allocate memory for %zu threads\n", shared_data->worker_count);
        	error = 1;
    	}		
	}
	else{
		fprintf(stderr, "ERROR:invalid numbers\n");
		error = 2;
	}
	free(shared_data);
	return error;
}

void* calculate(void* data){

	private_data_t * private_data = (private_data_t*)data;
	shared_data_t * shared_data = private_data->shared_data;
	while( shared_data->current_step < shared_data->max_steps && shared_data->finished == 0){
		for(size_t my_number = private_data->thread_num; my_number < shared_data->number_count; my_number += shared_data->worker_count){
			size_t next = (my_number + 1) % shared_data->number_count;
			size_t prev = (size_t)((long long)my_number - 1) % shared_data->number_count;

			if (shared_data->pre_numbers[my_number] > 1) {
				if (shared_data->pre_numbers[my_number] % 2 == 0) {
					shared_data->post_numbers[my_number] = shared_data->pre_numbers[my_number]/2;
				}
				else{
					shared_data->post_numbers[my_number] = shared_data->pre_numbers[prev] * shared_data->pre_numbers[my_number] + shared_data->pre_numbers[next];
				}
			}
		}

		pthread_barrier_wait(&shared_data->barrier);

		if(private_data->thread_num == 0) {
			++shared_data->current_step;
			size_t counter = 0;
			for(size_t index = 0; index < shared_data->number_count; ++index) {
				shared_data->pre_numbers[index] = shared_data->post_numbers[index];
				if(shared_data->pre_numbers[index] == 1) {
					++counter;
				}
			}
			if(counter == shared_data->number_count) {
				shared_data->finished = 1;
			}
		}

		pthread_barrier_wait(&shared_data->barrier);
	}
	return NULL;
}
