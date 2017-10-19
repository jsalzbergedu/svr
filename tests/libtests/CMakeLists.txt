cmake_minimum_required (VERSION 3.1)
project ( libtests )

find_package( OpenCV REQUIRED )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenCV_CFLAGS} -g -Wall -Werror" )
include_directories ( ${OpenCV_INCLUDE_DIRS} ../../include )

add_executable( simple simple.c )
add_dependencies( simple svr )
target_link_libraries( simple svr seawolf ${OpenCV_LIBRARIES} )

add_executable( stream_resize stream_resize.c )
add_dependencies( stream_resize svr )
target_link_libraries( stream_resize svr seawolf ${OpenCV_LIBRARIES} )

add_test( NAME simple
  COMMAND ${PYTHON}
  ${TOPLEVEL_DIR}/tests/start-svrd-continuation.py
  ${TOPLEVEL_DIR}/server/svrd
  bash
  ${CMAKE_CURRENT_SOURCE_DIR}/simple.sh
  ${TOPLEVEL_DIR} )

add_test( NAME stream_resize
  COMMAND ${PYTHON}
  ${TOPLEVEL_DIR}/tests/start-svrd-continuation.py
  ${TOPLEVEL_DIR}/server/svrd
  bash
  ${CMAKE_CURRENT_SOURCE_DIR}/stream_resize.sh
  ${TOPLEVEL_DIR} )
