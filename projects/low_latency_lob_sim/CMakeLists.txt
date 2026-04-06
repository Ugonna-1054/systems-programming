cmake_minimum_required(VERSION 3.16)
project(low_latency_lob_sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(PERF_BUILD "Enable perf-friendly flags" OFF)

add_executable(lob_bench.tsk
  src/main.cpp
  src/order_book.cpp
)

target_include_directories(lob_bench.tsk PRIVATE src)

target_compile_options(lob_bench.tsk PRIVATE -Wall -Wextra -Wpedantic)

if(PERF_BUILD)
  message(STATUS "Compiling with perf flags ON")
  target_compile_options(lob_bench.tsk PRIVATE -O3 -g -fno-omit-frame-pointer)
else()
  message(STATUS "Compiling with perf flags OFF")
  target_compile_options(lob_bench.tsk PRIVATE -O3 -march=native)
endif()
