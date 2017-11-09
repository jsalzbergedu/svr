cmake_minimum_required (VERSION 3.1)
project( tests )

set(Python_ADDITIONAL_VERSIONS 2.7 2.6 2.5)
find_package(PythonInterp REQUIRED)
set(Python_ADDITIONAL_VERSIONS $(PYTHON_VERSION_STRING))
find_program(PYTHON "python")

add_subdirectory( libtests )

# This test just checks whether svrctl can open v4l
# if you are confused by the syntax of the command,
# see start-svrd-continuation.py
add_test( NAME sanity_check
  COMMAND ${PYTHON}
  ${CMAKE_CURRENT_SOURCE_DIR}/start-svrd-continuation.py
  ${TOPLEVEL_DIR}/server/svrd
  bash
  ${CMAKE_CURRENT_SOURCE_DIR}/test.sh
  ${TOPLEVEL_DIR} )


# This test will only pass if both an avi file is recorded
# and ffmpeg confirms it is free of errors.
add_test( NAME record_file
  COMMAND ${PYTHON}
  ${CMAKE_CURRENT_SOURCE_DIR}/start-svrd-continuation.py
  ${TOPLEVEL_DIR}/server/svrd
  bash
  ${CMAKE_CURRENT_SOURCE_DIR}/vidfiletest.sh
  ${TOPLEVEL_DIR} )
