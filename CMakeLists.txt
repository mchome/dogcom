cmake_minimum_required(VERSION 3.0.0)
project(dogcom)

set(CMAKE_C_STANDARD_REQUIRED 99)
set(FORCE_ENCRYPT OFF)

file(GLOB_RECURSE SRCS "src/*.c" "include/*.h")
include_directories(${PROJECT_SOURCE_DIR}/include)

if (${FORCE_ENCRYPT})
    add_compile_definitions(FORCE_ENCRYPT)
endif ()

add_executable(dogcom ${SRCS})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(dogcom PRIVATE ws2_32)
endif ()
