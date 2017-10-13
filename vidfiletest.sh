#!/bin/bash
### This file must be passed the svr toplevel directory as its first arg

## Getting the testfile.avi 
$1/server/svrd &
SVRD=$(echo $!)
$1/util/svrctl -o forward,v4l:dev=/dev/video0
python $1/vidfiletest.py
kill "$SVRD"

## Testing the testfile.avi
if [ -f $1/testfile.avi ]
then
    # Checking the integrity of the video file.
    # If there is no error, the following variable will be ""
    ERRORP=$(ffmpeg -v error -i $1/testfile.avi -f null - 2>&1)

    # Now that all relevant information has been extracted, the testfile.avi can be removed
    rm $1/testfile.avi

    if [ "x$ERRORP" = "x" ]
    then
	exit 0
    else
	echo "Video integrity check failed"
	exit 1
    fi
else
    echo "testfile.avi does not exist"
    exit 1
fi