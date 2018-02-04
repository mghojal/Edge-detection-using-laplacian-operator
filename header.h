// describes an image. The image is the width, height, followed by pixel data.
typedef struct _Image {
    unsigned width;
    unsigned height;
    unsigned data[1]; 
} Image;

#define MAX_BRIGHTNESS 255
#define GetValue(a) ((a)&0xff)
#define ITERATION_NUM 10

Image* image_create( unsigned width, unsigned height );
void image_destroy( Image* image );
Image* image_load( const char *file_name );
int image_save( Image* image, const char* filename );
unsigned image_get_pixel( Image* image, int x, int y );
double image_get_pixeld( Image* image, int x, int y );
void image_set_pixel( Image* image, int x, int y, unsigned c );
void stencilCode(Image* in, Image* out);
void stencilCodeOMP( Image* in, Image* out );
Image* image_loadOMP( const char *file_name );


void stencilCode( Image* in, Image* out ) {
	double* buffer = (double*)calloc(in->width*in->height,sizeof(double));
	double min = 1.0, max = 0.0;
	    
	int x,y;
	// for each row, column, calculating the new value using Stencil Matrix (laplacian)
	for ( y = 1; y < in->height-2; y++ ) {
		for( x = 1; x < in->width-2; x++ ) {
			double val = abs(
			8 * image_get_pixeld(in, x, y)  -(
			image_get_pixeld(in, x-1, y-1 ) +
			image_get_pixeld(in, x  , y-1 ) +
			image_get_pixeld(in, x+1, y-1 ) +
			image_get_pixeld(in, x-1, y   ) +
			image_get_pixeld(in, x+1, y   ) +
			image_get_pixeld(in, x-1, y+1 ) +
			image_get_pixeld(in, x  , y+1 ) +
			image_get_pixeld(in, x+1, y+1 )));
	
			buffer[y*in->width+x] = val;
			if ( val > max ) max = val;
			if ( val < min ) min = val;
		}
	}

	//Normailzing the values and set pixel value in the proper location
	for ( y = 0; y < in->height; y++ ) {
		for( x = 0; x < in->width; x++ ) {
			double val = MAX_BRIGHTNESS * (buffer[y*in->width+x] - min) / (max-min);
			image_set_pixel( out, x, y, val );
		}
	}

	free( buffer );
}

//StencilCode function to be applied on input image and set values to output image 
void stencilCodeOMP( Image* in, Image* out ) {
	double* buffer = (double*)calloc(in->width*in->height,sizeof(double));
	double min = 1.0, max = 0.0;
	    
	int x,y;
	// for each row, column, calculating the new value using Stencil Matrix (laplacian)
	#pragma omp parallel for collapse(2)
	for ( y = 1; y < in->height-2; y++ ) {
		for( x = 1; x < in->width-2; x++ ) {
			double val = abs(
			8 * image_get_pixeld(in, x, y)  -(
			image_get_pixeld(in, x-1, y-1 ) +
			image_get_pixeld(in, x  , y-1 ) +
			image_get_pixeld(in, x+1, y-1 ) +
			image_get_pixeld(in, x-1, y   ) +
			image_get_pixeld(in, x+1, y   ) +
			image_get_pixeld(in, x-1, y+1 ) +
			image_get_pixeld(in, x  , y+1 ) +
			image_get_pixeld(in, x+1, y+1 )));
	
			buffer[y*in->width+x] = val;
			//#pragma omp critical
			{
				if ( val > max ) max = val;
				if ( val < min ) min = val;
			}		
		}
	}

	//Normailzing the values and set pixel value in the proper location
	#pragma omp parallel for collapse(2)
	for ( y = 0; y < in->height; y++ ) {
		for( x = 0; x < in->width; x++ ) {
			double val = MAX_BRIGHTNESS * (buffer[y*in->width+x] - min) / (max-min);
			image_set_pixel( out, x, y, val );
		}
	}

	free( buffer );
}

//creat empty image
Image* image_create( unsigned width, unsigned height ) {
	unsigned size = width * height * sizeof(unsigned);
	Image* image = (Image*)malloc(sizeof(unsigned)+size);
	memset( image->data, 0, size );
	image->width = width;
	image->height = height;
	return image;
}

