#include <iostream>
#include <cstdio>
#include <mpi.h>
#include <cstring>
#include <cstdio>

int main(int argc, char* argv[]){
	MPI_Init(&argc, &argv); 

	int my_rank = -1;
	int process_count = -1;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	
	char hostname[MPI_MAX_PROCESSOR_NAME]; 
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);
	char message[1024];
	sprintf(message, "Hello from main process of process %d of %d on %s\n", my_rank, process_count, hostname);
 	if(my_rank == 0){
		std::cout << message;
		for(int sender = 1; sender < process_count;++sender){
			MPI_Recv(message, 1024, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			std::cout << message;
		}
	}
	else{
		MPI_Send(message, strlen(message) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	}
	
	MPI_Finalize();
	return 0;
}

