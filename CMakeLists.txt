cmake_minimum_required(VERSION 3.1)

project(MagicView)

find_package(OpenGL)

find_package(Qt5Widgets)
find_package(Qt5OpenGL)
set(CMAKE_AUTOMOC ON)

find_package(CGAL)

include(${CGAL_USE_FILE})

add_subdirectory(glm)

file(GLOB Headers "*.h")
file(GLOB Sources "*.cpp")
file(GLOB Resources "resources/*.png")

add_executable(MagicView ${Headers} ${Sources} ${Resources})

qt5_use_modules(MagicView Widgets)
qt5_use_modules(MagicView OpenGL)

target_link_libraries( MagicView ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})
target_link_libraries( MagicView ${QT_LIBRARIES} ${OPENGL_LIBRARIES})