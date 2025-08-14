cmake_minimum_required(VERSION 3.28)
project(SFML_HelloWorld LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(YOGA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YOGA_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(YOGA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(YOGA_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

file(GLOB_RECURSE sources "src/")

add_subdirectory(external/yoga)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(main src/main.cpp)
target_compile_features(main PRIVATE cxx_std_17)

target_include_directories(main PRIVATE src external/yoga/yoga)

target_link_libraries(main PRIVATE SFML::Graphics yogacore)
