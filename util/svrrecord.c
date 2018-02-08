#include <svr.h>
#include "opencv2/highgui/highgui_c.h"
#define XVID_FOURCC 1145656920

int main(void) {
  SVR_Stream* stream;
  IplImage* frame;
  CvVideoWriter *writer;

  /* This call must execute successfully before any streams can be opened. It is
     responsible for opening a connection to the server and initializing the
     library */
  SVR_init();

  /* This call opens a stream associated with the given source */
  // stream = SVR_Stream(cam0); /* had to remove the "" */
  /* This stream must be null-checked. It has to be started by svrctl to work */
  stream = SVR_Stream_new("forward");
  if (!stream) {
    printf("Stream forward not initialized. \n");
    exit(EXIT_FAILURE);
  } else {

  /* CVAPI cvCreatevideowriter(const char* filename, int fourcc, double fps, CvSize frame_size,
     int is_color CV_DEFAULT(1)) */
    writer = cvCreateVideoWriter("test.avi", XVID_FOURCC, 30, cvSize(640, 480), 1);

    /* Here we set the encoding of the stream to "raw" */
    SVR_Stream_setEncoding(stream, "raw");
    /* Streams start in a pause state, and things like encoding and size can only
       be changed when a stream is paused */
    SVR_Stream_unpause(stream);
    while((cvWaitKey(10) & 0xFF) != 'q') {
      /* Retreive a frame. The second arguments will make the call block until a
	 frame becomes available */
      frame = SVR_Stream_getFrame(stream, true);
      cvWriteFrame(writer, frame);
      if(frame) {
	/* Process the frame... */
	
	/* Once we're done with the frame we return it (do not free it directly) */
	SVR_Stream_returnFrame(stream, frame);
	
      } else if(SVR_Stream_isOrphaned(stream)) {
	/* This indicates that the source (cam0) has been closed */
	fprintf(stderr, "Stream orphaned\n");
	break;
      }
    }
    cvReleaseVideoWriter( &writer );
    return 0;
  }
}
