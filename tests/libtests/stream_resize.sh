#!/bin/bash
# Must be passed the toplevel directory as the first arg
"$1"/util/svrctl -o forward,v4l:dev=/dev/video0
"$1"/tests/libtests/stream_resize
