#include <iostream>
#include <omp.h>

int main(int argc, char* argv[]){
	int thread_count = omp_get_max_threads();
	if(argc > 1){
		thread_count = atoi(argv[1]);
	}
	#pragma omp	parallel num_threads(thread_count)// crear threads paralelos, las subrutinas se invocan dentro de esta región 
	{
		#pragma omp critical(cout_mutex) // crear una zona crítica (mutex)
		std::cout << "Hello world from secondary thread " << omp_get_thread_num() << " of "<< omp_get_num_threads() << std::endl;
	}
	return 0;
}

