cmake_minimum_required (VERSION 3.1)
project( svr )


## CPack packages

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 1)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Seawolf video router")
set(CPACK_PACKAGE_CONTACT "ncsurobotics@ncsurobotics.com")

include(CPack) # This line must go after where the variables are set

## Global Configuration
if( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
  set( CMAKE_C_FLAGS "-D__SVR_Linux__" )
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
  set( CMAKE_C_FLAGS "-D__SW_Darwin__" )
endif()
enable_testing()
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(TOPLEVEL_DIR ${CMAKE_CURRENT_SOURCE_DIR})

## Subdirectories to be built
add_subdirectory(lib)
add_subdirectory(server)
add_subdirectory(python)
add_subdirectory(util)
add_subdirectory(tests)

## Uninstall
# Add uninstall target, taken from the cmake wiki:
# https://cmake.org/Wiki/CMake_FAQ#Can_I_do_.22make_uninstall.22_with_CMake.3F
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()


set(HEADER_FILES
blockalloc.h comm.h encoding.h errors.h forward.h frameproperties.h lockable.h logging.h mempool.h message.h messagehandlers.h messagerouting.h net.h optionstring.h pack.h refcount.h responseset.h source.h stream.h svrcv.h)

foreach(FILE ${HEADER_FILES})
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/svr/${FILE}
    DESTINATION include/svr)
endforeach()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/svr.h
  DESTINATION include)

## Documentation
# The documentation target, which expects to be built from the toplevel dir
add_custom_target(doc COMMAND doxygen ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile)
