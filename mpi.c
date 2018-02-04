#include <stdlib.h>
#include <math.h>
#include <png.h>
#include <time.h>
#include <mpi.h>
#include "header.h"

#define MASTER 0

int main( int argc, char* argv[] )
{
	int rank, numtasks, x, y, i , p;
	double start,end,start1,end1;
	Image* img1 = image_load( argv[1] ); //input image, returned to output image				
	int size = img1->width;
	double min=1.0,max=0.0;

	//MPI intialization and calucalating size and rank of work space
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
	MPI_Status status;
	if (rank==MASTER){
		printf("+++++++++++++++++++++++++++\n\tMPI Version\t\n+++++++++++++++++++++++++++\n");
		printf("time spent in parallel code by %d iterations using %d nodes\n", ITERATION_NUM, numtasks);
	}
	Image* img2 = image_create(img1->width, img1->height/numtasks); //partially image from original and mid process
	Image* img3 = image_create(img1->width, img1->height/numtasks); //partially image from original and mid process	

	//scatter image among cluster, with broadcasting important parameters to be used in the calculations
	MPI_Scatter(&img1->data, (size*size)/(numtasks), MPI_UNSIGNED, img2->data, (size*size)/(numtasks), MPI_UNSIGNED, 0, MPI_COMM_WORLD);
	//MPI_Bcast(&min, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	//MPI_Bcast(&max, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	//MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	//MPI_Bcast(&numtasks, 1, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);

	//repeating process to extract best time
	for(i=1;i<=ITERATION_NUM;i++){
		if(rank==MASTER) 
			start=MPI_Wtime();

		stencilCode(img2,img3);

		if(rank==MASTER){ 
			end = MPI_Wtime();
			printf("Iteration: %d\t time: %f sec\n", i, end-start);
		}		
	}

	//gathering restults partially images from slaves to master 
	MPI_Gather(&img3->data,(size*size)/numtasks, MPI_UNSIGNED, img1->data, size*size/numtasks, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);

	//Master save the image to output file
	if (rank==MASTER){
		//start1=MPI_Wtime();		
		image_save(img1, "output_MPI.png" );
		//end1=MPI_Wtime();
		//printf("time spent in saving is: %f sec\n", end1-start1);		
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank==MASTER)
			printf("++++++++++++++++++++++++++++++++++\n\tEnd of MPI Version\t\n++++++++++++++++++++++++++++++++++\n\n\n");
	MPI_Finalize();

	return 0;
}
