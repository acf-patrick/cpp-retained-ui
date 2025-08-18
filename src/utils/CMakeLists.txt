file(GLOB UTILS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(UTILS STATIC ${UTILS_SOURCES})

target_link_libraries(UTILS PRIVATE raylib)
target_include_directories(UTILS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
