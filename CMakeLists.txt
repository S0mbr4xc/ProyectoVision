cmake_minimum_required(VERSION 3.10)
project(ProyectoVisionQt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenCV REQUIRED)
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

set(SRC_FILES
        src/main.cpp
        src/mainwindow.cpp
        src/Procesar.cpp
)
qt5_wrap_ui(UI_HEADERS src/mainwindow.ui)
qt5_wrap_cpp(MOC_SOURCES src/mainwindow.h)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(ProyectoVisionQt
        ${SRC_FILES}
        ${UI_HEADERS}
        ${MOC_SOURCES}
)

target_link_libraries(ProyectoVisionQt
        Qt5::Widgets
        ${OpenCV_LIBS}
        ${ITK_LIBRARIES}
)
