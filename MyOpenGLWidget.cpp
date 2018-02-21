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
	glDepthFunc(GL_LESS);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	//init camera
	//camera.setView(QVector3D(0, 0, 4), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
	//camera.setProject(45.0, 4.0 / 3.0, 0.01, 100);
	//camera.init();

	//setRenderContexts();
}

void OGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < rModules.size(); ++i)
	{
		rModules[i]->setCamera(camera);
		rModules[i]->render();
	}
}

void OGLWidget::resizeGL(int width, int height)
{
	camera.setWinWidth(width);
	camera.setWinHeight(height);
	//cout << width << " " << height << endl;
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

void OGLWidget::setMesh(Mesh* m)
{
	mesh = m;
}

void OGLWidget::setRenderContexts()
{
	RenderModule* rm1 = new MeshModule();
	rModules.push_back(rm1);
	RenderModule* rm2 = new WireFrameModule();
	rModules.push_back(rm2);

	setMeshModule(rm1);
	setWireFrameModule(rm2);
}

void OGLWidget::setMeshModule(RenderModule* rm)
{
	std::vector<float> v;
	std::vector<int> idx;
	std::vector<float> n;
	std::vector<float> c;

	Point_3 cc(0, 0, 0);
	for (Vertex_iterator vi = mesh->vertices_begin(); vi != mesh->vertices_end(); ++vi)
		cc += Vector_3(vi->point().x(), vi->point().y(), vi->point().z());
	Point_3 ccc(cc.x() / mesh->size_of_vertices(), cc.y() / mesh->size_of_vertices(), cc.z() / mesh->size_of_vertices());

	for (Vertex_iterator vi = mesh->vertices_begin(); vi != mesh->vertices_end(); ++vi)
	{
		//cout << vi->point() << endl;
		v.push_back(vi->point().x() - ccc.x());
		v.push_back(vi->point().y() - ccc.y());
		v.push_back(vi->point().z() - ccc.z());

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

		//cout << "^^^^^^^^^^^^^^^^^^^^"<< endl;
		CGAL_For_all(he, end)
		{
			//cout << he->vertex()->idx() << endl;
			idx.push_back(he->vertex()->idx());
		}
	}

	rm->setData(v, idx, n, c);
}
void OGLWidget::setWireFrameModule(RenderModule* rm)
{
	std::vector<float> v;
	std::vector<int> idx;
	std::vector<float> n;
	std::vector<float> c;

	Point_3 cc(0, 0, 0);
	for (Vertex_iterator vi = mesh->vertices_begin(); vi != mesh->vertices_end(); ++vi)
		cc += Vector_3(vi->point().x(), vi->point().y(), vi->point().z());
	Point_3 ccc(cc.x() / mesh->size_of_vertices(), cc.y() / mesh->size_of_vertices(), cc.z() / mesh->size_of_vertices());

	for (Vertex_iterator vi = mesh->vertices_begin(); vi != mesh->vertices_end(); ++vi)
	{
		//cout << vi->point() << endl;
		v.push_back(vi->point().x() - ccc.x());
		v.push_back(vi->point().y() - ccc.y());
		v.push_back(vi->point().z() - ccc.z());

		n.push_back(vi->normal().x());
		n.push_back(vi->normal().y());
		n.push_back(vi->normal().z());

		c.push_back(1);
		c.push_back(0);
		c.push_back(0);
	}

	for (Edge_iterator ei = mesh->edges_begin(); ei != mesh->edges_end(); ++ei)
	{
		idx.push_back(ei->vertex()->idx());
		idx.push_back(ei->opposite()->vertex()->idx());
	}

	rm->setData(v, idx, n, c);
}

void OGLWidget::setCamera()
{

	camera.setView(QVector3D(0, 0, 4 * (mesh->zmax() - mesh->zmin())), QVector3D(0, 0, 0), QVector3D(0, 1, 0));

	camera.setProject(45.0, 4.0 / 3.0, 0.01, 100);
	
	camera.init();
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