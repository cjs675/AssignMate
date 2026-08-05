cmake_minimum_required(VERSION 3.16)
project(AssignmentTracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt's automated build tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find the Qt6 Widgets library
# CMake handles finding the correct paths automatically based on system
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Create executable
add_executable(Tracker
        main.cpp
        resources.qrc

        ui/MainWindow.cpp
        ui/MainWindow.h

        models/Assignment.cpp
        models/Assignment.h
        models/Exam.cpp
        models/Exam.h
        models/Homework.cpp
        models/Homework.h
        models/Course.cpp
        models/Course.h
)

target_include_directories(Tracker PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
)



# Link both Qt6 libraries to executable
target_link_libraries(Tracker PRIVATE
        Qt6::Widgets)
