cmake_minimum_required(VERSION 3.10)
project(FM_scheduling LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Werror")

include_directories(include)
include_directories(periodic_tasks_simulator/include)

add_executable( FM_scheduling src/RTO.cpp src/BWP.cpp
                main.cpp )

add_subdirectory( periodic_tasks_simulator )
target_link_libraries( FM_scheduling periodic_simulator )