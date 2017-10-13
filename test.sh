#!/bin/bash
# This file must be passed the directory of the svr project as its first arg
$1/server/svrd &
SVRD=$(echo $!)
$1/util/svrctl -o forward,v4l:dev=/dev/video0
RETURN_STATUS=$?
if [ "$RETURN_STATUS" = "0" ]
then
    kill "$SVRD"
    exit 0
else
    kill "$SVRD"
    exit "$RETURN_STATUS"
fi
