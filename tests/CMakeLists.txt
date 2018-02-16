enable_testing()
find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})
include_directories(. ../src)

aux_source_directory(../src COMMON_SOURCES)

# request test
add_executable(request_test request_test.c ../src/database/request.c ../src/database/request_parser.c ${COMMON_SOURCES})
target_link_libraries(request_test ${CHECK_LIBRARIES})
add_test(NAME request_test COMMAND request_test)

# response test
add_executable(response_test response_test.c ../src/client/response.c ../src/client/response_parser.c ${COMMON_SOURCES})
target_link_libraries(response_test ${CHECK_LIBRARIES})
add_test(NAME response_test COMMAND response_test)

# execve test
add_executable(execve_test execve_test.c)
target_link_libraries(execve_test ${CHECK_LIBRARIES})
add_test(NAME execve_test COMMAND execve_test)

# threads test
add_executable(threads_test threads_test.c)
target_link_libraries(threads_test ${CHECK_LIBRARIES})
add_test(NAME threads_test COMMAND threads_test)

# list test
add_executable(list_test list_test.c ../src/list.c)
target_link_libraries(list_test ${CHECK_LIBRARIES})
add_test(NAME list_test COMMAND list_test)

