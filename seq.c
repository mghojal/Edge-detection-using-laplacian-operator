//======================================================================================================//
// Course	: Scalable Computing									//	
// Dozent	: Prof. Dr. Peter Luksch								//
// Tutor	: Meisam Booshehri									//
// Project Name : Edge detection by applying stencil code on an image					//
// Group Members: Mohammad Ghojal			216205052					//
//		  Göksu Kara				216205943					//	 	 
// Description  : Task-1 Sequential_Version								//
// WS17-18 Rostock Universität										//
// Notes: 	1- 	Input image should be gray scale						//
//		2- 	Below are the steps using gcc compiler						//
//			$ gcc <fileName.c> -lpng -o <excutionNameFile>					//
//			$ ./<excutionNameFile> <nameOfTestImage.png>					//
//		3-	Output image is output_seq.png saved in the same directory			//
//======================================================================================================//

#include <stdlib.h>
#include <math.h>
#include <png.h>
#include <time.h>
#include "header.h"


int main( int argc, char* argv[] )
{
	//clock_t begin = clock();
	Image* img1 = image_load( argv[1] );				//Input Image
	Image* img3 = image_create( img1->width, img1->height );	//Output Image
	printf("++++++++++++++++++++++++++++++++++\n\tSequential Version\t\n++++++++++++++++++++++++++++++++++\n");	
	printf("time spent in sequential code with %d iterations\n", ITERATION_NUM);
	double avg_time = 0;
	int i;

	//repeating process to extract best time
	for ( i = 1; i<=ITERATION_NUM; i++)
	{
		clock_t begin1 = clock();
	 
		stencilCode( img1, img3 );
		clock_t end = clock();
		double time_spent = (double)(end - begin1) / CLOCKS_PER_SEC;
		avg_time += (time_spent*CLOCKS_PER_SEC);
		printf("iteration: %d \t time: %f sec\n",i, time_spent);
	}
	//printf("++++++++++++++++++++++++++++++++++++\nAvg timing is: %f sec\n++++++++++++++++++++++++++++++++++++\n",(avg_time/CLOCKS_PER_SEC)/ITERATION_NUM);
	image_save( img3, "output_seq.png" );

	//clock_t end1 = clock();
	//double time_spent1 = (double)(end1 - avg_time) / CLOCKS_PER_SEC;
	//printf("time spent in saving is: %f sec\n", time_spent1);
	printf("++++++++++++++++++++++++++++++++++++\n\tEnd of Sequential Version\t\n++++++++++++++++++++++++++++++++++++\n\n\n");
	return 0;
}
