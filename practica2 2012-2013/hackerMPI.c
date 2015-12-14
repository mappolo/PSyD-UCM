#include <stdlib.h>
#include <stdio.h>
#include "hackerMachine.h"

#define TAG 1
#define DEBUG 0

//#define MAXCOLS 3
//#define MAXROWS 3
//#define NUMCORES 3



int main(int argc, char* argv[]) {

/*************************************************************************
 * *DINAMICO
 *************************************************************************/

	int size, rank;
	int res[MAX_ROWS][MAX_COLS];
	int bitMap[MAX_COLS];
	MPI_Status status;


	int i,j;
	for(i = 0; i < MAX_ROWS; i++){
		for(j = 0; j < MAX_COLS; j++){
			res[i][j] = 0;
		}
	}

	for(i = 0; i < MAX_COLS; i++){
		bitMap[i] = 0;
	}
	// Init
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	initSystem(rank);

	int it = MAX_ROWS/(size-1);

	int num;
	for(num = 0;num<it;num++){
		int row = num*(size-1);
		if(rank == 0)
		{

			for(i = 1;i <= size-1; i++){
				MPI_Send(res[(row+i)-1],MAX_COLS,MPI_INT,i,1,MPI_COMM_WORLD);
			}
			for(i = 1;i <= size-1; i++){
				MPI_Recv(&res[(row+i)-1],MAX_COLS,MPI_INT,i,1,MPI_COMM_WORLD,&status);
			}

		}
		else{
			int ok[MAX_COLS];
			for(i = 0; i < MAX_COLS; i++){
				bitMap[i] = 0;
				ok[i] = 1;
			}
			MPI_Recv(&bitMap,MAX_COLS,MPI_INT,0,1,MPI_COMM_WORLD,&status);


			int fin = 0;

			int end = 0,aux_ok = 1;
				//int i;
			while(end == 0){
				compareMatrixRow(bitMap,ok,(row+rank)-1);
				aux_ok = 1;
				for(i = 0;i<MAX_COLS;i++){
					if(ok[i] == 1){
						bitMap[i]++;
						aux_ok = 0;
					}

				}

				if(aux_ok == 1){
					end = 1;
				}
			}

			MPI_Send(bitMap,MAX_COLS,MPI_INT,0,1,MPI_COMM_WORLD);
		}
	}


	int last = MAX_ROWS - ((size-1) * it);
	if(last > 0){
		int row = it*(size-1);
		if(rank == 0)
		{

			for(i = 1;i <= last; i++){
				MPI_Send(res[(row+i)-1],MAX_COLS,MPI_INT,i,1,MPI_COMM_WORLD);
			}
			for(i = 1;i <= last; i++){
				MPI_Recv(&res[(row+i)-1],MAX_COLS,MPI_INT,i,1,MPI_COMM_WORLD,&status);
			}
		}
		else if(rank <= last){
			MPI_Recv(&bitMap,MAX_COLS,MPI_INT,0,1,MPI_COMM_WORLD,&status);
			int ok[MAX_COLS];


			int fin = 0;

			int end = 0,aux_ok = 1;
				//int i;
			while(end == 0){
				compareMatrixRow(bitMap,ok,(row+rank)-1);
				aux_ok = 1;
				for(i = 0;i<MAX_COLS;i++){
					if(ok[i] == 1){
						bitMap[i]++;
						aux_ok = 0;
					}

				}

				if(aux_ok == 1){
					end = 1;
				}
			}

			MPI_Send(bitMap,MAX_COLS,MPI_INT,0,1,MPI_COMM_WORLD);
		}
	}
	if(rank == 0){
		printMatrix(res);
		int finish_him = checkCurrentKey(res);


		if(finish_him == 1){
			printf("Conseguido, DEAL WITH IT\n");
		}
		else{
			printf("FAIL\n");
		}
	}
	MPI_Finalize();
	exit(0);

/*************************************************************************
 * *ESTATICO
 *************************************************************************/
/*
	int size, rank;
	int data[MAX_COLS];
	int ok[MAX_COLS];
	int res[MAX_ROWS][MAX_COLS];

	int i,j = 0;
	for(i = 0; i < MAX_ROWS;i++){
		for(j=0;j < MAX_COLS; j++){
			res[i][j] = 0;
		}
		ok[i] = 1;
		data[i] = 0;
	}
	// Init
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	initSystem(rank);

	int block = MAX_ROWS/size;
	int frag_res[size][MAX_COLS];
	int num_data_send = size;
	int num_data_recive = MAX_COLS;


	int num;
	for(num = 0; num < block;num++){

		for(i = 0; i < size;i++){
			for(j=0;j < MAX_COLS; j++){
				frag_res[i][j] = 0;
			}
		}

		MPI_Scatter(frag_res,num_data_recive,MPI_INT,&data,num_data_recive,MPI_INT,0,MPI_COMM_WORLD);

		int end = 0,aux_ok = 1;

		while(end == 0){
			compareMatrixRow(data,ok,(num*size)+rank);
			aux_ok = 1;
			for(i = 0;i<MAX_COLS;i++){
				if(ok[i] == 1){
					data[i]++;
					aux_ok = 0;
				}

			}

			if(aux_ok == 1){
				end = 1;
			}
		}



		MPI_Gather(data,num_data_recive,MPI_INT,&frag_res,num_data_recive,MPI_INT,0,MPI_COMM_WORLD);


		if(rank == 0){
			int aux_i = 0;
			for(i = num*size; i < (num*size)+size;i++){
				for(j=0;j < MAX_COLS; j++){
					res[i][j] = frag_res[aux_i][j];
				}
				aux_i++;
			}
		}

	}


	if(rank == 0){

		int finish_him = checkCurrentKey(res);
		printMatrix(res);

		if(finish_him == 1){
			printf("Conseguido, DEAL WITH IT\n");
		}
		else{
			printf("FAIL\n");
		}
	}
	MPI_Finalize();
	exit(0);
*/

}



