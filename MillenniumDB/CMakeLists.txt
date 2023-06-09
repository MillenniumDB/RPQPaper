cmake_minimum_required(VERSION 3.12)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(EXECUTABLE_OUTPUT_PATH bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY bin)

# set the project name and version
project(MillenniumDB VERSION 0.1)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -pedantic -O0 -g3  \
-pthread -march=native -funroll-loops \
-fsanitize=undefined -fno-omit-frame-pointer -fsanitize=address")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -pedantic -O3 -g0 \
-pthread -march=native -funroll-loops")

find_package(Boost 1.71.0 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src)

add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/antlr4-runtime-4.9.3)
include_directories(${CMAKE_SOURCE_DIR}/third_party/antlr4-runtime-4.9.3/runtime/src)

file(GLOB_RECURSE SRCS src/*.cc)
file(GLOB_RECURSE BIN_SRCS src/bin/*.cc)
file(GLOB_RECURSE TEST_SRCS src/tests/*.cc)

set(sources ${SRCS})
list(REMOVE_ITEM sources ${BIN_SRCS})
list(REMOVE_ITEM sources ${TEST_SRCS})

add_library(millenniumdb STATIC ${sources})

set(BUILD_TARGETS
    create_db
    # create_db_sparql
    query
    server
    # server_sparql
)

set(TEST_TARGETS
    # check_bpts
    # check_extendible_hash
    # compare_datetime
    # compare_decimal_both_ext
    # compare_decimal_both_inl
    # compare_decimal_inl_ext
    # create_bpt
    # execute_query
    # normalize_decimal
)

foreach(target ${BUILD_TARGETS})
    add_executable(${target} ${CMAKE_SOURCE_DIR}/src/bin/${target}.cc)
    target_link_libraries(${target}
        millenniumdb
        antlr4_cpp_runtime
        stdc++fs
    )
endforeach(target)

enable_testing()
foreach(target ${TEST_TARGETS})
    add_executable(${target} ${CMAKE_SOURCE_DIR}/src/tests/${target}.cc)
    target_link_libraries(${target}
        millenniumdb
        antlr4_cpp_runtime
        stdc++fs
    )
    add_test(NAME ${target} COMMAND $<TARGET_FILE:${target}>)
endforeach(target)
