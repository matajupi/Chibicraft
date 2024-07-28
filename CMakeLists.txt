cmake_minimum_required(VERSION 3.13)
project(chibicraft CXX)

# Set output directories
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "Output directory for executables")
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib CACHE PATH "Output directory for libraries")

# Set compile option
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")        # Enable C++20
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")     # Enable all warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")               # Specify optimization level

# Setup OpenMP
find_package(OpenMP REQUIRED)
if(OpenMP_FOUND)
    message([STATUS] "OpenMP found" ...)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

include(FetchContent)

# Setup SDL1.2
FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL-1.2.git
    GIT_TAG 457d4e55ffe1b6ad4c4fa4559dbda8360bf8253d
)
FetchContent_MakeAvailable(SDL)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863
)
FetchContent_MakeAvailable(glm)

add_subdirectory(src)

