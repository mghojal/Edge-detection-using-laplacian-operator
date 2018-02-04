# Edge-detection-using-laplacian-operator
This program can deal with png files, grayscale with depth 8bits. a test image is provided which is 6000*6000 pixels.

you can compile and execute the program by using Makefile

below are some details:
> to compile all versions (sequential, OpenMP, MPI):

	by:	 		make
> to execute:

	-Sequential by: 	make seq
	-OpenMP by: 		make omp
	-OpenMPI by: 		make openmpi
	-MPI by: 		make mpi
> to clean the results:

	by: 			make clean
> Notes: 
General: 
	
	-all programs using libpng
	-the programs doing 10 iterations for each to check the best results
	-in case you need to change iteration number you can go to header.h and change the value of ITERATION_NUMBER to whatever you want
	-to change input file you need to go to Makefile for sequential, openmp, and mpi to:
		* the line 7 for sequential version and change the name of the desired input image
		* line 10 for OpenMP version
	        * line 16 for mpi version
	- where to change input file in OpenMPI you need to open mpi.job and change input image to the desired one in the line 12
OpenMP:

	-to change threads number go the the file omp.c and change the variable THREAD_NUMBER
OpenMPI:
	
	-to change number of nodes by accessing mpi.job and change:	#SBATCH --nodes=<#No of desired nodes>
	-to change thread number by accessing mpi.job and change:	#SBATCH --cpus-per-task=<desired thread #No>
mpi:

	-to change number of nodes open Makefile in the line 9 you can redefine number of nodes



