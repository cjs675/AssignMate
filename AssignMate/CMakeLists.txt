cmake_minimum_required(VERSION 3.16)
project(AssignmentTracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt's automated build tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find both the Qt6 Widgets AND Sql libraries. 
# CMake handles finding the correct paths automatically based on system
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

# Create executable
add_executable(Tracker main.cpp)

# Link both Qt6 libraries to executable
target_link_libraries(Tracker PRIVATE Qt6::Widgets Qt6::Sql)
