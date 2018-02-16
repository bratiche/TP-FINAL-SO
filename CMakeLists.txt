cmake_minimum_required(VERSION 3.5)
project(TPE-FINAL-SO C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread --std=c99 -pedantic -pedantic-errors -Wall -Wextra -Werror -Wno-unused-parameter -D_POSIX_C_SOURCE=200112L -O1 -fsanitize=address")

enable_testing()

#####################################################################
find_package (sqlite3 REQUIRED)
include_directories(${SQLITE3_INCLUDE_DIRS})

aux_source_directory(src COMMON_SOURCE_FILES)
aux_source_directory(src/client CLIENT_SOURCE_FILES)
aux_source_directory(src/server SERVER_SOURCE_FILES)
aux_source_directory(src/database DB_SOURCE_FILES)

add_executable(client ${CLIENT_SOURCE_FILES} ${COMMON_SOURCE_FILES})
add_executable(server ${SERVER_SOURCE_FILES} ${COMMON_SOURCE_FILES})
add_executable(database ${DB_SOURCE_FILES} ${COMMON_SOURCE_FILES})
target_link_libraries(database ${SQLITE3_LIBRARIES})
#####################################################################

add_subdirectory(tests)
