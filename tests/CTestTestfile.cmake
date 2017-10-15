# CMake generated Testfile for 
# Source directory: /home/jacob/sources/svr/tests
# Build directory: /home/jacob/sources/svr/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(sanity_check "/usr/bin/python" "/home/jacob/sources/svr/tests/start-svrd-continuation.py" "/home/jacob/sources/svr/server/svrd" "bash" "/home/jacob/sources/svr/tests/test.sh" "/home/jacob/sources/svr")
add_test(record_file "/usr/bin/python" "/home/jacob/sources/svr/tests/start-svrd-continuation.py" "/home/jacob/sources/svr/server/svrd" "bash" "/home/jacob/sources/svr/tests/vidfiletest.sh" "/home/jacob/sources/svr")
