#include <iostream>

int main(){
	#pragma omp	parallel // crear threads paralelos 
	{
		#pragma omp critical(cout_mutex) // crear una zona crítica (mutex)
		std::cout << "Hello world from secondary thread"<< std::endl;
	}
	return 0;
}
// PARA EL PROXIMO EJEMPLO
/*int argc, char* argv[]*/
//std::cout << "Hello world from secondary thread " << thread_num << " of "<< thread_count << std::endl;
