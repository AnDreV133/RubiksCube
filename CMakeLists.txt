cmake_minimum_required(VERSION 3.25)
project(RubiksCube)

set(CMAKE_CXX_STANDARD 14)

add_executable(RubiksCube
        main.cpp )

target_link_libraries(RubiksCube -lfreeglut -lopengl32  -lgdi32 -lglfw3 -lglu32
        )

