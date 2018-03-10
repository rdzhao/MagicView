#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include <QMouseEvent>

#include <CGAL/Simple_cartesian.h>
//#include <CGAL/Polyhedron_incremental_builder_3.h>

#include <limits>
#include <chrono>

#include "RenderModule.h"
#include "Camera.h"

#include "Polyhedron.h"
#include "WavefrontObjParser.h"
#include "KDTreeCPU.h"

typedef double FT;
typedef CGAL::Simple_cartesian<FT> Kernel;
typedef Kernel::Point_3 Point_3;
typedef Kernel::Vector_3 Vector_3;
typedef Enriched_polyhedron<Kernel, Enriched_items> Mesh;
typedef Mesh::HalfedgeDS HalfedgeDS;
typedef Mesh::Vertex_iterator Vertex_iterator;
typedef Mesh::Edge_iterator Edge_iterator;
typedef Mesh::Halfedge_iterator Halfedge_iterator;
typedef Mesh::Facet_iterator Facet_iterator;
typedef Mesh::Halfedge_around_facet_circulator Halfedge_around_facet_circulator;

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
	void mouseDoubleClickEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);

	void clear();

	void setMesh(Mesh* m);
	void setBall(Mesh* b);
	void setKDTree();
	void setRenderContexts();
	void setCamera();

	void setMeshModule(RenderModule* rm);
	void setWireFrameModule(RenderModule* rm);
	void addVertexHL(RenderModule* rm, float x, float y, float z);
	void delVertexHL(RenderModule* rm, int pos);

	void setFaceSelection(bool b);
	void setEdgeSelection(bool b);
	void setVertSelection(bool b);

	void selectFace(int wx, int wy);
	void selectEdge(int wx, int wy);
	void selectVertex(int wx, int wy);

private:
	void printContextInformation();

private:
	std::vector<RenderModule*> rModules;
	RenderModule* meshModule;
	RenderModule* wireFrameModule;
	RenderModule* vertexHLModule;

	Camera camera;

	bool leftPressed; // left button pressed or not
	bool rightPressed; // right button pressed or not

	Mesh* mesh;
	Mesh* ball; // for highlighting vertex
	KDTreeCPU* kdTree;

	//status
private:
	bool faceSelection;
	bool edgeSelection;
	bool vertSelection;
	std::vector<int> selectedVerts;
};