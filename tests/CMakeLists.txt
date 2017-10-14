cmake_minimum_required (VERSION 3.1)
project( tests )

# This test just checks whether svrctl can open v4l
add_test( NAME sanity_check
  COMMAND bash test.sh ${TOPLEVEL_DIR} )

# This test will only pass if both an avi file is recorded
# and ffmpeg confirms it is free of errors.
add_test( NAME record_file
  COMMAND bash vidfiletest.sh ${TOPLEVEL_DIR} )
