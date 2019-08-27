#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* run(void* data){
	//(void)data; para no usar data
 	size_t thread_num = (size_t)data;
 	//fprintf(stderr, "Hello world from secondary thread %zu\n", thread_num);
 	int a=0;
 	for(int i=0;i<10000;i++){
		a++;
	}
 	printf("Hello world from secondary thread %zu\n", thread_num);
	return NULL;
}

int main(int argc, char* argv[]){
	size_t numberOfThreads;
	if(argc>1){
		numberOfThreads = atoi(argv[1]);
		// numberOfThreads = stroull(argv[1], NULL, 10);
	}
	else{
		numberOfThreads = sysconf(_SC_NPROCESSORS_ONLN);
	}
	
	//pthread_t thread_v [numberOfThreads];
	pthread_t* thread_v = malloc(numberOfThreads * sizeof(pthread_t));
	
	//DEBE ser size_t para ser solo los ints positivos
	for(size_t index=0; index<numberOfThreads ;++index){
		pthread_create(&thread_v[index], NULL, run, (void*)index);
	}
	
	printf("Hello world from main thread\n");
	
	for(size_t index=0; index<numberOfThreads ;++index){
		pthread_join(thread_v[index], NULL);
	
	}
	free(thread_v);
	return 0;
}

// INDERTERMINISMO: no se sabe el orden en el que terminan los diferentes threads 
	// man sysconf
	// malloc(bytes) NO INICIALIZA
	// calloc (cnt, size) SE INICIALIZA TODO CON 0
	// realloc ni idea jeje
	// free, liberar memoria
	// valgrind y el exe
	
