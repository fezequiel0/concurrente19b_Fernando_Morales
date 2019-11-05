#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

typedef struct{
    size_t thread_count;
    double result;
    double bottom_limit;
    double top_limit;
    size_t number_of_trapezoids;
    pthread_mutex_t result_mutex;
} shared_data_t;

typedef struct {
    shared_data_t* shared_data;
    size_t thread_num; 
} private_data_t;

int create_threads(shared_data_t * );
void * trapezoidal_area(void *);
double witch_of_agnesi(double);

int main(int argc, char*argv[]){
    int error = 0;
    shared_data_t * shared_data = calloc(1, sizeof(shared_data_t));
    if(shared_data == NULL){
		return (void)fprintf(stderr, "ERROR: could not allocate shared memory\n") , 5;
	}
    if(argc >= 4){
        shared_data->bottom_limit = strtod(argv[1], NULL);
        if(!shared_data->bottom_limit){
            fprintf(stderr, "ERROR: invalid bottom_limit\n");
            error = 2;
        }
        shared_data->top_limit = strtod(argv[2], NULL);
        if(!shared_data->top_limit){
            fprintf(stderr, "ERROR: invalid upper_limit\n");
	    	error = 3;
        }        
        shared_data->number_of_trapezoids = strtoull(argv[3],NULL,10);    
        if(!shared_data->number_of_trapezoids){
            fprintf(stderr, "ERROR: invalid number_of_trapezoids\n");
	    	error = 4;
        }
        shared_data->thread_count = sysconf(_SC_NPROCESSORS_ONLN);
        if(argc >4){
            shared_data->thread_count = strtoull(argv[4],NULL,10);
            if(!shared_data->thread_count){
                fprintf(stderr, "ERROR: invalid number_of_threads\n");
	    	    error = 6;
            }        
        }
	    	    
        if(!error){
            pthread_mutex_init(&shared_data->result_mutex, NULL);
            struct timespec start_time;
	    	clock_gettime(CLOCK_MONOTONIC, &start_time);
            error = create_threads(shared_data);
            if(!error){
                struct timespec finish_time;
                clock_gettime(CLOCK_MONOTONIC, &finish_time);
                double elapsed_seconds = finish_time.tv_sec - start_time.tv_sec	+ 1e-9 * (finish_time.tv_nsec - start_time.tv_nsec);
                printf("Result : %f\n", shared_data->result);
                printf("Simulation time %.9lf s\n", elapsed_seconds);
            }
            pthread_mutex_destroy(&shared_data->result_mutex);
        }
	}
    else{
        fprintf(stderr, "usage: bottom_limit upper_limit number_of_trapezoids number_of_threads\n");
		error = 1;
	}    
    free(shared_data);
    return error;
}

int create_threads(shared_data_t * shared_data){
    int error = 0;
    pthread_t * threads = calloc(shared_data->thread_count,sizeof(pthread_t));
    if(threads){
        private_data_t * private_data = calloc(shared_data->thread_count, sizeof(private_data_t));
        if(private_data){
            for(size_t index = 0;index<shared_data->thread_count; ++index ){
                private_data[index].thread_num = index;
	    	    private_data[index].shared_data = shared_data;
    		    pthread_create(&threads[index], NULL, trapezoidal_area, &private_data[index]);
            }

            for(size_t index=0; index<shared_data->thread_count;++index){
		        pthread_join(threads[index], NULL);
	        }	
		
	        free(private_data);
	        free(threads);
	
        }
        else{
            fprintf(stderr, "ERROR: could not allocate private memory for %zu threads\n", shared_data->thread_count);
            error = 2;    
        }
    }
    else{
        fprintf(stderr, "ERROR: could not allocate memory for %zu threads\n", shared_data-> thread_count);
        error = 1;
    }
    return error;
}

void * trapezoidal_area(void* data){
    private_data_t * private_data = (private_data_t*)data;
    shared_data_t * shared_data = private_data->shared_data;
    double result = 0;
    double sum = (shared_data->top_limit - shared_data->bottom_limit)/(double)shared_data->number_of_trapezoids;        
    for(double index = shared_data->bottom_limit + (sum * (double)private_data->thread_num); index < shared_data->top_limit; index+= (sum * (double)shared_data->thread_count)){
        result += sum*((witch_of_agnesi(index)+witch_of_agnesi(index+sum))/((double)2));
    }
    pthread_mutex_lock(&shared_data->result_mutex);
    shared_data->result += result;
    pthread_mutex_unlock(&shared_data->result_mutex);
    
    return NULL;
}

double witch_of_agnesi(double x){
    double a = 3;
    return a*a*a / (x*x + a*a);
}