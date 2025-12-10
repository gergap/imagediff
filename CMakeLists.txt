cmake_minimum_required(VERSION 3.16)
project(ImageDiff LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Widgets Gui Core REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_executable(ImageDiff
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/ImageCompareWidget.h
    src/ImageCompareWidget.cpp
    src/ImageDiff.h
    src/ImageDiff.cpp
)

target_include_directories(ImageDiff PRIVATE src)
target_link_libraries(ImageDiff PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core)

install(TARGETS ImageDiff DESTINATION bin)
install(FILES scripts/git-png-review DESTINATION bin)
