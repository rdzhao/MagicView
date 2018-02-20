#include "MyOpenGLWidget.h"

void OGLWidget::initializeGL()
{
	// general initialize
	leftPressed = false;

	// opengl initialize
	initializeOpenGLFunctions();

	//connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));

	printContextInformation();

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	//init camera
	camera.setView(QVector3D(0, 0, 4), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
	camera.setProject(45.0, 4.0 / 3.0, 0.1, 10);
	camera.init();

	RenderModule* rm = new MeshModule();
	rModules.push_back(rm);
}

void OGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < rModules.size(); ++i)
	{
		std::vector<float> v;
		std::vector<int> idx;
		std::vector<float> n;
		std::vector<float> c;

		cout << "-----------------"<< endl;
		Mesh* mesh;
		Parser_obj<Kernel, Enriched_items> parser;
		parser.read("C:\\Users\\rdzhao\\Desktop\\Project\\QtViewer\\QtViewer\\x64\\Release\\ball_3.obj", mesh);
		cout << "-----------------" << endl;

		mesh->index_elements();
		mesh->compute_normals();
		
		for (Vertex_iterator vi = mesh->vertices_begin(); vi != mesh->vertices_end(); ++vi)
		{
			cout << vi->point() << endl;
			v.push_back(vi->point().x());
			v.push_back(vi->point().y());
			v.push_back(vi->point().z());

			n.push_back(vi->normal().x());
			n.push_back(vi->normal().y());
			n.push_back(vi->normal().z());

			c.push_back(0.3);
			c.push_back(0.5);
			c.push_back(0.7);
		}

		for (Facet_iterator fi = mesh->facets_begin(); fi != mesh->facets_end(); ++fi)
		{
			Halfedge_around_facet_circulator he = fi->facet_begin();
			Halfedge_around_facet_circulator end = he;

			CGAL_For_all(he, end)
				idx.push_back(he->vertex()->idx());
		}

		//set some tentative data
		//std::vector<float> v = { 0,0,0,2,0,0,0,2,0,-2,0,0,0,-2,0,1,1,0,-1,1,0,-1,-1,0,1,-1,0 };
		//std::vector<int> idx = { 0,1,5,0,2,6,0,3,7,0,4,8 };
		//std::vector<float> n = { 0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1 };
		//std::vector<float> c = { 1,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0,0,1 };

		rModules[i]->setData(v, idx, n, c);;
		rModules[i]->setCamera(camera);
		rModules[i]->render();
	}
}

void OGLWidget::resizeGL(int width, int height)
{
	camera.setWinWidth(width);
	camera.setWinHeight(height);
	cout << width << " " << height << endl;
}

void OGLWidget::mousePressEvent(QMouseEvent* event)
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
void OGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		leftPressed = false;
	}
}
void OGLWidget::mouseMoveEvent(QMouseEvent* event)
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
void OGLWidget::wheelEvent(QWheelEvent* event)
{
	if (event->delta() != 0)
	{
		camera.setscroll(event->delta());
		camera.zoom();
		update();
	}
}

void OGLWidget::printContextInformation()
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