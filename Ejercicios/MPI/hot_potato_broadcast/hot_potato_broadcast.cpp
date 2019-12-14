#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <omp.h>
#include <bits/stdc++.h>

#define MAX_HOSTNAME_LENGTH 1024

bool ive_won(bool processes_active[], int length) {
	bool result = false;
	int counter = 0;
	for (int i = 0; i < length; ++i) {
		if(processes_active[i] == true){
			++counter;
		}
	}
	if(counter){
		result = true;	
	}
	return result;
}

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);

	int my_rank = -1; 
	int total_processes = -1;

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &total_processes);

	char hostname[MAX_HOSTNAME_LENGTH];
	int hostname_length = -1;
	MPI_Get_processor_name(hostname, &hostname_length);

	int hot_potato = 0;
	int start_player = 0;
	if(!sscanf(argv[1],"%d", &hot_potato) || !sscanf(argv[2], "%d", &start_player)){
		if(my_rank == 0) {
			MPI_Finalize();
			return printf("Error in arguments.\nUse ./hot_potato_loser potato_value start_player\n"), 1;
		}
	}

	if(start_player >= total_processes) {
		MPI_Finalize();
		return printf("You can't start in a non player.\n"), 2;
	}

	if(total_processes < 2) {
		MPI_Finalize();
		return printf("You can't play alone.\n"), 3;
	}

	int current = 0;
	bool processes_still_playing[total_processes];
	for (int i = 0; i < total_processes; ++i) {
		processes_still_playing[i] = true;
	} 
	
	while(processes_still_playing[my_rank]) {
		if(current != 0 || my_rank != start_player) {
			int prev = my_rank;
			while (!processes_still_playing[prev]){
				--prev;
				if (prev < 0){
					prev = total_processes - 1;
				}
			} 
			MPI_Recv(&potato, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&processes_still_playing, total_processes, MPI_C_BOOL, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		if(ive_won(processes_still_playing, total_processes)) {
			printf("Process %d is the winner.\n", my_rank);
			MPI_Finalize();
		} 
		else {
			++current;
			if(hot_potato % 2 == 1){ 
				hot_potato = 3 * hot_potato + 1;
			}
			else{ 
				hot_potato /= 2;
			}
			if(hot_potato == 1) {
				processes_still_playing[my_rank] = false;
				printf("Process %d lose.\n", my_rank);
				sscanf(argv[1], "%d", &hot_potato);
			}
			int next = my_rank;
			while(processes_still_playing[next] == false){
				next = (next + 1) % total_processes;
			} 
			MPI_Send(&hot_potato, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
			MPI_Send(&processes_still_playing, total_processes, MPI_C_BOOL, next, 0, MPI_COMM_WORLD);
		}
	}

	return 0;
}
