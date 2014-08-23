#include "cv.h"
#include <highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>

// User's image mask (not an actual CvArr* mask)
IplImage* mask; 

static CvMemStorage* storage = 0;

static CvHaarClassifierCascade* cascade = 0;

void detect_and_draw( IplImage* image );

const char* cascade_name = "haarcascade_frontalface_default.xml";

// Function to detect and draw any faces that is present in an image
void detect_and_draw( IplImage* img )
{
	int scale = 1;
	// Create a new image based on frame image (img)
	IplImage* temp =
		cvCreateImage(cvSize(img->width/scale,img->height/scale),8,3);	
	
	// Two points to represent the face locations
	CvPoint pt1,pt2;
	 int i;
		
	 // Clear the memory storage used before
	cvClearMemStorage( storage );
	
	if( cascade )
	{
		// Detect the objects and store them in the sequence
		CvSeq* faces = cvHaarDetectObjects( img, cascade,storage,1.1,
				2,CV_HAAR_DO_CANNY_PRUNING,cvSize(20,20),cvSize(40, 40) );
		// Loop the number of faces found.
		for( i = 0; i < (faces ? faces->total : 0); i++ )
		{
			// Create a new rectangle for drawing face
			CvRect* r = (CvRect*)cvGetSeqElem(faces, i );
	
			// Find the dimensions of the face and scale if needed
			pt1.x = r->x*scale;
			pt2.x = (r->x+r->width)*scale;
			pt1.y = r->y*scale;
			pt2.y = (r->y+r->height)*scale;
						
			int width = pt2.x-pt1.x;
			int height = pt2.y-pt1.y;
			width = width + 1;
			height = height + 1;

			// Resize mask to fit face detection
			IplImage* t = cvCreateImage(cvSize(width,height),8,3);
			cvResize(mask,t,CV_INTER_LINEAR);
			CvSize sz = cvGetSize(t);

			cvSetImageROI(img,cvRect(pt1.x,pt1.y,width,height));
			cvCopy(t,img,NULL);
			
			cvResetImageROI(img);
		}
	}
		 
			cvShowImage( "Make-A-Mask", img );
		
			cvReleaseImage( &temp );
}



int main( int argc, char** argv ){
	// 1st argument is video, 2nd argument is mask pic
	IplImage *src;
	src = cvLoadImage(argv[2],CV_LOAD_IMAGE_COLOR);

	// Crop mask pic according to user's dimensions
	int usrW;
	int usrH;
	printf ("Enter width to crop mask or enter 0 to load mask as is: ");
	scanf ("%d", &usrW);
	if (usrW != 0){
		printf ("Enter height to crop mask: ");
		scanf ("%d", &usrH);

		cvSetImageROI(src, cvRect((src->width - usrW)/2, (src->height -
					usrH)/2,usrW,usrH));
  	
		mask = cvCreateImage(cvGetSize(src),src->depth, src->nChannels);
		cvCopy(src,mask,NULL);
		cvResetImageROI(src);
	}
	else{
		mask = cvLoadImage(argv[2],CV_LOAD_IMAGE_COLOR);
	}
		

	// For getting video from camera or avi
		 CvCapture* capture = 0;

		 // Images to capture the frame from video or camera or from file
		 IplImage *frame, *frame_copy = 0;
		
		// Used for calculations
		int optlen = strlen("--cascade=");

		 // Load the HaarClassifierCascade for face detection
		 cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name,0,0,0 );
		 
		if( !cascade )
		{
			fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
			 return -1;
		}
		  
		storage = cvCreateMemStorage(0);

		const char* input_name;
		input_name = argv[1];

		// avi video file
			// Change AVI to CAM in cvCaptureFrom if camera capture desired
		capture = cvCaptureFromAVI(input_name);

		cvNamedWindow( "Make-A-Mask", 1 );

		 if( capture )
		{
		
			for(;;)
			{
				// Capture the frame and load it in IplImage
					if( !cvGrabFrame( capture )){
						break;
					}
					frame = cvRetrieveFrame( capture,0);
					
					
					if( !frame ){
						printf("no frame exists");
						break;
					}
					
				  	// Allocate framecopy as the same size of the frame
					if( !frame_copy )
						frame_copy = cvCreateImage(cvSize(frame->width,frame->height),IPL_DEPTH_8U, frame->nChannels );
					// Check the origin of image.
						//If top left, copy the image frame to frame_copy. 
					if( frame->origin == IPL_ORIGIN_TL )
						cvCopy( frame, frame_copy,0 );
						// Else flip and copy the image
					else
						cvFlip( frame, frame_copy,0 );

					// Detect and draw mask on top of face
					detect_and_draw( frame_copy );
					
					// Wait time between frames
					if( cvWaitKey( 1 ) >= 0 )
				      break;
					}
			// Release the images and capture memory
			   cvReleaseImage( &frame_copy );
		      cvReleaseCapture( &capture );
	}
	 
		 cvDestroyWindow("Make-A-Mask");
		 cvReleaseHaarClassifierCascade(&cascade); 
			return 0;
}
