#include "Camera.h"


Camera::Camera()
{
	//Initialize camera position
	camX = 0.0;
	camY = -20.0;
	camZ = -15.0;

    //Initialize camera rotation
	rotX = 60.0;
	rotY = 0.0;
	rotZ = 0.0;

    //Initialize clipping planes
	fov = 60.0;
	nearClip = 0.1;
	farClip = 1000000.0;

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


Camera::~Camera(void)
{
	
}

