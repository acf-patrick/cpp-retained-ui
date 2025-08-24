file(GLOB_RECURSE UI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(UI STATIC ${UI_SOURCES})

target_include_directories(UI PUBLIC external/yoga/yoga ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(UI PUBLIC raylib yogacore)