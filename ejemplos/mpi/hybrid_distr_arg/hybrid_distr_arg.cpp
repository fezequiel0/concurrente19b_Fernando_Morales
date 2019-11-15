#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <omp.h>

int calculate_start(int worker_id, int workers, int finish, int begin){
	int range = finish - begin;
	return begin + worker_id * (range/workers) + std::min(worker_id, range % workers);
}
int calculate_finish(int worker_id, int workers, int finish, int begin){
	return calculate_start(worker_id + 1, workers, finish, begin);	
}

int main(int argc, char* argv[]){
	MPI_Init(&argc, &argv);

	int my_rank = -1;
	int process_count = -1;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	char hostname[MPI_MAX_PROCESSOR_NAME];
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);
	if(argc == 3){
		const int global_start = atoi(argv[1]);
		const int global_finish = atoi(argv[2]);
		
		const int my_start = calculate_start(my_rank, process_count, global_finish, global_start);
		const int my_finish = calculate_finish(my_rank, process_count, global_finish, global_start);
		const int my_width = my_finish - my_start;
		
		std::cout << hostname << ":" << my_rank << ": range [" << my_start  << ", " << my_finish << "[ size " << my_width << std::endl;
	
		#pragma omp parallel default(none) shared(my_rank, hostname, std::cout)
		{
			
			int my_thread_start = 0;
			int my_thread_finish = 0;
			
			#pragma omp for 
				for(int index = my_start; index < ++index)		
					if(my_thread_start == 0){
						my_thread_start = index;
						my_thread_finish = index;
					}
				}
				
			#pragma omp critical(stdout)
			{
				std::cout << "\tHello from thread "<< omp_get_thread_num() << " of " << omp_get_num_threads() << " of process " << my_rank << " on " << hostname << std::endl;
			}				
	}
		MPI_Finalize();
	return 0;
}
