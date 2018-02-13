#pragma once

#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include <QMouseEvent>

#include "RenderModule.h"
#include "Camera.h"

#include <iostream>

using namespace std;

class MyWindow :
	public QOpenGLWindow,
	protected QOpenGLFunctions
{
	Q_OBJECT
public:
	~MyWindow();

	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);

protected slots:
	void update();

private:
	void printContextInformation();

	std::vector<RenderModule*> rModules;
	Camera camera;

	bool leftPressed; // left button pressed or not
};


#endif // !MYWINDOW_H
