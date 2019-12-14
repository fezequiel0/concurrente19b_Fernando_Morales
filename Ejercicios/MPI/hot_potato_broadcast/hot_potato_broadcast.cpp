#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <omp.h>
#include <iomanip>
#include <math.h>

int main(int argc, char* argv[]){
	MPI_Init(&argc, &argv);
	
	int my_rank = -1;
	int processes_count = -1;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
	
	char hostname[MPI_MAX_PROCESSOR_NAME];
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);
	
	int hot_potato = 0;
	int original_hot_potato = 0;
	int processes_out=0;
	int initial_process=0;
	if ( argc >= 3 ){
		hot_potato= atoi(argv[1]);
		initial_process=atoi(argv[2]);
	}
	else{
		if(my_rank==0){
			std::cin >> hot_potato;
			std::cin >> initial_process;
		}
		MPI_Bcast(&hot_potato, 1, MPI_INT,0, MPI_COMM_WORLD);
		MPI_Bcast(&initial_process, 1, MPI_INT, 0, MPI_COMM_WORLD);
	}
	if(initial_process >= processes_count) {
		MPI_Finalize();
		return printf("You can't start in a non player.\n"), 2;
	}
	if(processes_count < 2) {
		MPI_Finalize();
		return printf("You can't play alone.\n"), 3;
	}

	int turn=initial_process;
	int im_out=0;
	original_hot_potato=hot_potato;
	int root=turn;
	
	while(processes_out<processes_count){
		root=turn;
		if(my_rank==turn){
			if(!im_out){	
				if(hot_potato%2==0){
					hot_potato=hot_potato/2;
				}
				else{
					hot_potato=3*hot_potato+1;
				}
				if(hot_potato==1){
					printf("Process %d lost.\n", my_rank);
					im_out=1;
					hot_potato=original_hot_potato;
					processes_out++;		
				}
				else{
					if(hot_potato<1){
						im_out=1;
					}
				}
			}

			if(processes_out==processes_count-1 && !im_out){
				printf("Process %d is the winner.\n", my_rank);
				im_out=1;
				hot_potato=0;
				processes_out+=2;
			}

			if(turn==processes_count-1){
				turn=0;
			}
			else{
				turn++;
			}
		}
		if(processes_out==processes_count && im_out){
			--processes_out;
		}
		MPI_Bcast(&hot_potato, 1, MPI_INT,root, MPI_COMM_WORLD);
		MPI_Bcast(&processes_out, 1, MPI_INT,root, MPI_COMM_WORLD);
		MPI_Bcast(&turn, 1, MPI_INT,root, MPI_COMM_WORLD);
	}
				
	MPI_Finalize();
}
