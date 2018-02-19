#include "Viewer.h"

Viewer::Viewer(QWidget *parent)
	: QMainWindow(parent)
{
	createMenu();
	createStatus();
	createToolBar();
	createCanvas();
}

void Viewer::createMenu()
{
	fileMenu = menuBar()->addMenu("File");
	fileMenu->setFixedWidth(150);
	{
		newAct = new QAction("New", this);
		newAct->setShortcuts(QKeySequence::New);
		fileMenu->addAction(newAct);

		openAct = new QAction("Open", this);
		openAct->setShortcuts(QKeySequence::Open);
		fileMenu->addAction(openAct);

		saveAct = new QAction("Save", this);
		saveAct->setShortcuts(QKeySequence::Save);
		fileMenu->addAction(saveAct);

		exitAct = new QAction("Exit", this);
		exitAct->setShortcuts(QKeySequence::Quit);
		fileMenu->addAction(exitAct);
		connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
	}

	selectMenu = menuBar()->addMenu("Selection");
	selectMenu->setFixedWidth(150);
	{
		faceAct = new QAction("Select Face", this);
		faceAct->setShortcut(Qt::Key::Key_0);
		faceAct->setCheckable(true);
		faceAct->setChecked(false);
		selectMenu->addAction(faceAct);
		connect(faceAct, SIGNAL(triggered()), this, SLOT(uncheckEdge()));
		connect(faceAct, SIGNAL(triggered()), this, SLOT(uncheckVert()));

		edgeAct = new QAction("Select Edge", this);
		edgeAct->setShortcut(Qt::Key::Key_1);
		edgeAct->setCheckable(true);
		edgeAct->setChecked(false);
		selectMenu->addAction(edgeAct);
		connect(edgeAct, SIGNAL(triggered()), this, SLOT(uncheckFace()));
		connect(edgeAct, SIGNAL(triggered()), this, SLOT(uncheckVert()));

		vertAct = new QAction("Select Vertex", this);
		vertAct->setShortcut(Qt::Key::Key_2);
		vertAct->setCheckable(true);
		vertAct->setChecked(false);
		selectMenu->addAction(vertAct);
		connect(vertAct, SIGNAL(triggered()), this, SLOT(uncheckFace()));
		connect(vertAct, SIGNAL(triggered()), this, SLOT(uncheckEdge()));
	}

	opMenu = menuBar()->addMenu("Operation");
	opMenu->setFixedWidth(150);
	{
		operationAct_1 = new QAction("Operation_1", this);
		operationAct_1->setShortcut(tr("Crtl+O"));
		opMenu->addAction(operationAct_1);
	}
}

void Viewer::createCanvas()
{
	OGLWidget* canvas = new OGLWidget();
	setCentralWidget(canvas);

	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setVersion(3, 3);
	format.setSamples(4);

	canvas->setFormat(format);
	canvas->show();
}

void Viewer::createStatus()
{
	statusBar()->show();
}

void Viewer::createToolBar()
{
	toolBar = addToolBar("Tools");
	toolBar->setMinimumHeight(30);
	toolBar->setMinimumWidth(30);

	showFaceAct = new QAction(this);
	showFaceAct->setIcon(QIcon("Resources/1.png"));
	toolBar->addAction(showFaceAct);

	showEdgeAct = new QAction(this);
	showEdgeAct->setIcon(QIcon("Resources/2.png"));
	toolBar->addAction(showEdgeAct);

	showVertAct = new QAction(this);
	showVertAct->setIcon(QIcon("Resources/3.png"));
	toolBar->addAction(showVertAct);
}

// Slots
void Viewer::uncheckFace()
{
	faceAct->setChecked(false);
}

void Viewer::uncheckEdge()
{
	edgeAct->setChecked(false);
}

void Viewer::uncheckVert()
{
	vertAct->setChecked(false);
}