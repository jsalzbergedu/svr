#include <svr.h>
#include <stdio.h>

int main(void) {
  SVR_Stream* stream;

  /* Testing SVR_Stream_resize() */
  SVR_init();

  stream = SVR_Stream_new("forward");

  if (!stream) {
    printf("Stream forward not initialized. \n");
    exit(EXIT_FAILURE);
  } else {
    if(SVR_Stream_resize(stream, 200, 200)) {
      printf("Failed to resize stream.");
      exit(EXIT_FAILURE);
    } else {
      return 0;
    }
  }
}
