cmake_minimum_required(VERSION 2.8)

project(MagicView)

find_package(OpenGL)

find_package(Qt5Widgets)
find_package(Qt5OpenGL)
set(CMAKE_AUTOMOC ON)

find_package(CGAL)

include(${CGAL_USE_FILE})

include_directories(/glm/)

aux_source_directory(. SRCS)

add_executable(MagicView ${SRCS})

qt5_use_modules(MagicView Widgets)
qt5_use_modules(MagicView OpenGL)

target_link_libraries( MagicView ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})
target_link_libraries( MagicView ${QT_LIBRARIES} ${OPENGL_LIBRARIES})