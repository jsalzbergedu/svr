#!/usr/bin/env python

## Commentary:
'''

'''


# Call svrd, wait for v4l to be opened,
# then start the executable passed to it as an arg.
# Finally, once that executable returns, kill svrd
# and exit with the status of the executable.
# Must be passed svrd as the first arg and the
# executable as the second

import sys
import subprocess

# This gets its imputs from the CMakeLists in the same directory.
# It must be called by python, which is given by the variable in CMakeLists 
# Then, as its next arg, you must feed it the location of svrd.


# Open the svrd process
svrd = subprocess.Popen([sys.argv[1], "-d"], stderr=subprocess.PIPE)
while True:
    # Read through the lines printed by svrd -d
    line = svrd.stderr.readline()
    # If the `[DEBUG][SVR] source_type 'v4l'` line is reached:
    if line.rstrip()[-4:][0:3] == 'v4l':
        # Then call the continuation, and store its return value in exit_status,
        exit_status = subprocess.call(sys.argv[2:])
        # do some cleanup,
        svrd.kill()
        # and finally return with exit_status.
        exit(exit_status)
