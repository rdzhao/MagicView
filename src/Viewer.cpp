#include "Viewer.h"

Viewer::Viewer(QWidget *parent)
	: QMainWindow(parent)
{
	createCanvas();
	createMenu();
	createStatus();
	createToolBar();
}

void Viewer::createMenu()
{
	menuBar()->setMinimumHeight(25);

	fileMenu = menuBar()->addMenu("File");
	fileMenu->setFixedWidth(250);
	{
		newAct = new QAction("New", this);
		newAct->setShortcuts(QKeySequence::New);
		fileMenu->addAction(newAct);

		openAct = new QAction("Open", this);
		openAct->setShortcuts(QKeySequence::Open);
		fileMenu->addAction(openAct);
		connect(openAct, SIGNAL(triggered()), this, SLOT(load()));

		saveAct = new QAction("Save", this);
		saveAct->setShortcuts(QKeySequence::Save);
		fileMenu->addAction(saveAct);

		exitAct = new QAction("Exit", this);
		exitAct->setShortcuts(QKeySequence::Quit);
		fileMenu->addAction(exitAct);
		connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
	}

	sceneMenu = menuBar()->addMenu("Scene");
	sceneMenu->setFixedWidth(250);
	{
		ClearAct = new QAction("Clear Scene", this);
		ClearAct->setShortcut(QKeySequence(tr("Ctrl+P")));
		sceneMenu->addAction(ClearAct);
		connect(ClearAct, SIGNAL(triggered()), this, SLOT(clearSceneSlot()));
	}

	opMenu = menuBar()->addMenu("Operation");
	opMenu->setFixedWidth(250);
	{
		operationAct_1 = new QAction("Operation_1", this);
		operationAct_1->setShortcut(tr("Crtl+O"));
		opMenu->addAction(operationAct_1);
	}
}

void Viewer::createCanvas()
{
	canvas = new OGLWidget();
	setCentralWidget(canvas);

	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setVersion(3, 3);
	format.setSamples(4);

	canvas->setFormat(format);
	canvas->show();

	string fn = "../../resources/ball.obj";
	ObjReader<Kernel, Enriched_items> reader(fn);
	reader.read();

	ball = new Enriched_polyhedron<Kernel, Enriched_items>;
	ObjBuilder<HalfedgeDS> builder(reader.vertices(), reader.facets());
	ball->delegate(builder);
	ball->basic_init();
	canvas->setBall(ball);
}

void Viewer::createStatus()
{
	statusBar()->show();
}

