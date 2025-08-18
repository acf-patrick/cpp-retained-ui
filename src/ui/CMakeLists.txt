file(GLOB_RECURSE UI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(UI STATIC ${UI_SOURCES})

target_include_directories(UI PRIVATE external/yoga/yoga)
target_link_libraries(UI PRIVATE raylib yogacore)

target_include_directories(UI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})