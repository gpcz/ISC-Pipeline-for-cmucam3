// IGVC Test Program
// Derived from main.c in the "hello-world" project.
//
// This program is for experimental/learning purposes in order to master the
// CC3 API.

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <cc3.h>

//#include "ISC_pipeline.h"
#include "ISC_util_assert.h"
//#include "ISC_process_convolution.h"
#include "ISC_out_histogram.h"
//#include "ISC_out_png.h"
#include "ISC_out_jpeg.h"
#include "ISC_in_cmucam.h"
//#include "ISC_util_rowqueue.h"
//#include "ISC_process_subsample.h"
//#include "ISC_process_convolution.h"
//#include "ISC_util_transfer.h"
#include "ISC_process_clamp_colorspace.h"
#include "ISC_process_tripler.h"

void EnterMainLoop( void );
void TestConvolution(void);
//void SendGraphic( void );

int main (void)
{
	#ifndef VIRTUAL_CAM
	int32_t val;
	#endif
	
  	// init filesystem driver
  	cc3_filesystem_init ();

  	// configure uarts
 	cc3_uart_init (0, CC3_UART_RATE_115200, CC3_UART_MODE_8N1, CC3_UART_BINMODE_BINARY);
  	// Make it so that stdout and stdin are not buffered
  	#ifndef VIRTUAL_CAM
  	val = setvbuf (stdout, NULL, _IONBF, 0);
	#endif
  	//val = setvbuf (stdin, NULL, _IONBF, 0);

  	cc3_camera_init ();

  	cc3_camera_set_colorspace (CC3_COLORSPACE_RGB);
  	cc3_camera_set_resolution (CC3_CAMERA_RESOLUTION_HIGH);
  	cc3_camera_set_auto_white_balance (true);
  	cc3_camera_set_auto_exposure (true);
	//cc3_pixbuf_frame_set_subsample(CC3_SUBSAMPLE_NEAREST,1,1);

  	printf ("Starting ISC_pipeline...\n");

  	cc3_led_set_state (0, false);
  	cc3_led_set_state (1, false);
  	cc3_led_set_state (2, false);

  	// sample wait command in ms
  	cc3_timer_wait_ms (1000);
  	cc3_led_set_state (0, true);
	cc3_camera_set_power_state (true);

	//DoFullBenchmark();
  	EnterMainLoop();

  	//while (1);

  	return 0;
}

void EnterMainLoop( void )
{
	#ifndef VIRTUAL_CAM
	char c;
	#endif
	//char test[2];
  	//cc3_image_t img;

	#ifndef VIRTUAL_CAM
    while ( 1 )
	{
		c = getchar();
	
		if ( c == 'S' )
		{
			c = getchar();
			if ( c == 'J' )
			{
				c = getchar();
				if ( c == '\r' )
				{
					printf ("ACK\r");
					TestConvolution( );
					printf ("JPG_END\r:");
				}
			}
		}
	}
	#else
	TestConvolution();
	#endif	
}

