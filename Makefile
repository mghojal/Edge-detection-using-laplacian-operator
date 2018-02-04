all:
	gcc seq.c -lpng -o seq.o
	gcc omp.c -lpng -fopenmp -o omp.o
	mpicc mpi.c -lpng -o mpi.o

seq:
	./seq.o test-image.png

omp:
	./omp.o test-image.png 

openmpi:
	sbatch mpi.job

mpi:	
	mpirun -np 10 ./mpi.o test-image.png

clean: 
	rm *.o
	rm output_*
	rm slurm*.out
