# QtViewer

This project only supports windows platform x64.

Basic implementation for viewing polyhedral mesh based on Qt GUI.

Now this code includes a basic implementation for OpenGL rendering and arcball for user interaction.

Dependencies: CGAL and Qt5.

Install CGAL and Qt for Windows. 

Make sure the following environment variables are set: <br />
BOOST_LIBRARYDIR = ${BOOST_LOCAL_DIR}\lib64-msvc-xx.0
BOOST_INCLUDEDIR = ${BOOST_LOCAL_DIR}
CGAL_DIR = ${CGAL_LOCAL_DIR}
Qt5_DIR = ${Qt5_LOCAL_DIR}\5.xx.0\msvcxxxx_64

Add the following environment variables to PATH:
${BOOST_LOCAL_DIR}lib64-msvc-14.0
${CGAL_LOCAL_DIR}\auxiliary\gmp\lib
${CGAL_LOCAL_DIR}\build\bin
${Qt5_LOCAL_DIR}\5.xx.0\msvcxxxx_64\bin
${Qt5_LOCAL_DIR}\5.xx.0\msvcxxxx_64\plugins\platforms

Copy the following files to .exe folder from ${Qt5_LOCAL_DIR}\5.xx.0\msvcxxxx_64\bin:
libEGL.dll
libGLESv2.dll
Qt5Core.dll
Qt5Gui.dll
Qt5OpenGL.dll
Qt5Widgets.dll

Copy the following folder to .exe folder:
${Qt5_LOCAL_DIR}\5.xx.0\msvcxxxx_64\plugins\platforms
