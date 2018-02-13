#include "MyWindow.h"
#include <QDebug>
#include <QString>

MyWindow::~MyWindow()
{
	makeCurrent();
}

void MyWindow::initializeGL()
{
	// general initialize
	leftPressed = false;

	// opengl initialize
	initializeOpenGLFunctions();

	//connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));

	printContextInformation();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	//init camera
	camera.setView(QVector3D(0, 0, 4), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
	camera.setProject(45.0, 4.0 / 3.0, 0.1, 10);
	camera.init();

	RenderModule* rm = new MeshModule();
	rModules.push_back(rm);

}

void MyWindow::resizeGL(int width, int height)
{
	camera.setWinWidth(width);
	camera.setWinHeight(height);
}

void MyWindow::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < rModules.size(); ++i)
	{
		// set some tentative data
		std::vector<float> v = { 0,0,0,2,0,0,0,2,0,-2,0,0,0,-2,0,1,1,0,-1,1,0,-1,-1,0,1,-1,0 };
		std::vector<int> idx = { 0,1,5,0,2,6,0,3,7,0,4,8 };
		std::vector<float> n = { 0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1 };
		std::vector<float> c = { 1,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0,0,1 };

		rModules[i]->setData(v, idx, n, c);;
		rModules[i]->setCamera(camera);
		rModules[i]->render();
	}
}

void MyWindow::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		//cout << "@@@@@@@@@@@@@@@@@@@@@@@"<< endl;
		leftPressed = true;

		camera.setPWX(event->x());
		camera.setPWY(event->y());
		camera.updatePUnitCoord();
		camera.setPRotationIdentity();
	}
}

void MyWindow::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		leftPressed = false;
	}
}

void MyWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (leftPressed)
	{
		//camera.moveUnitCoordToPre();

		camera.setWX(event->x());
		camera.setWY(event->y());
		camera.updateUnitCoord();

		camera.arcballRotate();
		update();
	}
}

void MyWindow::wheelEvent(QWheelEvent* event)
{
	if (event->delta() != 0)
	{
		camera.setscroll(event->delta());
		camera.zoom();
		update();
	}
}

void MyWindow::update()
{
	QOpenGLWindow::update();
}

void MyWindow::printContextInformation()
{
	QString glType;
	QString glVersion;
	QString glProfile;

	// Get Version Information
	glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
	glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

	// Get Profile Information
#define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
	switch (format().profile())
	{
		CASE(NoProfile);
		CASE(CoreProfile);
		CASE(CompatibilityProfile);
	}
#undef CASE

	// qPrintable() will print our QString w/o quotes around it.
	qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";
}