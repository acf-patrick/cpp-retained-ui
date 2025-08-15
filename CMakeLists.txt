cmake_minimum_required(VERSION 3.28)
project(RetainedUI LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(RAYLIB_VERSION 5.0)
set(UI "src/ui")
set(UTILS "src/utils")

set(YOGA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YOGA_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(YOGA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(YOGA_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(external/yoga)

include(FetchContent)

FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(raylib)

add_subdirectory(${UI})
add_subdirectory(${UTILS})
add_executable(RetainedUI src/main.cpp)

target_compile_features(RetainedUI PRIVATE cxx_std_17)

target_include_directories(RetainedUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src external/yoga/yoga)

set_target_properties(raylib PROPERTIES BUILD_SHARED_LIBRS ON)

target_link_libraries(RetainedUI PRIVATE raylib yogacore UI UTILS)
