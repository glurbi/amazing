cmake_minimum_required(VERSION 2.8)

set(EXECUTABLE_NAME "amazing")

project(${EXECUTABLE_NAME})

set(SOURCE
    amazing.cpp
    graph.cpp
    matrix.cpp
    menu.cpp
    misc.cpp
    model.cpp
    play.cpp
    program.cpp
    texture.cpp
    timer.cpp
)

set(HEADERS
    context.hpp
    graph.hpp
    geometry.hpp
    matrix.hpp
    menu.hpp
    misc.hpp
    model.hpp
    play.hpp
    program.hpp
    texture.hpp
    timer.hpp
)

add_executable(${EXECUTABLE_NAME} ${SOURCE} ${HEADERS})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(SFML 2.1 REQUIRED system window graphics network audio)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED glew32)
        
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY})

include_directories(${SFML_INCLUDE_DIR} ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_PATH})

configure_file(smiley.png ${CMAKE_CURRENT_BINARY_DIR}/Debug/smiley.png COPYONLY)
configure_file(Anonymous.ttf ${CMAKE_CURRENT_BINARY_DIR}/Debug/Anonymous.ttf COPYONLY)