void TestConvolution( void )
{
    uint8_t *inRow/*, *transRow*/;
    ISC_out_histogram *ijc;
    ISC_in_cmucam *iic;
    ISC_util_imagecontext ic;
    ISC_out_jpeg *iop;
    //struct ISC_util_rowqueue *rq;
    //ISC_process_subsample *ips;
    //ISC_process_convolution_kernel convkern;
    //ISC_process_convolution *ipc, *ipc2;
	//ISC_process_clamp_colorspace *ipc;
	//ISC_process_tripler *ipc2;
    uint16_t count, count2;
    uint16_t temp;
    uint16_t max;
    FILE *fp;
    uint8_t *maxes;
    
    cc3_pixbuf_load();

/*convkern.matrix[0][0] =-2;convkern.matrix[0][1] =-1;convkern.matrix[0][2] = 0;
convkern.matrix[1][0] =-1;convkern.matrix[1][1] = 1;convkern.matrix[1][2] = 1;
convkern.matrix[2][0] = 0;convkern.matrix[2][1] = 1;convkern.matrix[2][2] = 2;
    
    convkern.kernelSize = 3;
    convkern.centerX = 1;
    convkern.centerY = 1;*/

    // Open the export image.
	#ifndef VIRTUAL_CAM
    //fp = fopen( "c:/new.jpeg", "w" );
	#else
	//fp = fopen( "new.jpeg", "w" );
	#endif

    // Get the current Image Context.
    ic = ISC_util_imagecontext_getfromcurrent(CC3_CAMERA_RESOLUTION_HIGH, CC3_COLORSPACE_RGB);
	//ic.frame.coi = CC3_CHANNEL_BLUE;

    // Start In-Cmucam (first module).
    iic = ISC_in_cmucam_start(ic);

    // Start Process-Subsample (next module).
    // Notice that it uses the context function of the previous module in
    // the pipeline as its input.  This is an important theme in ISC Pipeline.
    //ips = ISC_process_subsample_start( ISC_in_cmucam_context( iic ), 4, 4, 1 );
    
    //ipc = ISC_process_clamp_colorspace_start( ISC_in_cmucam_context(iic) );

	//ipc2 = ISC_process_tripler_start( ISC_process_clamp_colorspace_context( ipc ) );

    // Start Out-PPM (last module).
    // Notice that it uses the context function of the previous module in
    // the pipeline as its input.  This is an important theme in ISC Pipeline.
	printf( "ySize = %d", ic.frame.height );
    ijc = ISC_out_histogram_start( ISC_in_cmucam_context(iic), 16, 16, 4 );
    //iop = ISC_out_jpeg_start( ISC_in_cmucam_context( iic ), stdout );

    // The pipeline loop.
    while ( ISC_out_histogram_running(ijc) )
    {
	// In-Module: CMUCAM
        inRow = ISC_in_cmucam_process( iic );

		//ISC_process_clamp_colorspace_feed( ipc, inRow );
		//inRow = ISC_process_clamp_colorspace_process( ipc );

	/*if ( inRow )
	{
		for ( count = 0; count < ic.frame.width; count++ )
		{
			max = 0;
			for ( count2 = 0; count2 < 3; count2++ )
				if ( inRow[count*3+count2] > max )
					max = inRow[count*3+count2];
			for ( count2 = 0; count2 < 3; count2++ )
			{
				temp = (uint16_t)inRow[count*3+count2] * 255;
				temp /= max;
				inRow[count*3+count2] = (uint8_t)temp;
			}
		}
	}*/

		//ISC_process_tripler_feed( ipc2, inRow );
		//inRow = ISC_process_tripler_process( ipc2 );

		// Out-Module: PPM
	//ISC_out_jpeg_feed( iop, inRow );
		ISC_out_histogram_feed( ijc, inRow );
        if ( ijc->linesLeft == 0 )
		{
            //ISC_out_histogram_export_text( ijc, stdout );
	    	for ( count = 0; count < 16; count++ )
	    	{
		    	maxes = ISC_out_histogram_getmaxes( ijc, count );
		    	if ( maxes[0] < 3 && maxes[2] == 0 )
					printf( "G" );
		    	else if ( maxes[0] == 2 && maxes[1] == 2 && (maxes[2] == 1 || maxes[2] == 2) )
					printf( "S" );
		    	else if ( maxes[0] >= 2 && maxes[1] >= 2 && maxes[2] >= 2 )
					printf( "W" );
		    	else
					printf( "?" );
		    	free(maxes);
	    	}
	    	printf( "\r" );
		}
    }

    //ISC_out_jpeg_end(iop);
    ISC_out_histogram_end(ijc);
    ISC_in_cmucam_end( iic );
    //ISC_process_clamp_colorspace_end(ipc);
	//ISC_process_tripler_end(ipc2);
    
    //fclose(fp);
}

