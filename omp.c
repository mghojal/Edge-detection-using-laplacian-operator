#include <stdlib.h>
#include <math.h>
#include <png.h>
#include <omp.h>
#include "header.h"

#define THREAD_NUMBER 4

int main( int argc, char* argv[] )
{
	//double begin_p = omp_get_wtime();
	omp_set_num_threads(THREAD_NUMBER);
	Image* img1 = image_loadOMP( argv[1] );				//Input Image
	Image* img3 = image_create( img1->width, img1->height );	//Output Image
	printf("++++++++++++++++++++++++++++++\n\tOpenMP Version\t\n++++++++++++++++++++++++++++++\n");
	printf("time spent in parallel code in %d iterations using %d threads\n", ITERATION_NUM, THREAD_NUMBER);
	double avg_time=0;
	int i;

	//repeating process to extract best time	
	for ( i=1;i<=ITERATION_NUM;i++)
	{
		double begin_p1 = omp_get_wtime();
		 
		stencilCodeOMP( img1, img3 );
		double end_p1 = omp_get_wtime();
		double time_spent_p1 = end_p1 - begin_p1;
		avg_time += time_spent_p1;
		printf("iternation: %d\t time: %f sec\n",i,time_spent_p1);
	}
	
	//printf("++++++++++++++++++++++++++++++++++++\nAvg timing is: %f sec\n++++++++++++++++++++++++++++++++++++\n",avg_time/ITERATION_NUM);
	image_save( img3, "output_OMP.png" );

	//double end_p = omp_get_wtime();
	//double time_spent_p = (end_p - begin_p) - avg_time;
	//printf("time spent in saving is: %f sec\n", time_spent_p);
	printf("++++++++++++++++++++++++++++++++++++\n\tEnd of OpenMP Version\t\n++++++++++++++++++++++++++++++++++++\n\n\n");
	return 0;
}
