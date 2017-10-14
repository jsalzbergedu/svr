# CMake generated Testfile for 
# Source directory: /home/jacob/sources/svr/tests
# Build directory: /home/jacob/sources/svr/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(sanity_check "bash" "test.sh" "/home/jacob/sources/svr")
add_test(record_file "bash" "vidfiletest.sh" "/home/jacob/sources/svr")
