
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
		void setTranslation(double, double, double);
		void setTarget(double, double, double);
		void draw();

		//Camera parameters
		double camX, camY, camZ;
		double rotX, rotY, rotZ;
		double fov, nearClip, farClip;
		double dx, dy, dz;


		//Tweening
		double camXtarget, camYtarget, camZtarget;

		double camDist;
		double camDistTarget;

		bool showInfoOverlay;	//Display diagnostic information on the screen
		bool showGrid;	//Display the grid
		bool showForceGrid;	//Display the force grid
		bool showTrails;	//True if trails should be drawn
		bool showProxy;

		int cameraType;

};

class GlWindow;

#endif // Camera_H