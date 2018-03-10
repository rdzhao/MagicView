#pragma once

#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QtMath>
#include <iostream>

using namespace std;

class Camera
{
public:
	Camera() {};
	~Camera() {};

	//Modyfier
	void setView(QVector3D p, QVector3D c, QVector3D u);
	void setProject(double fov, double ar, double np, double fp);
	
	void setWinWidth(int val);
	void setWinHeight(int val);
	void setWX(int val);
	void setWY(int val);
	void setPWX(int val);
	void setPWY(int val);
	void setPRotationIdentity();
	void setPTranslationIdentity();

	void setscroll(int val);

	// Other functionality
	void init();
	
	void updateUnitCoord();
	void updatePUnitCoord();
	void move(double approxBBSize);
	void arcballRotate();

	void zoom();

	void getFarNearPointWorld(int wx, int wy, QVector3D& nearP, QVector3D& farP);

	// helper
	void rotateAroundZ(double d); // test
	void moveUnitCoordToPre();
	void toScreenCoord(int wx, int wy, double& sx, double& sy); // convert window coordinate to normalized screen coordinate
	void clamp(double& val, double lb, double ub);

	QMatrix4x4 rotationMatrix;
	QMatrix4x4 translationMatrix;
	QMatrix4x4 viewMatrix;
	QMatrix4x4 projectionMatrix;

private:
	QVector3D pos;
	QVector3D center;
	QVector3D up;

	double fieldOfView;
	double aspectRatio;
	double nearPlane;
	double farPlane;

	// Arcball
	int winWidth, winHeight; // window width and length 
	int winX, winY, preWinX, preWinY;
	double unitX, unitY, preUnitX, preUnitY;
	QMatrix4x4 pRotation;
	QMatrix4x4 pTranslation;

	// zoom
	int scroll;
};