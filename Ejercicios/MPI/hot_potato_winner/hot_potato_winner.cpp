#include <bits/stdc++.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <omp.h>

#define MAX_HOSTNAME_LENGTH 1024

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
	if(!sscanf(argv[1],"%d", &hot_potato)) {
		if(my_rank == 0) {
			MPI_Finalize();
			return printf("Error in arguments.\nUse ./hot_patato_loser patatoValue\n"), 1;
		}
	}

	if(total_processes < 2) {
		MPI_Finalize();
		return printf("You can't play alone.\n"), 2;
	}

	int current = 0;
	int processes_still_playing = total_processes - 1;
	bool continuePlaying = true;
	
	while(processes_still_playing >= 1) {
		
		if(current != 0 || my_rank != 0) {
			int prev = my_rank -  1;
			if (prev < 0) {
				prev = total_processes - 1;
			}
			MPI_Recv(&hot_potato, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&processes_still_playing, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}	
		++current;
		if(continuePlaying) {
			--hot_potato;
			if(hot_potato == 0) {
				--processes_still_playing;
				continuePlaying = false;
				int place = total_processes - processes_still_playing - 1;
				printf("Process %d lost in place %d.\n", my_rank, place);
				sscanf(argv[1], "%d", &hot_potato);
			}
		}
		if(players >= 0) {
			int next = (my_rank + 1) % total_processes;
			MPI_Send(&hot_potato, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
			MPI_Send(&processes_still_playing, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
		}
			
	}

	if(continuePlaying) {
		printf("Process %d is the winner.\n", my_rank);
	}

	MPI_Finalize();

	return 0;
}
