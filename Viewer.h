#pragma once

#include <QWidget>
#include <QMainWindow>
#include <QOpenGLWidget>

#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>

#include <QIcon>

#include <iostream>

#include "MyOpenGLWidget.h"

using namespace std;

class Viewer : public QMainWindow
{
	Q_OBJECT

public:
	Viewer(QWidget* parent = Q_NULLPTR);

	private slots:
	void uncheckFace();
	void uncheckEdge();
	void uncheckVert();

private:
	void createMenu();
	void createCanvas();
	void createStatus();
	void createToolBar();

	// menu
private:
	QMenu* fileMenu;
	QMenu* selectMenu;
	QMenu* opMenu;

	QAction *newAct;
	QAction *openAct;
	QAction *saveAct;
	QAction *exitAct;

	QAction *faceAct;
	QAction *edgeAct;
	QAction *vertAct;

	QAction *operationAct_1;

private:
	QToolBar * toolBar;

	QAction *showFaceAct;
	QAction *showEdgeAct;
	QAction *showVertAct;

};