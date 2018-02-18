#pragma once

#include <QWidget>
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QMenuBar>
#include <QStatusBar>
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

	// menu action
private:
	QMenu* fileMenu;
	QMenu* SelectMenu;

	QAction *newAct;
	QAction *openAct;
	QAction *saveAct;
	QAction *exitAct;

	QAction *faceAct;
	QAction *edgeAct;
	QAction *vertAct;
	
};