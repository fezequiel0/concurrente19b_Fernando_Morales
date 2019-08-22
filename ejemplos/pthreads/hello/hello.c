#include <stdio.h>
#include <pthread.h>

//int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
//   void *(*start_routine)(void*), void *arg);

void* run(void* data){
	//(void)data; para no usar data
 	size_t thread_num = (size_t)data;
 	printf("Hello world from secondary thread %zu\n", thread_num);
	return NULL;
}

void* correr(void* data1){
	size_t thread_num1 = (size_t)data1;
	int a;
	int b; 
	for(a=0;a<100000;a++){	
		b++;
	}
	a=a+b;
	printf("Hello world from tertiary thread %zu and this is my sum: %d\n" , thread_num1, a);	
	return NULL;
}

int main(void){
	pthread_t thread;
	pthread_create(&thread, NULL, run, (void*)6);
	
	pthread_t thread1;
	pthread_create(&thread1, NULL, correr, (void*)1000);	
	
	printf("Hello world from main thread\n");
	pthread_join(thread, NULL);
	pthread_join(thread1, NULL);
	return 0;
}
