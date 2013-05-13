#include "Camera.h"


Camera::Camera()
{
	//Initialize camera position
	camX = 0.0;
	camY = -20.0;
	camZ = -15.0;

    //Initialize camera rotation
	rotX = 45.0;
	rotY = -45.0;
	rotZ = 0.0;

    //Initialize clipping planes
	fov = 60.0;
	nearClip = 0.1;
	farClip = 1000000.0;

	dx = dy = dz = 0.0;


	camXtarget = camYtarget = camZtarget = 0.0;

	showInfoOverlay = false;	//Display diagnostic information on the screen
	showGrid = true;	//Display the grid
	showForceGrid = false;	//Display the force grid
	showTrails = true;	//True if trails should be drawn
	showProxy = true;	//True if the proxy should be drawn

	camDist = 5.0;

	isStatic = false;

}

void Camera::updateClipping(double fov1, double nearClip1, double farClip1) {
	fov += fov1;
	nearClip *= nearClip1;
	farClip *= farClip1;
}

void Camera::updateRotation(double rotX1, double rotY1, double rotZ1) {
	rotX += rotX1;
	rotY += rotY1;
	rotZ += rotZ1;
}

void Camera::updateTranslation(double camX1, double camY1, double camZ1) {
	camX += camX1;
	camY += camY1;
	camZ += camZ1;
}

void Camera::setTranslation(double camX1, double camY1, double camZ1) {
	camX = camX1;
	camY = camY1;
	camZ = camZ1;
}

void Camera::setTarget(double camX1, double camY1, double camZ1) {
	camXtarget = camX1;
	camYtarget = camY1;
	camZtarget = camZ1;
}

Camera::~Camera(void)
{
	
}

