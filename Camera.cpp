#include "Camera.h"

void Camera::setView(QVector3D p, QVector3D c, QVector3D u)
{
	pos = p;
	center = c;
	up = u;
}

void Camera::setProject(double fov, double ar, double np, double fp)
{
	fieldOfView = fov;
	aspectRatio = ar;
	nearPlane = np;
	farPlane = fp;
}

void Camera::setWinWidth(int val)
{
	winWidth = val;
}

void Camera::setWinHeight(int val)
{
	winHeight = val;
}

void Camera::setWX(int val)
{
	winX = val;
}

void Camera::setWY(int val)
{
	winY = val;
}

void Camera::setPWX(int val)
{
	preWinX = val;
}

void Camera::setPWY(int val)
{
	preWinY = val;
}

void Camera::setPRotationIdentity()
{
	pRotation.setToIdentity();
}

void Camera::setscroll(int val)
{
	scroll = val;
}

void Camera::init()
{
	rotationMatrix.setToIdentity();
	translationMatrix.setToIdentity();
	viewMatrix.setToIdentity();
	projectionMatrix.setToIdentity();

	viewMatrix.lookAt(pos, center, up);
	projectionMatrix.perspective(fieldOfView, aspectRatio, nearPlane, farPlane);
}

void Camera::updateUnitCoord()
{
	toScreenCoord(winX, winY, unitX, unitY);
	//cout << winWidth << " " << winHeight << endl;
	//cout << initUX << " " << initUX << endl;
}

void Camera::updatePUnitCoord()
{
	toScreenCoord(preWinX, preWinY, preUnitX, preUnitY);
}

void Camera::arcballRotate()
{
	QVector3D initV(preUnitX, preUnitY, qSqrt(2 - preUnitX*preUnitX - preUnitY*preUnitY));
	QVector3D dragV(unitX, unitY, qSqrt(2 - unitX*unitX - unitY*unitY));

	//cout << "Look! " << dragV.x() << " " << dragV.y() << " " << dragV.z() << endl;

	QVector3D axis = QVector3D::crossProduct(initV, dragV);
	double angle = atan2(QVector3D::dotProduct(QVector3D::crossProduct(initV, dragV), axis), QVector3D::dotProduct(initV, dragV));
	//cout << axis.x()<<" "<<axis.y()<<" "<<axis.z()<<" "<<angle << endl;

	QMatrix4x4 rotation;
	rotation.rotate(1.5*angle * 180 / M_PI, axis);

	rotationMatrix = rotation*pRotation.inverted()*rotationMatrix;
	pRotation = rotation;
}

void Camera::zoom()
{
	//QMatrix4x4 vm = (viewMatrix*translationMatrix*rotationMatrix);

	//cout << "matrix:"<< endl;
	//cout << vm.row(0)[0] << " " << vm.row(0)[1] << " " << vm.row(0)[2] << " " << vm.row(0)[3] << endl;
	//cout << vm.row(1)[0] << " " << vm.row(1)[1] << " " << vm.row(1)[2] << " " << vm.row(1)[3] << endl;
	//cout << vm.row(2)[0] << " " << vm.row(2)[1] << " " << vm.row(2)[2] << " " << vm.row(2)[3] << endl;
	//cout << vm.row(3)[0] << " " << vm.row(3)[1] << " " << vm.row(3)[2] << " " << vm.row(3)[3] << endl;
	//cout << endl;

	//get camera position;
	QMatrix4x4 vm_inv = (viewMatrix*translationMatrix*rotationMatrix)/*.inverted()*/;
	QVector3D eye_unit = (vm_inv.column(3)).toVector3DAffine();

	//cout << eye_unit[0]<<" " << eye_unit[1] << " " << eye_unit[2]  << endl;

	QMatrix4x4 translation;
	translation.translate(scroll*0.001*eye_unit);
	
	translationMatrix *= translation;
}

void Camera::getFarNearPointWorld(int wx, int wy, QVector3D& nearP, QVector3D& farP)
{
	double sx, sy;
	toScreenCoord(wx, wy, sx, sy);
	cout << "Screen Coordinate: " << sx << " " << sy << endl;

	QVector4D sNearP, sFarP;
	sNearP = QVector4D(sx, sy, 1, 1);
	sFarP = QVector4D(sx, sy, -1, 1);

	QMatrix4x4 inv = (projectionMatrix*viewMatrix*translationMatrix*rotationMatrix).inverted();
	nearP = (inv*sNearP).toVector3DAffine();
	farP = (inv*sFarP).toVector3DAffine();
}

void Camera::rotateAroundZ(double d)
{
	QMatrix4x4 rotation;
	rotation.rotate(d, QVector3D(0, 0, 1));

	rotationMatrix = rotation*rotationMatrix;
}

void Camera::moveUnitCoordToPre()
{
	preUnitX = unitX;
	preUnitY = unitY;
}

void Camera::toScreenCoord(int wx, int wy, double& sx, double& sy)
{
	//double tx, ty;
	sx = 1.0*(2 * wx - winWidth) / winWidth;
	sy = 1.0*(2 * (winHeight - wy) - winHeight) / winHeight;

	//cout << sx << " " << sy << endl;

	clamp(sx, -1, 1);
	clamp(sy, -1, 1);
}

void Camera::clamp(double& val, double lb, double ub)
{
	if (val < lb)
		val = lb;
	if (val > ub)
		val = ub;
}