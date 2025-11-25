cmake_minimum_required(VERSION 3.31)
project(SnakeGame)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Finde SDL2 package
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Definiere source files
set(COMMON_SOURCES
        Snake.cpp
        grid.cpp
        game.cpp
        Pathfinder.cpp
        ObstacleGenerator.cpp
        HUD.cpp
)

set(HEADER_FILES
        Snake.h
        common.h
        grid.h
        game.h
        Pathfinder.h
        ObstacleGenerator.h
        HUD.h
)

# Main executable
add_executable(SnakeGame
        main.cpp
        ${COMMON_SOURCES}
        ${HEADER_FILES}
)

# Handle SDL2 linking richtig
if(TARGET SDL2::SDL2)
    target_link_libraries(SnakeGame SDL2::SDL2 SDL2::SDL2main SDL2_ttf::SDL2_ttf)
else()
    include_directories(${SDL2_INCLUDE_DIRS})
    target_link_libraries(SnakeGame ${SDL2_LIBRARIES})
endif()

# Set output directory fuer alle ausführbare Dateien
set_target_properties(SnakeGame  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)