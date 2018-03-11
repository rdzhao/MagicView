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

void Camera::setPTranslationIdentity()
{
	pTranslation.setToIdentity();
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

	zoomRatio = 1;
}

void Camera::updateUnitCoord()
{
	toScreenCoord(winX, winY, unitX, unitY);
}

void Camera::updatePUnitCoord()
{
	toScreenCoord(preWinX, preWinY, preUnitX, preUnitY);
}

void Camera::move(double approxBBSize)
{
	QVector3D initV(preUnitX, preUnitY, 0);
	QVector3D dragV(unitX, unitY, 0);
	QVector4D dirV = QVector4D(dragV - initV, 1);

	QMatrix4x4 vm_inv = (viewMatrix*rotationMatrix*translationMatrix)/*.inverted()*/;
	QVector3D cam_pos = (vm_inv.column(3)).toVector3DAffine();
	cam_pos = (rotationMatrix*translationMatrix).inverted()*cam_pos;

	QMatrix4x4 translation;
	translation.translate(((rotationMatrix.inverted()*dirV).toVector3DAffine())*approxBBSize);
	
	translationMatrix = translation*pTranslation.inverted()*translationMatrix;
	pTranslation = translation;
}

void Camera::arcballRotate()
{
	QVector3D initV(preUnitX, preUnitY, qSqrt(2 - preUnitX*preUnitX - preUnitY*preUnitY));
	QVector3D dragV(unitX, unitY, qSqrt(2 - unitX*unitX - unitY*unitY));

	QVector3D axis = QVector3D::crossProduct(initV, dragV);
	double angle = atan2(QVector3D::dotProduct(QVector3D::crossProduct(initV, dragV), axis), QVector3D::dotProduct(initV, dragV));

	QMatrix4x4 rotation;
	rotation.rotate(1.5*angle * 180 / M_PI, axis);

	rotationMatrix = rotation*pRotation.inverted()*rotationMatrix;
	pRotation = rotation;
}

void Camera::zoom()
{
	//get camera position;
	QMatrix4x4 vm_inv = (viewMatrix*rotationMatrix*translationMatrix)/*.inverted()*/;
	QVector3D cam_pos = (vm_inv.column(3)).toVector3DAffine();
	cam_pos = (rotationMatrix*translationMatrix).inverted()*cam_pos;

	QMatrix4x4 translation;

	double moveLen = (zoomRatio*pow(0.9, -scroll / 120) - zoomRatio)*cam_pos.length();
	if (scroll < 0 && moveLen>-0.001)
		moveLen = 0;
	zoomRatio *= pow(0.9, -scroll / 120);
	//cout << moveLen << endl;
	translation.translate(cam_pos.normalized()*moveLen);
	translationMatrix *= translation;
}

void Camera::getFarNearPointWorld(int wx, int wy, QVector3D& nearP, QVector3D& farP)
{
	double sx, sy;
	toScreenCoord(wx, wy, sx, sy);
	cout << "Screen Coordinate: " << sx << " " << sy << endl;

	QVector4D sNearP, sFarP;
	sNearP = QVector4D(sx, sy, -1, 1);
	sFarP = QVector4D(sx, sy, 1, 1);

	QMatrix4x4 inv = (projectionMatrix*viewMatrix*rotationMatrix*translationMatrix).inverted();
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