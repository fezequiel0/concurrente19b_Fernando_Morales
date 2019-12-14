#include <mpi.h>
#include <iostream>

#define MESSAGE_CAPACITY 1024

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	int my_rank = -1;
	int process_count = -1;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	char hostname[MPI_MAX_PROCESSOR_NAME];
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);

    int hot_potato = 0;

	if ( argc >= 2 ){
		hot_potato = atoi(argv[1]);
	}
	else
	{
		if ( my_rank == 0 )
		{
			std::cin >> hot_potato;
			for ( int destination = 1; destination < process_count; ++destination )
			{
				MPI_Send(&hot_potato, 1, MPI_INT, destination, /*tag*/ 0, MPI_COMM_WORLD);
			}
		}
		else{
			MPI_Recv(&hot_potato, 1, MPI_INT, /*source*/ 0, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}

    if(my_rank == 0){
        --hot_potato;
        if(hot_potato > 0){
            MPI_Send(&hot_potato, /*count*/ 1, MPI_INT, /*dest*/ my_rank + 1, /*tag*/ 0, MPI_COMM_WORLD);
        }
        else{
            std::cout << "The process with rank "<< my_rank << " lost." << std::endl;
            for ( int destination = 1; destination < process_count; ++destination ){
                MPI_Send(&hot_potato, 1, MPI_INT, destination, /*tag*/ 0, MPI_COMM_WORLD);
            }
        }
    }
    while(hot_potato > 0){
        MPI_Recv(&hot_potato, /*capacity*/ 1, MPI_INT, /*source*/ MPI_ANY_SOURCE, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        --hot_potato;
        if(hot_potato == 0 ){
            std::cout << "The process with rank "<< my_rank << " lost." <<std::endl;
            for ( int destination = 0; destination < process_count; ++destination ){
                if(destination != my_rank){
				    MPI_Send(&hot_potato, 1, MPI_INT, destination, /*tag*/ 0, MPI_COMM_WORLD);
                }
			}
        }
	    else{
            if(hot_potato > 0){
                if ( my_rank < process_count - 1 ){
                   MPI_Send(&hot_potato, /*count*/ 1, MPI_INT, /*dest*/ my_rank + 1, /*tag*/ 0, MPI_COMM_WORLD);
                }
                else{
                    MPI_Send(&hot_potato, /*count*/ 1, MPI_INT, /*dest*/ 0, /*tag*/ 0, MPI_COMM_WORLD);
                }
            }
        }
        

    }

	MPI_Finalize();
}