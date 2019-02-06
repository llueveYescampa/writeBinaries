cmake_minimum_required(VERSION 3.10)

## set appropriately the following line to create a Double Precision version
set(DOUBLE_PRECISION ON)

project(writeBinaries C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

if (DOUBLE_PRECISION)
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -DDOUBLE")
endif()

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" )
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -lm -Wno-unused-result -ftree-vectorize -fopt-info-vec-optimized -ffast-math -mavx2")
endif ( )



include_directories("src/include")
set (writeBinaries "src/writeBinaries.c")
add_executable(writeBinaries ${writeBinaries})

