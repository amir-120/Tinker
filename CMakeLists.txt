cmake_minimum_required(VERSION 3.10)

# set the project name and standards
project(Tinkerboard C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(code)

# Link wiringPi
find_library(WIRINGPI_LIBRARIES NAMES wiringPi)
target_link_libraries(code ${WIRINGPI_LIBRARIES})