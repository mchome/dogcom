cmake_minimum_required(VERSION 3.0.0)
project(dogcom)

file(GLOB_RECURSE SRCS "src/*.c" "include/*.h")
include_directories(${PROJECT_SOURCE_DIR}/include)
message(${SRCS})
add_executable(dogcom ${SRCS})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message("on windows!\n")
    add_link_options("-lws2_32")
endif ()