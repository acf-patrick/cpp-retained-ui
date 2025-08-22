file(GLOB_RECURSE CORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(CORE STATIC ${CORE_SOURCES})

target_link_libraries(CORE PRIVATE raylib)

target_include_directories(CORE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
