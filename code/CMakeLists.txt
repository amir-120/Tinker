cmake_minimum_required(VERSION 3.7)

# Add source files
set(SOURCE_FILES main.cpp)

# Add header files
set(HEADER_FILES StructDS.hpp)

# Add the executable
add_executable(code {$SOURCE_FILES} {$HEADER_FILES})

# Link wiringPi
find_library(WIRINGPI_LIBRARIES NAMES wiringPi)
target_link_libraries(code ${WIRINGPI_LIBRARIES})