# add the executable
add_executable(code main.cpp)

# Link wiringPi
find_library(WIRINGPI_LIBRARIES NAMES wiringPi)
target_link_libraries(code ${WIRINGPI_LIBRARIES})