cmake_minimum_required(VERSION 3.8)
project(TPE-FINAL-SO)

set(CMAKE_C_STANDARD 99)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread --std=c99 -pedantic -pedantic-errors -Wall -Wextra -Werror -Wno-unused-parameter -D_POSIX_C_SOURCE=200112L -O1 -fsanitize=address")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -lsqlite3 --std=c99 -pedantic -pedantic-errors -fsanitize=address")

aux_source_directory(client CLIENT_SOURCE_FILES)
aux_source_directory(server SERVER_SOURCE_FILES)
aux_source_directory(database DB_SOURCE_FILES)
aux_source_directory(common COMMON_SOURCE_FILES)
aux_source_directory(tests TESTS_SOURCE_FILES)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_executable(client ${CLIENT_SOURCE_FILES} ${COMMON_SOURCE_FILES})
add_executable(server ${SERVER_SOURCE_FILES} ${COMMON_SOURCE_FILES})
add_executable(database ${DB_SOURCE_FILES} ${COMMON_SOURCE_FILES})
add_executable(tests ${TESTS_SOURCE_FILES} ${COMMON_SOURCE_FILES} database/db_functions.c database/db_functions.h database/request.c database/request.h database/request_parser.c database/request_parser.h
                                                                  client/response.c client/response.h client/response_parser.c client/response_parser.h)

find_package (sqlite3)
if (SQLITE3_FOUND)
    include_directories(${SQLITE3_INCLUDE_DIRS})
    target_link_libraries (database ${SQLITE3_LIBRARIES})
    target_link_libraries (tests ${SQLITE3_LIBRARIES})
endif (SQLITE3_FOUND)