//empty memory occupied by image
void image_destroy( Image* image ) {
	free( image );
}

// retrieve the pixel value.
unsigned image_get_pixel( Image* image, int x, int y ) {
	if ( x < 0 || x >= image->width || y < 0 || y >= image->height ) return 0;
	return image->data[y*image->width+x];
}

// retrieve a particular value as a double
double image_get_pixeld( Image* image, int x, int y ) {
	if ( x < 0 || x >= image->width || y < 0 || y >= image->height ) return 0.;
	return (double)GetValue(image->data[y*image->width+x]);
}

// set a pixel value.
void image_set_pixel( Image* image, int x, int y, unsigned clr ) {
	if ( x < 0 || x >= image->width || y < 0 || y >= image->height ) return;
	image->data[y*image->width+x] = clr;
}

// load a new image from png file.
Image* image_load( const char *file_name ) {
	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 width, height;
	FILE *fp;
	Image* image = NULL;
	unsigned x,y;

	if ((fp = fopen(file_name, "rb")) == NULL)
		return 0;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (png_ptr == NULL) {
		fclose(fp);
		return 0;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return 0;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		fclose(fp);
		return 0;
	}

	png_init_io(png_ptr, fp);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_ALPHA, NULL);

	png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	width = png_get_image_width(png_ptr, info_ptr);
	image = image_create( width, height );

	for ( y = 0; y < height; y++ ) {
		for( x = 0; x < width; x++ ) {
			unsigned c = 0;
			unsigned char* ch = (unsigned char*)&c;
			unsigned char* array = row_pointers[y];

			ch[0] = array[x];
			image_set_pixel(image, x, y, c);
		}
	}
	   
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
	fclose(fp);
	return image;
}

// load a new image from png file.
Image* image_loadOMP( const char *file_name ) {
	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 width, height;
	FILE *fp;
	Image* image = NULL;
	unsigned x,y;

	if ((fp = fopen(file_name, "rb")) == NULL)
		return 0;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (png_ptr == NULL) {
		fclose(fp);
		return 0;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return 0;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		fclose(fp);
		return 0;
	}

	png_init_io(png_ptr, fp);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_ALPHA, NULL);

	png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	width = png_get_image_width(png_ptr, info_ptr);
	image = image_create( width, height );

	#pragma omp parallel for collapse(2)
	for ( y = 0; y < height; y++ ) {
		for( x = 0; x < width; x++ ) {
			unsigned c = 0;
			unsigned char* ch = (unsigned char*)&c;
			unsigned char* array = row_pointers[y];

			ch[0] = array[x];
			image_set_pixel(image, x, y, c);
		}
	}
	   
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
	fclose(fp);
	return image;
}

// save png file.
int image_save( Image* image, const char* filename ) {
	png_structp png_ptr = 0;
	png_infop png_info = 0;
	png_bytep row_ptr = 0;
	int x,y;
	int error = 1;

	FILE* file = fopen(filename, "wb");
	if ( file == 0 ) {
		return 0;
	}

	row_ptr = (png_bytep)malloc(image->width);
	png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);

	if ( png_ptr == 0 ) {
		goto cleanup;
	}

	png_info = png_create_info_struct(png_ptr);
	if ( png_info == 0 ) {
		goto cleanup;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		goto cleanup;
	}

	png_init_io( png_ptr, file );

	png_set_IHDR(png_ptr, png_info, image->width, image->height, 
		8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	png_write_info(png_ptr, png_info);

	for ( y = 0; y < image->height; y++ ) {
		for( x = 0; x < image->width; x++ ) {
			unsigned clr = image_get_pixel( image, x, y );
			row_ptr[x] = ( clr );
		}
		png_write_row(png_ptr, row_ptr);
	}
	png_write_end(png_ptr, png_info);
	error = 0;
	cleanup:
	fclose(file);
	if ( png_ptr ) { 
		png_destroy_write_struct(&png_ptr, &png_info);
	}    
	free( row_ptr );
	return !error;
}

