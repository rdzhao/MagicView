#pragma once

#include <QWidget>
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QFileDialog>
#include <QMessageBox>

#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QIcon>

#include <QTextStream>

#include <iostream>

#include "MyOpenGLWidget.h"

using namespace std;

class Viewer : public QMainWindow
{
	Q_OBJECT

public:
	Viewer(QWidget* parent = Q_NULLPTR);

	private slots:
	void load();

	void faceSelectionSlot();
	void edgeSelectionSlot();
	void vertSelectionSlot();

	void clearSceneSlot();

private:
	void createCanvas();
	void createMenu();
	void createStatus();
	void createToolBar();

	//widget
private:
	OGLWidget* canvas;

	// menu
private:
	QMenu *fileMenu;
	QMenu *sceneMenu;
	QMenu *selectMenu;
	QMenu *opMenu;

	QAction *newAct;
	QAction *openAct;
	QAction *saveAct;
	QAction *exitAct;

	QAction *ClearAct;

	QAction *faceAct;
	QAction *edgeAct;
	QAction *vertAct;

	QAction *operationAct_1;

private:
	QToolBar * toolBar;

	QAction *showFaceAct;
	QAction *showEdgeAct;
	QAction *showVertAct;
	
	//data
private:
	Mesh* mesh;
	Mesh* ball;
};