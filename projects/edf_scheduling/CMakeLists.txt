cmake_minimum_required(VERSION 3.13)

include(../../pico-sdk/pico_sdk_init.cmake)

project(edf_scheduling_example)

# Enable try/catch exception interface.
#set(PICO_CXX_ENABLE_EXCEPTIONS 1)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Specify etl location.
set(GIT_DIR_LOOKUP_POLICY ALLOW_LOOKING_ABOVE_CMAKE_SOURCE_DIR)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/etl
                 ${CMAKE_CURRENT_BINARY_DIR}/../../lib/etl)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

# Specify where to look for header files if they're not all in the same place.
#target_include_directories(${PROJECT_NAME} PUBLIC inc)
# Specify where to look for header files if they're all in one place.
include_directories(inc)


# Link libraries to the targets that need them.
target_link_libraries(${PROJECT_NAME} PUBLIC pico_stdlib)
target_link_libraries(${PROJECT_NAME} PRIVATE etl::etl)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})

# USB serial. Enable this for every library/executable that uses it.
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
