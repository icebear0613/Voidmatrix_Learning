cmake_minimum_required( VERSION 3.10)

PROJECT(Moyu)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(EASY_DIR D:/ChromeCoreDownloads/easyx4mingw)

include_directories(${INC_DIR})
include_directories(${EASY_DIR}/include)
link_directories(${EASY_DIR}/lib)
link_libraries(libeasyx.a)


file(GLOB SRC ${SRC_DIR}/*.cpp)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)


add_executable(Moyu ${SRC})

target_link_libraries(Moyu easyx gdi32 winmm msimg32)
