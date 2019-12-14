#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <mpi.h>
#include <omp.h>

int calculate_start(int worker_id, int workers, int finish, int begin)
{
	int range = finish - begin;
	return begin + worker_id * (range / workers) + std::min(worker_id, range % workers);
}

int calculate_finish(int worker_id, int workers, int finish, int begin)
{
	return calculate_start(worker_id + 1, workers, finish, begin);
}

bool is_prime(size_t number){
	if ( number < 2 ){ 
        return false;
    }
	if ( number == 2 ){ 
        return true;
    }
	if ( number % 2 == 0 ){ 
        return false;
    }
	for ( size_t i = 3, last = (size_t)(double)sqrt(number); i <= last; i += 2 ){
		if ( number % i == 0 ){
			return false;
        }
    }
	return true;
}

int main(int argc, char* argv[])
{	
	int thread_count = omp_get_max_threads();
	MPI_Init(&argc, &argv);

	int my_rank = -1;
	int process_count = -1;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	char hostname[MPI_MAX_PROCESSOR_NAME];
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);

	int global_start = 0;
	int global_finish = 0;
	double start_time = MPI_Wtime();

	if ( argc >= 3 )
	{
		global_start = atoi(argv[1]);
		global_finish = atoi(argv[2]);
	}
	else
	{
		if ( my_rank == 0 )
		{
			std::cin >> global_start >> global_finish;
			for ( int destination = 1; destination < process_count; ++destination )
			{
				MPI_Send(&global_start, 1, MPI_INT, destination, /*tag*/ 0, MPI_COMM_WORLD);
				MPI_Send(&global_finish, 1, MPI_INT, destination, /*tag*/ 0, MPI_COMM_WORLD);
			}
		}
		else
		{
			MPI_Recv(&global_start, 1, MPI_INT, /*source*/ 0, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&global_finish, 1, MPI_INT, /*source*/ 0, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}

	const int my_start = calculate_start( my_rank, process_count, global_finish, global_start);
	const int my_finish = calculate_finish( my_rank, process_count, global_finish, global_start);

	// hostname1:0: range [3, 12[ size 9 in 0.000123s
	//std::cout << hostname << ":" << my_rank << ": range [" << my_start
	//	<< ", " << my_finish << "[ size " << my_width
	//	<< " in " << std::fixed << std::setprecision(9) << elapsed << 's' << std::endl;

	int prime_count = 0;
	int private_count = 0;
	int global_prime = 0;
	int global_threads = 0;
	#pragma omp parallel for num_threads(thread_count) default(none) shared(prime_count) reduction(+:private_count)
		for(int iteration = my_start;iteration<my_finish;++iteration){
			if(is_prime(iteration)){
				++private_count;
			}
		}

		#pragma omp critical(prime_count)
        	prime_count += private_count;
    	// int my_thread_start = 0;
		// int my_thread_finish = 0;

		// #pragma omp for
		// for ( int index = my_start; index < my_finish; ++index )
		// {
		// 	if ( my_thread_start == 0 )
		// 		my_thread_start = index;
		// 	my_thread_finish = index;
		// }

		// const int my_thread_width = ++my_thread_finish - my_thread_start;

		// hostname1:0.0: range [3,6[ size 3
//		#pragma omp critical(stdout)
//		std::cout << '\t' << hostname << ":" << my_rank << ":" << omp_get_thread_num() << ": range ["
//			<< my_thread_start << "," << my_thread_finish << "[ size " << my_thread_width << std::endl;

	MPI_Reduce(&prime_count, &global_prime, /*count*/ 1, MPI_INT, MPI_SUM, /*root*/ 0, MPI_COMM_WORLD);
	MPI_Reduce(&thread_count, &global_threads, /*count*/ 1, MPI_INT, MPI_SUM, /*root*/ 0, MPI_COMM_WORLD);
	double elapsed = MPI_Wtime() - start_time;
	double global_elapsed = 0;

	MPI_Reduce(&elapsed, &global_elapsed, /*count*/ 1, MPI_DOUBLE, MPI_MAX, /*root*/ 0, MPI_COMM_WORLD);
	int global_processes = 0;
	MPI_Reduce(&my_rank, &global_processes, /*count*/ 1, MPI_INT, MPI_MAX, /*root*/ 0, MPI_COMM_WORLD);

	if(my_rank == 0){
		fprintf(stdout, "%d primes found in range [%d, %d[ in %.9lfs with %d processes and %d threads\n", global_primes, global_start, global_finish, global_elapsed,global_processes+1, global_threads);
	}
	MPI_Finalize();
}
