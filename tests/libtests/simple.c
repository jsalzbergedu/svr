#include <svr.h>
#include <stdio.h>

int main(void) {
  SVR_Stream* stream;
  IplImage* frame;

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
    /* Here we set the encoding of the stream to "raw" */
    SVR_Stream_setEncoding(stream, "raw");
    /* Streams start in a pause state, and things like encoding and size can only
       be changed when a stream is paused */
    SVR_Stream_unpause(stream);
    for(int i = 0; i < 30; i++) {
      /* Retreive a frame. The second arguments will make the call block until a
	 frame becomes available */
      frame = SVR_Stream_getFrame(stream, true);
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
    return 0;
  }
}
