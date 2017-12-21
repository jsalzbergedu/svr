cmake_minimum_required (VERSION 3.1)
set(Python_ADDITIONAL_VERSIONS 2.7 2.6 2.5)
find_package(PythonInterp REQUIRED)


add_custom_target(pylib_copy_so
  COMMAND cp -f ${CMAKE_BINARY_DIR}/lib/libsvr.so ${CMAKE_CURRENT_SOURCE_DIR}/
  DEPENDS svr)

add_custom_target(pylib_setup
  COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/libsvr.so ${CMAKE_CURRENT_SOURCE_DIR}/libsvr.so.${LIBSVR_SOVERSION}
  DEPENDS pylib_copy_so)

add_custom_target(pylib
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/buildscript.sh ${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON_EXECUTABLE} build
  DEPENDS pylib_setup)

add_custom_target(pylib-install
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/buildscript.sh ${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON_EXECUTABLE} install)