void Viewer::createToolBar()
{
	toolBar = addToolBar("Tools");
	toolBar->setMinimumHeight(50);
	toolBar->setMinimumWidth(50);
	toolBar->setIconSize(QSize(45, 45));

	tbImportAct = new QAction(this);
	tbImportAct->setIcon(QIcon("../../resources/import.png"));
	tbImportAct->setText("Import Mesh");
	toolBar->addAction(tbImportAct);
	connect(tbImportAct, SIGNAL(triggered()), this, SLOT(load()));

	tbExportAct = new QAction(this);
	tbExportAct->setIcon(QIcon("../../resources/export.png"));
	tbExportAct->setText("Export Mesh");
	toolBar->addAction(tbExportAct);

	toolBar->addSeparator();

	showFaceAct = new QAction(this);
	showFaceAct->setIcon(QIcon("../../resources/show_face.png"));
	showFaceAct->setText("Show Face");
	showFaceAct->setCheckable(true);
	showFaceAct->setChecked(true);
	toolBar->addAction(showFaceAct);
	connect(showFaceAct, SIGNAL(triggered()), this, SLOT(showFaceSlot()));

	showEdgeAct = new QAction(this);
	showEdgeAct->setIcon(QIcon("../../resources/show_edge.png"));
	showEdgeAct->setText("Show Wireframe");
	showEdgeAct->setCheckable(true);
	showEdgeAct->setChecked(false);
	toolBar->addAction(showEdgeAct);
	connect(showEdgeAct, SIGNAL(triggered()), this, SLOT(showEdgeSlot()));

	toolBar->addSeparator();

	tbSelectFaceAct = new QAction();
	tbSelectFaceAct->setIcon(QIcon("../../resources/select_face.png"));
	tbSelectFaceAct->setText("Select Face");
	tbSelectFaceAct->setCheckable(true);
	tbSelectFaceAct->setChecked(false);
	toolBar->addAction(tbSelectFaceAct);
	connect(tbSelectFaceAct, SIGNAL(triggered()), this, SLOT(faceSelectionSlot()));
	
	tbSelectEdgeAct = new QAction();
	tbSelectEdgeAct->setIcon(QIcon("../../resources/select_edge.png"));
	tbSelectEdgeAct->setText("Select Edge");
	tbSelectEdgeAct->setCheckable(true);
	tbSelectEdgeAct->setChecked(false);
	toolBar->addAction(tbSelectEdgeAct);
	connect(tbSelectEdgeAct, SIGNAL(triggered()), this, SLOT(edgeSelectionSlot()));

	tbSelectVertAct = new QAction();
	tbSelectVertAct->setIcon(QIcon("../../resources/select_vertex.png"));
	tbSelectVertAct->setText("Select Vertex");
	tbSelectVertAct->setCheckable(true);
	tbSelectVertAct->setChecked(false);
	toolBar->addAction(tbSelectVertAct);
	connect(tbSelectVertAct, SIGNAL(triggered()), this, SLOT(vertSelectionSlot()));
}

// Slots
void Viewer::load()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open"), "",
		tr("WaveFront Obj (*.obj);;All Files (*)"));

	if (fileName.isEmpty())
		return;
	else 
	{
		string fn = fileName.toLocal8Bit().constData();
		ObjReader<Kernel, Enriched_items> reader(fn);
		reader.read();

		mesh = new Enriched_polyhedron<Kernel, Enriched_items>;
		ObjBuilder<HalfedgeDS> builder(reader.vertices(), reader.facets());
		mesh->delegate(builder);
		mesh->basic_init();

		canvas->setMesh(mesh);
		canvas->setKDTree();
		canvas->setRenderContexts();
		canvas->setCamera();
		canvas->update();
	}

}

void Viewer::showFaceSlot()
{
	canvas->setFaceVisible(showFaceAct->isChecked());
}

void Viewer::showEdgeSlot()
{
	canvas->setEdgeVisible(showEdgeAct->isChecked());
}

void Viewer::faceSelectionSlot()
{
	if (tbSelectFaceAct->isChecked())
	{
		tbSelectEdgeAct->setChecked(false);
		tbSelectVertAct->setChecked(false);

		canvas->setFaceSelection(true);
		canvas->setEdgeSelection(false);
		canvas->setVertSelection(false);
	}
	else
	{
		canvas->setFaceSelection(false);
	}
}

void Viewer::edgeSelectionSlot()
{
	if (tbSelectEdgeAct->isChecked())
	{
		tbSelectFaceAct->setChecked(false);
		tbSelectVertAct->setChecked(false);

		canvas->setFaceSelection(false);
		canvas->setEdgeSelection(true);
		canvas->setVertSelection(false);
	}
	else
	{
		canvas->setEdgeSelection(false);
	}
}

void Viewer::vertSelectionSlot()
{
	if (tbSelectVertAct->isChecked())
	{
		tbSelectFaceAct->setChecked(false);
		tbSelectEdgeAct->setChecked(false);

		canvas->setFaceSelection(false);
		canvas->setEdgeSelection(false);
		canvas->setVertSelection(true);
	}
	else
	{
		canvas->setVertSelection(false);
	}
}

void Viewer::clearSceneSlot()
{
	canvas->clear();
	canvas->update();
}