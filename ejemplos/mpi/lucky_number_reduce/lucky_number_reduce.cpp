#include <iostream>
#include <cstdio>
#include <mpi.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>

int main(int argc, char* argv[]){
	MPI_Init(&argc, &argv); 

	int my_rank = -1;
	int process_count = -1;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	
	char hostname[MPI_MAX_PROCESSOR_NAME]; 
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);
	
	srand(my_rank + time(nullptr) + clock());//para diferenciar los procesos
	int my_lucky_number = rand() % 100;
	
	int global_min = my_lucky_number;
	int global_max = my_lucky_number;
	int global_sum = my_lucky_number;
	
	MPI_Reduce(&my_lucky_number, &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);			
	MPI_Reduce(&my_lucky_number, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);			
	MPI_Reduce(&my_lucky_number, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);			

	if(	my_rank == 0){
		double global_average = static_cast<double>(global_sum) / process_count;
		std::cout << "Global min:" << std::setw(2) << global_min << std::endl;
		std::cout << "Global max:" << std::setw(2) << global_max << std::endl;
		std::cout << "Global sum:" << std::setw(2) << global_sum << std::endl;
		std::cout << "Global average:" << std::fixed << std::setprecision(2) << global_average << std::endl;
	}
	
	char next = 'Y';
	if(my_rank != 0){
		MPI_Recv(&next, /*capacity*/1, /*tipo de dato*/MPI_CHAR, /*source*/ my_rank - 1, /*tag*/0, MPI_COMM_WORLD, /*status: reportar mensajes de error*/ MPI_STATUS_IGNORE);
	}
	
	std::cout << "Process " << my_rank << ": my lucky number is " << std::setw(2) << std::setfill('0') << my_lucky_number << "\n";
	std::cout << "Global min for process " << my_rank << ": " << global_min << std::endl;
	std::cout << "Global max for process " << my_rank << ": " << global_max << std::endl;
	std::cout << "Global sum for process " << my_rank << ": " << global_sum << std::endl;
	
	if(my_rank < process_count - 1){
			MPI_Send(&next/*lo que se va a mandar*/, 1/*longitud del dato*/, /*tipo de dato*/MPI_CHAR, /*dest*/ my_rank + 1, /*tag: tipos de mensaje, puramente para el programador*/0, MPI_COMM_WORLD);
	}
	
	MPI_Finalize();
	return 0;
}
