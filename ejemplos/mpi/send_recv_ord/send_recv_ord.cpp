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
	//std::cout << "Hello from main thread of process " << my_rank << " of " << process_count  << " on " << hostname << "\n";
	if(my_rank == 0){
		std::cout << message;
		for(int sender = 1; sender < process_count;++sender){
			MPI_Recv(message, /*capacity*/1024, /*tipo de dato*/MPI_CHAR, /*source*/ sender, /*tag*/0, MPI_COMM_WORLD, /*status: reportar mensajes de error*/ MPI_STATUS_IGNORE);
			std::cout << message;
		}
	}
	else{
		MPI_Send(message/*lo que se va a mandar*/, strlen(message) + 1/*longitud del dato*/, /*tipo de dato*/MPI_CHAR, /*dest*/ 0, /*tag: tipos de mensaje, puramente para el programador*/0, MPI_COMM_WORLD);
	}
	
	MPI_Finalize();
	return 0;
}

//comunicacion entre MPI: point2point y comunicación colectiva
//tipos de send:
//buffer BSend			BRecv
//non-blocking ISend	IRecv
//              Send	 Recv
//ready RSend			RRecv
//synchronous SSend		SRecv
