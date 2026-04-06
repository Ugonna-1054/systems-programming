cmake_minimum_required(VERSION 3.16)
project(low_latency_lob_sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(lob_bench
  src/main.cpp
  src/order_book.cpp
)

target_include_directories(lob_bench PRIVATE include)

target_compile_options(lob_bench PRIVATE -O3 -march=native -Wall -Wextra -Wpedantic)
