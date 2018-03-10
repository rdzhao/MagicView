#include "MyOpenGLWidget.h"

void OGLWidget::initializeGL()
{
	// general initialize
	leftPressed = false;
	faceSelection = false;
	edgeSelection = false;
	vertSelection = false;

	// opengl initialize
	initializeOpenGLFunctions();

	//connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));

	printContextInformation();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void OGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < rModules.size(); ++i)
	{
		if (rModules[i]->visible())
		{
			rModules[i]->setCamera(camera);
			rModules[i]->setLightDistance(5 * mesh->radius());
			rModules[i]->render();
		}
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
	if (faceSelection && event->button() == Qt::LeftButton)
	{
		cout << "At Face Selection..."<< endl;

		selectFace(event->x(), event->y());
	}
	else if (edgeSelection && event->button() == Qt::LeftButton)
	{
		cout << "At Edge Selection..." << endl;
	
		selectEdge(event->x(), event->y());
	}
	else if (vertSelection && event->button() == Qt::LeftButton)
	{
		cout << "At Vertex Selection..." << endl;

		selectVertex(event->x(), event->y());
	}
	else if (event->button() == Qt::LeftButton)
	{
		//cout << event->x()<<" "<< event->y() << endl;
		leftPressed = true;

		camera.setPWX(event->x());
		camera.setPWY(event->y());
		camera.updatePUnitCoord();
		camera.setPRotationIdentity();
	}
	else if (event->button() == Qt::RightButton)
	{
		rightPressed = true;

		camera.setPWX(event->x());
		camera.setPWY(event->y());
		camera.updatePUnitCoord();
		camera.setPTranslationIdentity();
	}
}

void OGLWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	setCamera();
	update();
}

void OGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		leftPressed = false;
	}
	else if (event->button() == Qt::RightButton)
	{
		rightPressed = false;
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
	else if (rightPressed)
	{
		camera.setWX(event->x());
		camera.setWY(event->y());
		camera.updateUnitCoord();

		double ratio = mesh->radius();

		camera.move(ratio);
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

void OGLWidget::clear()
{
	//for (int i = 0; i < rModules.size(); ++i)
	//	rModules[i]->clear();
	rModules.clear();

	mesh->clear();
	delete kdTree;

	selectedVerts.clear();
}

void OGLWidget::setMesh(Mesh* m)
{
	mesh = m;
}

void OGLWidget::setBall(Mesh* b)
{
	ball = b;
}

void OGLWidget::setKDTree()
{
	auto start = std::chrono::system_clock::now();

	int num_verts, num_tris;
	glm::vec3 *verts, *tris;
	int k;
	// import mesh data
	num_verts = mesh->size_of_vertices();
	verts = new glm::vec3[num_verts];
	for (Vertex_iterator vi = mesh->vertices_begin(); vi != mesh->vertices_end(); ++vi)
		verts[vi->idx()] = 
		glm::vec3(vi->point().x() - mesh->xcenter(),
			vi->point().y() - mesh->ycenter(),
			vi->point().z() - mesh->zcenter());
	num_tris = mesh->size_of_facets();
	tris = new glm::vec3[num_tris];
	for (Facet_iterator fi = mesh->facets_begin(); fi != mesh->facets_end(); ++fi)
		tris[fi->idx()] =
		glm::vec3(fi->halfedge()->vertex()->idx(),
			fi->halfedge()->next()->vertex()->idx(),
			fi->halfedge()->next()->next()->vertex()->idx());

	kdTree = new KDTreeCPU(num_tris, tris, num_verts, verts);
	
	cout << "Num Verts: " << num_verts << endl;
	cout << "Num Faces: " << num_tris << endl;
	cout << "KD tree construction complete ..." << endl;

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	cout << "Construction Time: " << elapsed_seconds.count() << endl;

}

void OGLWidget::setRenderContexts()
{
	meshModule = new MeshModule();
	rModules.push_back(meshModule);
	
	wireFrameModule = new WireFrameModule();
	rModules.push_back(wireFrameModule);

	vertexHLModule = new MeshModule();
	rModules.push_back(vertexHLModule);

	setMeshModule(meshModule);
	setWireFrameModule(wireFrameModule);
}

void OGLWidget::setMeshModule(RenderModule* rm)
{
	std::vector<float> v;
	std::vector<int> idx;
	std::vector<float> n;
	std::vector<float> c;

	for (Facet_iterator fi = mesh->facets_begin(); fi != mesh->facets_end(); ++fi)
	{
		Halfedge_around_facet_circulator he = fi->facet_begin();
		Halfedge_around_facet_circulator end = he;

		do{
			v.push_back(he->vertex()->point().x() - mesh->xcenter());
			v.push_back(he->vertex()->point().y() - mesh->ycenter());
			v.push_back(he->vertex()->point().z() - mesh->zcenter());

			n.push_back(he->vertex()->normal().x());
			n.push_back(he->vertex()->normal().y());
			n.push_back(he->vertex()->normal().z());

			c.push_back(0.5);
			c.push_back(0.5);
			c.push_back(0.5);

			//idx.push_back(he->vertex()->idx());
			++he;
		} while (he != end);
	}

	rm->setData(v, idx, n, c);
}

void OGLWidget::setWireFrameModule(RenderModule* rm)
{
	std::vector<float> v;
	std::vector<int> idx;
	std::vector<float> n;
	std::vector<float> c;

	for (Halfedge_iterator ei = mesh->edges_begin(); ei != mesh->edges_end(); ++ei, ++ei)
	{
		//cout << ei->idx() << " " << ei->opposite()->idx() << endl;

		v.push_back(ei->vertex()->point().x() - mesh->xcenter());
		v.push_back(ei->vertex()->point().y() - mesh->ycenter());
		v.push_back(ei->vertex()->point().z() - mesh->zcenter());

		n.push_back(ei->vertex()->normal().x());
		n.push_back(ei->vertex()->normal().y());
		n.push_back(ei->vertex()->normal().z());

		c.push_back(0.2);
		c.push_back(0.2);
		c.push_back(0.2);

		v.push_back(ei->opposite()->vertex()->point().x() - mesh->xcenter());
		v.push_back(ei->opposite()->vertex()->point().y() - mesh->ycenter());
		v.push_back(ei->opposite()->vertex()->point().z() - mesh->zcenter());
	
		n.push_back(ei->opposite()->vertex()->normal().x());
		n.push_back(ei->opposite()->vertex()->normal().y());
		n.push_back(ei->opposite()->vertex()->normal().z());
	
		c.push_back(0.2);
		c.push_back(0.2);
		c.push_back(0.2);
	}

	rm->setData(v, idx, n, c);
}

void OGLWidget::addVertexHL(RenderModule* rm, float x, float y, float z)
{
	std::vector<float> v;
	std::vector<int> idx;
	std::vector<float> n;
	std::vector<float> c;

	double ave=0;
	for (Halfedge_iterator hei = mesh->halfedges_begin(); hei != mesh->halfedges_end(); ++hei)
		ave += hei->length();
	ave /= 2 * mesh->size_of_halfedges();
	float ratio = ave / ball->radius()*0.2;
	
	for (Facet_iterator fi = ball->facets_begin(); fi != ball->facets_end(); ++fi)
	{
		Halfedge_around_facet_circulator he = fi->facet_begin();
		Halfedge_around_facet_circulator end = he;

		do {
			v.push_back((he->vertex()->point().x() - ball->xcenter())*ratio + x - mesh->xcenter());
			v.push_back((he->vertex()->point().y() - ball->ycenter())*ratio + y - mesh->ycenter());
			v.push_back((he->vertex()->point().z() - ball->zcenter())*ratio + z - mesh->zcenter());

			n.push_back(he->vertex()->normal().x());
			n.push_back(he->vertex()->normal().y());
			n.push_back(he->vertex()->normal().z());

			c.push_back(0.8);
			c.push_back(0.6);
			c.push_back(0.2);

			//idx.push_back(he->vertex()->idx());
			++he;
		} while (he != end);
	}

	rm->appData(v, idx, n, c);
}

void OGLWidget::delVertexHL(RenderModule* rm, int pos)
{
	int posBegin = pos*ball->size_of_facets() * 9;
	rm->delData(posBegin, ball->size_of_facets() * 9);
}

void OGLWidget::setCamera()
{

	camera.setView(QVector3D(0, 0, 5*mesh->radius()), QVector3D(0, 0, 0), QVector3D(0, 1, 0));

	camera.setProject(45.0, 4.0 / 3.0, 0.01, 10000);
	
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

void OGLWidget::setFaceVisible(bool v)
{
	meshModule->setVisible(v);
	update();
}

void OGLWidget::setEdgeVisible(bool v)
{
	wireFrameModule->setVisible(v);
	update();
}

void OGLWidget::setFaceSelection(bool b)
{
	faceSelection = b;
}

void OGLWidget::setEdgeSelection(bool b)
{
	edgeSelection = b;
}

void OGLWidget::setVertSelection(bool b)
{
	vertSelection = b;
}

void OGLWidget::selectFace(int wx, int wy)
{
	QVector3D nearP, farP, d;
	camera.getFarNearPointWorld(wx, wy, nearP, farP);
	d = (farP - nearP).normalized();

	glm::vec3 rayO, rayD, hitP, normal;
	float t;
	int idx;
	rayO = glm::vec3(nearP.x(), nearP.y(), nearP.z());
	rayD = glm::vec3(d.x(), d.y(), d.z());
	bool intersected = kdTree->intersectNew(rayO, rayD, t, hitP, normal, idx);

	if (intersected)
	{
		mesh->facet(idx)->selected() = !mesh->facet(idx)->selected();
		meshModule->highlightFace(idx, mesh->facet(idx)->selected());
		update();
	}
}

void OGLWidget::selectEdge(int wx, int wy)
{
	QVector3D nearP, farP, d;
	camera.getFarNearPointWorld(wx, wy, nearP, farP);
	d = (farP - nearP).normalized();

	glm::vec3 rayO, rayD, hitP, normal;
	float t;
	int idx;
	rayO = glm::vec3(nearP.x(), nearP.y(), nearP.z());
	rayD = glm::vec3(d.x(), d.y(), d.z());
	bool intersected = kdTree->intersectNew(rayO, rayD, t, hitP, normal, idx);

	if (intersected)
	{
		// choose the closest edge to the hit point
		double distance = std::numeric_limits<double>::max();
		int eIdx;

		Point_3 hp(hitP.x + mesh->xcenter(), hitP.y + mesh->ycenter(), hitP.z + mesh->zcenter());
		Facet_iterator fi = mesh->facet(idx);
		Halfedge_iterator hei = fi->halfedge();
		do {
			Vector_3 ev, hv;
			ev = hei->opposite()->vertex()->point() - hei->vertex()->point();
			hv = hp - hei->vertex()->point();
			glm::vec3 gev, ghv;
			gev = glm::vec3(ev.x(), ev.y(), ev.z());
			ghv = glm::vec3(hv.x(), hv.y(), hv.z());
			double dis = glm::length((ghv - (glm::dot(ghv, glm::normalize(gev))*glm::normalize(gev))));

			if (dis < distance)
			{
				distance = dis;
				eIdx = hei->idx();
			}

			hei = hei->next();
		} while (hei != fi->halfedge());

		mesh->halfedge(eIdx)->selected() = !mesh->halfedge(eIdx)->selected();
		mesh->halfedge(eIdx)->opposite()->selected() = mesh->halfedge(eIdx)->selected();
		wireFrameModule->highlightEdge(eIdx, mesh->halfedge(eIdx)->selected());
		update();
	}
}

void OGLWidget::selectVertex(int wx, int wy)
{
	QVector3D nearP, farP, d;
	camera.getFarNearPointWorld(wx, wy, nearP, farP);
	d = (farP - nearP).normalized();

	glm::vec3 rayO, rayD, hitP, normal;
	float t;
	int idx;
	rayO = glm::vec3(nearP.x(), nearP.y(), nearP.z());
	rayD = glm::vec3(d.x(), d.y(), d.z());
	bool intersected = kdTree->intersectNew(rayO, rayD, t, hitP, normal, idx);

	if (intersected){
		double distance = std::numeric_limits<double>::max();
		int vIdx;

		glm::vec3 hp(hitP.x + mesh->xcenter(), hitP.y + mesh->ycenter(), hitP.z + mesh->zcenter());
		Facet_iterator fi = mesh->facet(idx);
		Halfedge_iterator hei = fi->halfedge();
		do {
			glm::vec3 gp(hei->vertex()->point().x(), hei->vertex()->point().y(), hei->vertex()->point().z());
			double dis = glm::length(hp - gp);

			if (dis < distance){
				distance = dis;
				vIdx = hei->vertex()->idx();
			}

			hei = hei->next();
		} while (hei != fi->halfedge());
		//cout << "Vertex Idx: " << vIdx << endl;

		if (!mesh->vertex(vIdx)->selected()) {
			selectedVerts.push_back(vIdx);
			mesh->vertex(vIdx)->selected() = !mesh->vertex(vIdx)->selected();
			mesh->vertex(vIdx)->pos() = selectedVerts.size() - 1;
			addVertexHL(vertexHLModule, mesh->vertex(vIdx)->point().x(), mesh->vertex(vIdx)->point().y(), mesh->vertex(vIdx)->point().z());
		}
		else{
			//update selected pos
			selectedVerts.erase(selectedVerts.begin() + mesh->vertex(vIdx)->pos());
			//update pos
			for (int i = 0; i < selectedVerts.size(); ++i)
				mesh->vertex(selectedVerts[i])->pos() = i;
			mesh->vertex(vIdx)->selected() = !mesh->vertex(vIdx)->selected();
			delVertexHL(vertexHLModule, mesh->vertex(vIdx)->pos());
		}

		update();
	}
}