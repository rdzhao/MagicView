# MagicView

This project only supports Visual Studio 2015+ x64.

Basic implementation for viewing polyhedral mesh based on Qt GUI.

Now this code includes a basic implementation for OpenGL rendering, arcball and primitive selection for user interaction.

Dependencies: CGAL and Qt5. Install CGAL and Qt for Windows first. 

## GUI Example
This example shows loading a large mesh.
![alt text](https://github.com/rdzhao/QtViewer/blob/master/illustration/example_2.png) <!-- .element height="50%" width="50%" -->

This example shows face, edge and vertex selection. 
![alt text](https://github.com/rdzhao/QtViewer/blob/master/illustration/example_1.png) <!-- .element height="50%" width="50%" -->

## Make sure the following environment variables are set: <br />
BOOST_LIBRARYDIR = ${BOOST_LOCAL_DIR}\lib64-msvc-xx.0 <br />
BOOST_INCLUDEDIR = ${BOOST_LOCAL_DIR} <br />
CGAL_DIR = ${CGAL_LOCAL_DIR} <br />
Qt5_DIR = ${Qt5_LOCAL_DIR}\5.xx.0\msvcxxxx_64

## Add the following environment variables to PATH: <br />
${BOOST_LOCAL_DIR}lib64-msvc-14.0 <br />
${CGAL_LOCAL_DIR}\auxiliary\gmp\lib <br />
${CGAL_LOCAL_DIR}\build\bin <br />
${Qt5_LOCAL_DIR}\5.xx.0\msvcxxxx_64\bin <br />
${Qt5_LOCAL_DIR}\5.xx.0\msvcxxxx_64\plugins\platforms

## Copy the following files to .exe folder from ${Qt5_LOCAL_DIR}\5.xx.0\msvcxxxx_64\bin: <br />
libEGL.dll <br />
libGLESv2.dll <br />
Qt5Core.dll <br />
Qt5Gui.dll <br />
Qt5OpenGL.dll <br />
Qt5Widgets.dll

## Copy the following folder to .exe folder: <br />
${Qt5_LOCAL_DIR}\5.xx.0\msvcxxxx_64\plugins\platforms

## References
Ray Triangle intersection code is from https://github.com/jeremynewlin/Accel.

Import and export icons are from https://icons8.com.

Black curor-clicked icon is from https://www.shareicon.net, which is integrated in face, edge and vertex selection icon.
