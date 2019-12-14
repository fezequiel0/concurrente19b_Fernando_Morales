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

    int original_hot_potato = 0;
    int hot_potato = 0;
    int number_processes_out = 0;
    int im_out = 0;
    	if ( argc >= 2 ){
		original_hot_potato = atoi(argv[1]);
	}
	else
	{
		if ( my_rank == 0 )
		{
			std::cin >> hot_potato;
			for ( int destination = 1; destination < process_count; ++destination )
			{
				MPI_Send(&original_hot_potato, 1, MPI_INT, destination, /*tag*/ 0, MPI_COMM_WORLD);
			}
		}
		else{
			MPI_Recv(&original_hot_potato, 1, MPI_INT, /*source*/ 0, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}

    if(my_rank == 0){
        hot_potato = original_hot_potato; 
        --hot_potato;
        if(hot_potato > 0){
            MPI_Send(&number_processes_out, 1, MPI_INT, my_rank + 1, /*tag*/ 0, MPI_COMM_WORLD);
            MPI_Send(&hot_potato, /*count*/ 1, MPI_INT, /*dest*/ my_rank + 1, /*tag*/ 0, MPI_COMM_WORLD);
        }
        else{
            im_out = 1;
            ++number_processes_out;
            MPI_Send(&number_processes_out, 1, MPI_INT, my_rank + 1, /*tag*/ 0, MPI_COMM_WORLD);
            MPI_Send(&original_hot_potato, 1, MPI_INT, my_rank + 1, /*tag*/ 0, MPI_COMM_WORLD);
        }
    }
    while(hot_potato > 0){
        MPI_Recv(&number_processes_out, /*capacity*/ 1, MPI_INT, /*source*/ MPI_ANY_SOURCE, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&hot_potato, /*capacity*/ 1, MPI_INT, /*source*/ MPI_ANY_SOURCE, /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if(im_out && number_processes_out != process_count - 1){
            if ( my_rank < process_count - 1 ){
                MPI_Send(&number_processes_out, /*count*/ 1, MPI_INT, /*dest*/ my_rank + 1, /*tag*/ 0, MPI_COMM_WORLD);
                MPI_Send(&original_hot_potato, /*count*/ 1, MPI_INT, /*dest*/ my_rank + 1, /*tag*/ 0, MPI_COMM_WORLD);
            }
            else{
                MPI_Send(&number_processes_out, /*count*/ 1, MPI_INT, /*dest*/ 0, /*tag*/ 0, MPI_COMM_WORLD);
                MPI_Send(&original_hot_potato, /*count*/ 1, MPI_INT, /*dest*/ 0, /*tag*/ 0, MPI_COMM_WORLD);
            }
        }
        else{
            --hot_potato;
            if(hot_potato == 0 ){
                ++number_processes_out;
                im_out = 1;
                if ( my_rank < process_count - 1 ){
                    MPI_Send(&number_processes_out, /*count*/ 1, MPI_INT, /*dest*/ my_rank + 1, /*tag*/ 0, MPI_COMM_WORLD);
                    MPI_Send(&original_hot_potato, /*count*/ 1, MPI_INT, /*dest*/ my_rank + 1, /*tag*/ 0, MPI_COMM_WORLD);
                }
                else{
                    MPI_Send(&number_processes_out, /*count*/ 1, MPI_INT, /*dest*/ 0, /*tag*/ 0, MPI_COMM_WORLD);
                    MPI_Send(&original_hot_potato, /*count*/ 1, MPI_INT, /*dest*/ 0, /*tag*/ 0, MPI_COMM_WORLD);
                }
            }
            else{
                if(hot_potato > 0){
                    if(number_processes_out == process_count - 1){
                        std::cout << "The process with rank "<< my_rank << " won."<< std::endl;
                        hot_potato = -1;    
                        for ( int destination = 0; destination < process_count; ++destination ){
                            if(destination != my_rank){
                                MPI_Send(&number_processes_out, 1, MPI_INT, destination, /*tag*/ 0, MPI_COMM_WORLD);
                                MPI_Send(&hot_potato, 1, MPI_INT, destination, /*tag*/ 0, MPI_COMM_WORLD);
                            }
                        }
                    }
                    else{
                        if ( my_rank < process_count - 1 ){
                            MPI_Send(&number_processes_out, /*count*/ 1, MPI_INT, /*dest*/ my_rank + 1, /*tag*/ 0, MPI_COMM_WORLD);
                            MPI_Send(&hot_potato, /*count*/ 1, MPI_INT, /*dest*/ my_rank + 1, /*tag*/ 0, MPI_COMM_WORLD);
                        }
                        else{
                            MPI_Send(&number_processes_out, /*count*/ 1, MPI_INT, /*dest*/ 0, /*tag*/ 0, MPI_COMM_WORLD);
                            MPI_Send(&hot_potato, /*count*/ 1, MPI_INT, /*dest*/ 0, /*tag*/ 0, MPI_COMM_WORLD);
                        }
                    }
                }
            }
        }
    }

	MPI_Finalize();
}