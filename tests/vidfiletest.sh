#!/bin/bash

## Getting the testfile.avi 
"$1"/util/svrctl -o forward,v4l:dev=/dev/video0
python "$1"/tests/vidfiletest.py

## Testing the testfile.avi
if [ -f "$1"/tests/testfile.avi ]
then
    exit 0
else
    echo "testfile.avi does not exist"
    exit 1
fi
