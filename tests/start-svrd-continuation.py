#!/usr/bin/env python
# Call svrd, wait for v4l to be opened,
# then start the executable passed to it as an arg.
# Finally, once that executable returns, kill svrd
# and exit with the status of the executable.
# Must be passed svrd as the first arg and the
# executable as the second
 
import sys
import subprocess

svrd = subprocess.Popen([sys.argv[1], "-d"], stderr=subprocess.PIPE)
while True:
    line = svrd.stderr.readline()
    if line.rstrip()[-4:][0:3] == 'v4l':
        exit_status = subprocess.call(sys.argv[2:])
        svrd.kill()
        exit(exit_status)
