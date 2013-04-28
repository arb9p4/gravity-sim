
#ifndef Camera_H
#define Camera_H

#include <math.h>

class Camera
{
	public:
		Camera();
		virtual ~Camera();

		void updateClipping(double, double, double);
		void updateTranslation(double, double, double);
		void updateRotation(double, double, double);
		void draw();

		//Camera parameters
		double camX, camY, camZ;
		double rotX, rotY, rotZ;
		double fov, nearClip, farClip;

};

class GlWindow;

#endif // Camera_H