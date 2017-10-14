#!/usr/bin/env python
# Calls svrd, waits for v4l to be opened,
# then starts the executable passed to it as an arg.
# Must be passed svrd as the first arg and the
# executable as the second
 
import sys
print(sys.argv[1])
import subprocess

svrd = subprocess.Popen([sys.argv[1], "-d"], stderr=subprocess.PIPE)
while True:
    line = svrd.stderr.readline()
    if line.rstrip()[-4:][0:3] == 'v4l':
        print "sucess"
        exit_status = subprocess.call(sys.argv[2:])
        svrd.kill()
        exit(exit_status)
