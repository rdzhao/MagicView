#include "Viewer.h"

Viewer::Viewer(QWidget *parent)
	: QMainWindow(parent)
{
	createMenu();
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

	SelectMenu = menuBar()->addMenu("Selection");
	SelectMenu->setFixedWidth(150);
	{
		faceAct = new QAction("Select Face", this);
		faceAct->setShortcut(Qt::Key::Key_0);
		faceAct->setCheckable(true);
		faceAct->setChecked(false);
		SelectMenu->addAction(faceAct);
		connect(faceAct, SIGNAL(triggered()), this, SLOT(uncheckEdge()));
		connect(faceAct, SIGNAL(triggered()), this, SLOT(uncheckVert()));

		edgeAct = new QAction("Select Edge", this);
		edgeAct->setShortcut(Qt::Key::Key_1);
		edgeAct->setCheckable(true);
		edgeAct->setChecked(false);
		SelectMenu->addAction(edgeAct);
		connect(edgeAct, SIGNAL(triggered()), this, SLOT(uncheckFace()));
		connect(edgeAct, SIGNAL(triggered()), this, SLOT(uncheckVert()));

		vertAct = new QAction("Select Vertex", this);
		vertAct->setShortcut(Qt::Key::Key_2);
		vertAct->setCheckable(true);
		vertAct->setChecked(false);
		SelectMenu->addAction(vertAct);
		connect(vertAct, SIGNAL(triggered()), this, SLOT(uncheckFace()));
		connect(vertAct, SIGNAL(triggered()), this, SLOT(uncheckEdge()));
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