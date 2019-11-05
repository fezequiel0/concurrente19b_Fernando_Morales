#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

void* run(void*); 

int main(int argc, char* argv[]){
	size_t number_of_trials;
	if(argc>1){
		number_of_trials = strtoull(argv[1], NULL, 10);
	}
	else{
		number_of_trials = 1;
	}
    
    pthread_t secondary_thread;

    struct timespec start_time;
	struct timespec finish_time;
	double elapsed_seconds;	
    for(size_t i = 0;i<number_of_trials;++i){
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        pthread_create(&secondary_thread, NULL, run, NULL);
		pthread_join(secondary_thread, NULL);
        clock_gettime(CLOCK_MONOTONIC, &finish_time);
        double temp = finish_time.tv_sec - start_time.tv_sec + 1e-9 * (finish_time.tv_nsec - start_time.tv_nsec);
        if(temp < elapsed_seconds || i==0){
            elapsed_seconds = temp;
        }
	}

    	
	printf("Minimum thread creation and destruction time was %.9lf s among %zu trials\n", elapsed_seconds, number_of_trials);
	return 0;
}

void* run(void* data){
    (void)data;
	return NULL;
}