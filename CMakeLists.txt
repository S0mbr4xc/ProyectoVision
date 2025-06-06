cmake_minimum_required(VERSION 3.10)
project(ProyectoVisionQt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)    # Para que Qt procese los .ui y genere los ui_*.h
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenCV REQUIRED)
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

# Le decimos al compilador que busque encabezados también en src/
include_directories(${CMAKE_SOURCE_DIR}/src)

# Listamos todos los .cpp que van en el ejecutable:
set(SRC_FILES
        src/main.cpp
        src/mainwindow.cpp
        src/Procesar.cpp
)

# Aquí va el .ui para que AUTOUIC lo procese
set(UI_FILES
        src/mainwindow.ui
)

add_executable(ProyectoVisionQt
        ${SRC_FILES}
        ${UI_FILES}
)

target_link_libraries(ProyectoVisionQt
        Qt5::Widgets
        ${OpenCV_LIBS}
        ${ITK_LIBRARIES}
)
