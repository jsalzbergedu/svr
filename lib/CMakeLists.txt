cmake_minimum_required(VERSION 3.1)
project(libsvr)

find_package( OpenCV REQUIRED )
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenCV_CFLAGS} -Wall -Werror")
include_directories ( ${OpenCV_INCLUDE_DIRS} ../include )

add_library(svr SHARED blockalloc.c mempool.c message.c pack.c net.c logging.c refcount.c frameproperties.c encoding.c lockable.c main.c encodings/raw.c responseset.c messagerouting.c messagehandlers.c stream.c source.c comm.c optionstring.c encodings/jpeg.c)
target_link_libraries( svr pthread seawolf jpeg ${OpenCV_LIBS})

set(LIBSVR_SOVERSION 0)
set(LIBSVR_VERSION "0.1")

set_target_properties(svr PROPERTIES VERSION ${LIBSVR_VERSION})
set_target_properties(svr PROPERTIES SOVERSION ${LIBSVR_SOVERSION})

install(TARGETS svr DESTINATION "lib")
