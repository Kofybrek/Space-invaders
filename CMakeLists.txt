project(SpaceInvaders CXX)
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 17)

set(SOURCES 
    Source/Animation.cpp
    Source/DrawText.cpp
    Source/Enemy.cpp
    Source/EnemyManager.cpp
    Source/Main.cpp
    Source/Player.cpp
    Source/Ufo.cpp
)

set (HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Headers/)

find_package(SFML 2.5
            COMPONENTS system window graphics audio REQUIRED)

add_executable(invaders ${SOURCES})
target_include_directories(invaders PRIVATE ${HEADERS_DIR} ${SFML_INCLUDE_DIRS})
target_link_libraries(invaders sfml-graphics sfml-system sfml-audio ${SFML_DEPENDENCIES})

# Copy resources to final executable location
message(STATUS "Copying resource files to ${CMAKE_CURRENT_BINARY_DIR}")
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Source/Resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)