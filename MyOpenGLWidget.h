#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include <QMouseEvent>

#include "RenderModule.h"
#include "Camera.h"

class OGLWidget : 
	public QOpenGLWidget,
	public QOpenGLFunctions
{
	Q_OBJECT

public:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);

private:
	void printContextInformation();

private:
	std::vector<RenderModule*> rModules;
	Camera camera;

	bool leftPressed; // left button pressed or not
};