/*
 *	GlWindow.cpp
 *
 *	Created on : Apr 10, 2013
 *	Course     : CS7610
 *	Instructor : Kannappan Palaniappan
 *	Author	   : Andrew Buck
 *	Purpose    : Main OpenGL window class
 *
 *	Copyright University of Missouri-Columbia
 */

#include "GlWindow.h"
#include <iostream>
#include <vector>
using namespace std;

#define PI_180 0.01745329251994329576923690768489

//Called each frame to update variables and redraw the screen
void animate(void* pData) {
	if (pData != NULL) {

        //Get pointer to the window class
		GlWindow* pWindow = reinterpret_cast<GlWindow*>(pData);

		//Update mouse history
        double mX = pWindow->mouseX - pWindow->mouseX2;
        double mY = pWindow->mouseY - pWindow->mouseY2;
        pWindow->mouseX2 = pWindow->mouseX;
        pWindow->mouseY2 = pWindow->mouseY;

		// clipping variables
		double fov = pWindow->fovSensitivity*pWindow->keyMinus - pWindow->fovSensitivity*pWindow->keyPlus;
		double nearClip = (1 + pWindow->clipSensitivity*pWindow->keyRBracket*pWindow->shiftKey -
								  pWindow->clipSensitivity*pWindow->keyLBracket*pWindow->shiftKey);
		double farClip = (1 + pWindow->clipSensitivity*pWindow->keyRBracket*(1 - pWindow->shiftKey) -
								 pWindow->clipSensitivity*pWindow->keyLBracket*(1 - pWindow->shiftKey));

        // rotation variables
		double rotX = pWindow->rotSpeed*pWindow->keyX - pWindow->rotSpeed*pWindow->keyC + mY*pWindow->mouseSensitivity;
		double rotY = pWindow->rotSpeed*pWindow->keyV - pWindow->rotSpeed*pWindow->keyZ + mX*pWindow->mouseSensitivity;
		double rotZ = pWindow->rotSpeed*pWindow->keyE - pWindow->rotSpeed*pWindow->keyQ;

        // translation variables
        double dX = pWindow->moveSpeed*pWindow->keyA - pWindow->moveSpeed*pWindow->keyD;
        double dY = pWindow->moveSpeed*pWindow->keyF - pWindow->moveSpeed*pWindow->keyR;
        double dZ = pWindow->moveSpeed*pWindow->keyW - pWindow->moveSpeed*pWindow->keyS;
		double camX = dX*cos(PI_180*pWindow->cam1.rotY) - dZ*sin(PI_180*pWindow->cam1.rotY);
		double camY = dY + dZ*sin(PI_180*pWindow->cam1.rotX);
		double camZ = dZ*cos(PI_180*pWindow->cam1.rotY) + dX*sin(PI_180*pWindow->cam1.rotY);
		
		//Update clipping planes
		pWindow->cam1.updateClipping(fov, nearClip, farClip);

		//Update camera translation
		pWindow->cam1.updateTranslation(camX, camY, camZ);
		
		//Update camera rotation
		if (pWindow->secWin) {
			pWindow->cam2.updateRotation(rotX, rotY, rotZ);
		} else {
			pWindow->cam1.updateRotation(rotX, rotY, rotZ);
		}

        pWindow->theUniverse.addTime(pWindow->timestep);

		// simple check to see if there is a planet, then follows the first planet in the list.
		if (pWindow->theUniverse.objList.size() > 0) {
			pWindow->cam2.camXtarget = pWindow->theUniverse.objList.front().Xpos;
			pWindow->cam2.camYtarget = pWindow->theUniverse.objList.front().Ypos;
			pWindow->cam2.camZtarget = pWindow->theUniverse.objList.front().Zpos;

			//Tweening
			double tweenRate = 0.5;
			pWindow->cam2.camX += (pWindow->cam2.camXtarget - pWindow->cam2.camX) * tweenRate;
			pWindow->cam2.camY += (pWindow->cam2.camYtarget - pWindow->cam2.camY) * tweenRate;
			pWindow->cam2.camZ += (pWindow->cam2.camZtarget - pWindow->cam2.camZ) * tweenRate;
			pWindow->camDist1 += (pWindow->camDist1target - pWindow->camDist1) * tweenRate;
			pWindow->camDist2 += (pWindow->camDist2target - pWindow->camDist2) * tweenRate;

			pWindow->cam2.dx = pWindow->theUniverse.objList.front().dXpos;
			pWindow->cam2.dy = pWindow->theUniverse.objList.front().dYpos;
			pWindow->cam2.dz = pWindow->theUniverse.objList.front().dZpos;
			//cout << pWindow->theUniverse.objList.front().dXpos << ", "<<
			//	pWindow->theUniverse.objList.front().dYpos << ", " <<
			//	pWindow->theUniverse.objList.front().dZpos << endl;
		}
		else {
			pWindow->cam2.camX = 0;
			pWindow->cam2.camY = 0;
			pWindow->cam2.camZ = 0;
		}

		// debug statements
		//cout << "rotX = " << rotX << ", rotY = " << rotY << ", rotZ = " << rotZ << endl;

        //Redraw and reset timer
		pWindow->redraw();
		Fl::repeat_timeout(1.0/30.0, animate, pWindow);
	}
}

//Initialize window
//Do this on init or when window's size is changed
void GlWindow::initialize(int W,int H) {

    //Initialize viewport
	glViewport(0,0,W,H);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Create light source
	GLfloat light_position[] = { 10.0, 20.0, 30.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glDisable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	
}

double normalize(double x, double y, double z) {
	return sqrt((x*x) + (y*y) + (z*z));
}

// split out drawing function so that you could draw multiple cameras,
// then specifiy lookAt = true to follow a planet.
void GlWindow::displayMe(Camera camNew, bool lookAt, bool showInfoOverlay, double camDist) {
	
    //Initialize projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camNew.fov, double(w())/double(h()), camNew.nearClip, camNew.farClip);
	

    //Initialize modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (lookAt) {
		// TODO fix this
		gluLookAt(
			cam2.camX, cam2.camY, cam2.camZ+camDist, 
			cam2.camX, cam2.camY, cam2.camZ, 
			0, 1, 0);
		glTranslatef(cam2.camX, cam2.camY, cam2.camZ);
		//glRotatef(camNew.rotZ, 0.0, 0.0, 1.0);
		glRotatef(camNew.rotX, 1.0, 0.0, 0.0);
		glRotatef(camNew.rotY, 0.0, 1.0, 0.0);
		
		glTranslatef(-cam2.camX, -cam2.camY, -cam2.camZ);
			
			//glTranslatef(camNew.camX, camNew.camY, camNew.camZ);


		/*

		// TODO fix this

		// normalize velocity
		// - noramilzed velocty
		double normalizedVector = normalize(cam2.dx, cam2.dy, cam2.dz);
		normalizedVector += 0.000000000000000000000001;
		double x = cam2.camX - (cam2.dx/normalizedVector);
		double y = cam2.camY - (cam2.dy/normalizedVector);
		double z = cam2.camZ - (cam2.dz/normalizedVector);
		
		cout << normalizedVector << endl;

		if (theUniverse.objList.size() > 0) {
			gluLookAt(
				x, y, z, 
				cam2.camX, cam2.camY, cam2.camZ, 
				0, 1, 0);
		} else {
			gluLookAt(
				cam2.camX, cam2.camY, cam2.camZ+5.0, 
				cam2.camX, cam2.camY, cam2.camZ, 
				0, 1, 0);

			glTranslatef(cam2.camX, cam2.camY, cam2.camZ);
		glRotatef(camNew.rotX, 1.0, 0.0, 0.0);
		glRotatef(camNew.rotY, 0.0, 1.0, 0.0);
		glTranslatef(-cam2.camX, -cam2.camY, -cam2.camZ);
		}
		/*
		glTranslatef(cam2.camX, cam2.camY, cam2.camZ);
		glRotatef(camNew.rotX, 1.0, 0.0, 0.0);
		glRotatef(camNew.rotY, 0.0, 1.0, 0.0);
		glTranslatef(-cam2.camX, -cam2.camY, -cam2.camZ);
		*/
			
			//glTranslatef(camNew.camX, camNew.camY, camNew.camZ);
		
	} else {
		//Move camera
		glRotatef(camNew.rotZ, 0.0, 0.0, 1.0);
		glRotatef(camNew.rotX, 1.0, 0.0, 0.0);
		glRotatef(camNew.rotY, 0.0, 1.0, 0.0);
		glTranslatef(camNew.camX, camNew.camY, camNew.camZ);
	}

    //Draw grid
	if(showGrid) {
		
		glBegin(GL_LINES);
		
		
		glColor3f(0.3, 0.3, 0.4);
		for(GLfloat i = -100.0; i <= 100.0; ++i) {
			glVertex3f(i, 0.0, -100.0);
			glVertex3f(i, 0.0, 100.0);
		}
		for(GLfloat i = -100.0; i <= 100.0; ++i) {
			glVertex3f(-100.0, 0.0, i);
			glVertex3f(100.0, 0.0, i);
		}
		
		glEnd();
		
		//Draw axes
		double axisWidth = 0.03;
		double axisLength = 100;

		glBegin(GL_QUADS);
		glColor3f(0.5, 0.5, 0.5);
		glVertex3f(-axisWidth, -axisWidth, -axisWidth);
		glVertex3f(-axisWidth,  axisWidth, -axisWidth);
		glVertex3f(-axisWidth,  axisWidth,  axisWidth);
		glVertex3f(-axisWidth, -axisWidth,  axisWidth);
		glVertex3f(-axisWidth, -axisWidth, -axisWidth);
		glVertex3f( axisWidth, -axisWidth, -axisWidth);
		glVertex3f( axisWidth, -axisWidth,  axisWidth);
		glVertex3f(-axisWidth, -axisWidth,  axisWidth);
		glVertex3f(-axisWidth, -axisWidth, -axisWidth);
		glVertex3f( axisWidth, -axisWidth, -axisWidth);
		glVertex3f( axisWidth,  axisWidth, -axisWidth);
		glVertex3f(-axisWidth,  axisWidth, -axisWidth);
		glEnd();

		glColor3f(0.6, 0.3, 0.3);
		glBegin(GL_QUAD_STRIP);
		glVertex3f(axisWidth , -axisWidth, -axisWidth);
		glVertex3f(axisLength, -axisWidth, -axisWidth);
		glVertex3f(axisWidth ,  axisWidth, -axisWidth);
		glVertex3f(axisLength,  axisWidth, -axisWidth);
		glVertex3f(axisWidth ,  axisWidth,  axisWidth);
		glVertex3f(axisLength,  axisWidth,  axisWidth);
		glVertex3f(axisWidth , -axisWidth,  axisWidth);
		glVertex3f(axisLength, -axisWidth,  axisWidth);
		glVertex3f(axisWidth , -axisWidth, -axisWidth);
		glVertex3f(axisLength, -axisWidth, -axisWidth);
		glEnd();
		glBegin(GL_QUADS);
		glVertex3f(axisLength, -axisWidth, -axisWidth);
		glVertex3f(axisLength,  axisWidth, -axisWidth);
		glVertex3f(axisLength,  axisWidth,  axisWidth);
		glVertex3f(axisLength, -axisWidth,  axisWidth);
		glEnd();

		glColor3f(0.3, 0.6, 0.3);
		glBegin(GL_QUAD_STRIP);
		glVertex3f(-axisWidth, axisWidth , -axisWidth);
		glVertex3f(-axisWidth, axisLength, -axisWidth);
		glVertex3f( axisWidth, axisWidth , -axisWidth);
		glVertex3f( axisWidth, axisLength, -axisWidth);
		glVertex3f( axisWidth, axisWidth ,  axisWidth);
		glVertex3f( axisWidth, axisLength,  axisWidth);
		glVertex3f(-axisWidth, axisWidth ,  axisWidth);
		glVertex3f(-axisWidth, axisLength,  axisWidth);
		glVertex3f(-axisWidth, axisWidth , -axisWidth);
		glVertex3f(-axisWidth, axisLength, -axisWidth);
		glEnd();
		glBegin(GL_QUADS);
		glVertex3f(-axisWidth, axisLength, -axisWidth);
		glVertex3f( axisWidth, axisLength, -axisWidth);
		glVertex3f( axisWidth, axisLength,  axisWidth);
		glVertex3f(-axisWidth, axisLength,  axisWidth);
		glEnd();

		glColor3f(0.3, 0.3, 0.6);
		glBegin(GL_QUAD_STRIP);
		glVertex3f(-axisWidth, -axisWidth, axisWidth );
		glVertex3f(-axisWidth, -axisWidth, axisLength);
		glVertex3f( axisWidth, -axisWidth, axisWidth );
		glVertex3f( axisWidth, -axisWidth, axisLength);
		glVertex3f( axisWidth,  axisWidth, axisWidth );
		glVertex3f( axisWidth,  axisWidth, axisLength);
		glVertex3f(-axisWidth,  axisWidth, axisWidth );
		glVertex3f(-axisWidth,  axisWidth, axisLength);
		glVertex3f(-axisWidth, -axisWidth, axisWidth );
		glVertex3f(-axisWidth, -axisWidth, axisLength);
		glEnd();
		glBegin(GL_QUADS);
		glVertex3f(-axisWidth, -axisWidth, axisLength);
		glVertex3f( axisWidth, -axisWidth, axisLength);
		glVertex3f( axisWidth,  axisWidth, axisLength);
		glVertex3f(-axisWidth,  axisWidth, axisLength);
		glEnd();
		
	}

	if(showForceGrid) {

		double forceGridOffset = -(forceGridSize-1)*forceGridResolution/2.0;
		

		double offsetX = 0;
		double offsetY = 0;
		double offsetZ = 0;

		if(theUniverse.objList.size() > 0 && 0) {
			offsetX = theUniverse.objList.front().Xpos;
			offsetY = theUniverse.objList.front().Ypos;
			offsetZ = theUniverse.objList.front().Zpos;
		}


		std::vector<std::vector<double> > forces(forceGridSize, std::vector<double>(forceGridSize, 0));
		
		double f;
		for(int i = 0; i < forceGridSize; ++i) {
			for(int j = 0; j < forceGridSize; ++j) {
				f = theUniverse.computeForce(offsetX+forceGridOffset+i*forceGridResolution,offsetY,
					                         offsetZ+forceGridOffset+j*forceGridResolution);
				//forces[i][j] = max(-2*sqrt(sqrt(abs(f))), -100);
				forces[i][j] = -log(10*abs(f)+1);
				//forces[i][j] = f;
			}
		}

		//Smooth grid
		std::vector<std::vector<double> > forcesOld(forces);
		int count;
		for(int i = 0; i < forceGridSize; ++i) {
			for(int j = 0; j < forceGridSize; ++j) {
				
				f = 0;
				count = 0;
				for(int ii = max(i-1,0); ii < min(i+1,forceGridSize); ++ii) {
					for(int jj = max(j-1,0); jj < min(j+1,forceGridSize); ++jj) {
						f += forcesOld[ii][jj];
						count++;
					}
				}

				forces[i][j] = f/count;

			}
		}

		glColor3f(0.3, 0.4, 0.3);
		for(int i = 0; i < forceGridSize; ++i) {
			glBegin(GL_LINE_STRIP);
			for(int j = 0; j < forceGridSize; ++j) {
				glVertex3f(offsetX+forceGridOffset+i*forceGridResolution, offsetY-1+forces[i][j], 
					       offsetZ+forceGridOffset+j*forceGridResolution);
			}
			glEnd();
		}
		for(int j = 0; j < forceGridSize; ++j) {
			glBegin(GL_LINE_STRIP);
			for(int i = 0; i < forceGridSize; ++i) {
				glVertex3f(offsetX+forceGridOffset+i*forceGridResolution, offsetY-1+forces[i][j], 
					       offsetZ+forceGridOffset+j*forceGridResolution);
			}
			glEnd();
		}
		

	}

	/* Turns out this may not be necessary after all, but could come in handy later

	//Draw selectors
	theUniverse.drawSelectors();

	*/

	GLint viewport[4];                  // Where The Viewport Values Will Be Stored
	glGetIntegerv(GL_VIEWPORT, viewport);           // Retrieves The Viewport Values (X, Y, Width, Height)

	GLdouble modelview[16];                 // Where The 16 Doubles Of The Modelview Matrix Are To Be Stored
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);       // Retrieve The Modelview Matrix

	GLdouble projection[16];                // Where The 16 Doubles Of The Projection Matrix Are To Be Stored
	glGetDoublev(GL_PROJECTION_MATRIX, projection);     // Retrieve The Projection Matrix

	

	

	//Draw objects
	theUniverse.draw(showTrails);

	if(showGrid)
		glColor4f(0.3, 0.3, 0.4, 0.4);	//Give selection plane some transparency
	else
		glColor4f(0.3, 0.3, 0.4, 0.0);	//Make selection plane completely invisible

	glBegin(GL_QUADS);
		glVertex3f(-100, 0, -100);
		glVertex3f(-100, 0, 100);
		glVertex3f(100, 0, 100);
		glVertex3f(100, 0, -100);
	glEnd();

	if(updateFocus) {
		//Get the screen coordinates of the mouse cursor
		GLfloat cursorWinX, cursorWinY, cursorWinZ;
		cursorWinX = cursorX;
		cursorWinY = viewport[3] - cursorY;
		glReadPixels(cursorWinX, cursorWinY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &cursorWinZ);

		//Get the world coodrdinate of the mouse cursor
		GLdouble cursorPosX, cursorPosY, cursorPosZ;              // Hold The Final Values
		gluUnProject( cursorWinX, cursorWinY, cursorWinZ, modelview, projection, viewport, &cursorPosX, &cursorPosY, &cursorPosZ);

		theUniverse.selectObject(cursorPosX, cursorPosY, cursorPosZ);

		updateFocus = false;
	}

	if(addObj > 0) {

		GLint viewport[4];                  // Where The Viewport Values Will Be Stored
		glGetIntegerv(GL_VIEWPORT, viewport);           // Retrieves The Viewport Values (X, Y, Width, Height)

		GLdouble modelview[16];                 // Where The 16 Doubles Of The Modelview Matrix Are To Be Stored
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);       // Retrieve The Modelview Matrix

		GLdouble projection[16];                // Where The 16 Doubles Of The Projection Matrix Are To Be Stored
		glGetDoublev(GL_PROJECTION_MATRIX, projection);     // Retrieve The Projection Matrix


 
		GLfloat winX, winY, winZ;               // Holds Our X, Y and Z Coordinates
		GLfloat winX2, winY2, winZ2;

		winX = clickX;                  // Holds The Mouse X Coordinate
		winY = viewport[3] - clickY;                  // Holds The Mouse Y Coordinate
		glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
		//winZ = 0.99;

		winX2 = clickX2;
		winY2 = viewport[3] - clickY2;
		glReadPixels(winX2, winY2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ2);

		GLdouble posX, posY, posZ;              // Hold The Final Values
		GLdouble posX2, posY2, posZ2;

		gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
		gluUnProject( winX2, winY2, winZ2, modelview, projection, viewport, &posX2, &posY2, &posZ2);

		double speedScale = 0.1;

		



		if(addObj == 1) {

			//Create proxy object
			theUniverse.setProxy(posX,posY,posZ,1.0);
			addObj = 2;

		}
		else if(addObj == 2) {
			if(shiftKey) {
				theUniverse.proxy.mass *= 1.1;
				theUniverse.proxy.updateRadius();
			}
			theUniverse.setProxyVector(posX2,posY2,posZ2);
		}
		else if(addObj == 3) {

			double posX = theUniverse.proxy.Xpos;
			double posY = theUniverse.proxy.Ypos;
			double posZ = theUniverse.proxy.Zpos;

			double dx = (posX2 - posX)*speedScale;
			double dy = (posY2 - posY)*speedScale;
			double dz = (posZ2 - posZ)*speedScale;

			double m = theUniverse.proxy.mass;

			theUniverse.addObject(posX, posY, posZ, dx, 0, dz, m);
			theUniverse.clearProxy();

			addObj = 0;
		}
	}

	//Display info on screen
	if(showInfoOverlay) {

		//Prepare camera for text overlay
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, w(), h(), 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glColor3f(1.0, 1.0, 1.0);

		char buffer[128];
		int lineHeight = 12;
		int currentLine = lineHeight;

		//Display camera coordinates
		sprintf(buffer, "Camera Position:");
		printString(2, currentLine, buffer);

		currentLine += lineHeight;
		sprintf(buffer, "X: %.2f   Y: %.2f   Z: %.2f", -cam1.camX, -cam1.camY, -cam1.camZ);
		printString(2, currentLine, buffer);

		currentLine += lineHeight;
		sprintf(buffer, "Yaw: %.2f   Pitch: %.2f   Roll: %.2f", cam1.rotY, -cam1.rotX, cam1.rotZ);
		printString(2, currentLine, buffer);

		currentLine += lineHeight;
		sprintf(buffer, "FOV: %.2f   Near: %.2f   Far: %.2f", cam1.fov, cam1.nearClip, cam1.farClip);
		printString(2, currentLine, buffer);

		currentLine += lineHeight;
		sprintf(buffer, "# of Objects: %i", theUniverse.objList.size()-1);
		printString(2, currentLine, buffer);

		currentLine += lineHeight;
		sprintf(buffer, "Simulation Speed: %.3f", timestep);
		printString(2, currentLine, buffer);

		currentLine += lineHeight;
		sprintf(buffer, "Click Point: %.0f %.0f", clickX, clickY);
		printString(2, currentLine, buffer);

		currentLine += lineHeight;
		sprintf(buffer, "Click2 Point: %.0f %.0f", clickX2, clickY2);
		printString(2, currentLine, buffer);

		currentLine += lineHeight;
		sprintf(buffer, "Mouse Point: %.0f %.0f", mouseX, mouseY);
		printString(2, currentLine, buffer);

		currentLine += lineHeight;
		sprintf(buffer, "Mouse2 Point: %.0f %.0f", mouseX2, mouseY2);
		printString(2, currentLine, buffer);

		currentLine += lineHeight;
		sprintf(buffer, "Cursor Point: %.0f %.0f", cursorX, cursorY);
		printString(2, currentLine, buffer);

		currentLine += lineHeight;
		sprintf(buffer, "addObj: %d", addObj);
		printString(2, currentLine, buffer);
	}
}

//Draw method
void GlWindow::draw() {
	if (!valid()) { valid(1); initialize(w(), h()); }      // first time? init
	//Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw original scene on whole screen
	glViewport(0,0,w(), h());
    GlWindow::displayMe(cam1, true, showInfo, camDist1);

	if(windowMode == 1) {

		// draw second scene in the top right corner
		glViewport(w()/2, h()/2, w()/2, h()/2);
		// used so I could clear only the top right of the screen
		glEnable(GL_SCISSOR_TEST);
		glScissor(w()/2, h()/2, w()/2, h()/2);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GlWindow::displayMe(cam2, true, false, camDist2);
		glDisable(GL_SCISSOR_TEST);

	}

	glFlush();

}

//Print a string on the screen
void GlWindow::printString(int x, int y, char* str) {
    glRasterPos2i(x, y);
    for (char *s = str; *s; s++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *s);

}

//Handle window resizing
void GlWindow::resize(int X, int Y, int W, int H) {
	Fl_Gl_Window::resize(X,Y,W,H);
	initialize(W,H);
	redraw();
}

//Handle input events
int GlWindow::handle(int Fl_event) {
	switch(Fl_event) {

	//Handle mouse events
	case FL_FOCUS:
	case FL_UNFOCUS:
		return 1;

	case FL_ENTER:
        Fl::focus(this);
        return 1;

	case FL_MOVE:
		cursorX = Fl::event_x();
		cursorY = Fl::event_y();
		return 1;

	case FL_DRAG:
		
		if(addObj > 0) {

			//Adding object
			clickX2 = Fl::event_x();
			clickY2 = Fl::event_y();
		} 
		else {

			//Rotating camera
			mouseX = Fl::event_x();
			mouseY = Fl::event_y();

		}

        return 1;

    case FL_PUSH:

		switch(Fl::event_button()) {
		case FL_LEFT_MOUSE:

			mouseX = Fl::event_x();
			mouseY = Fl::event_y();
			mouseX2 = mouseX;
			mouseY2 = mouseY;

			mouseButton = 1;
			if (mouseX > w()/2 && mouseY < h()/2 && !secWin && windowMode == 1) {
				secWin = true;
			}

			updateFocus = true;
			return 1;
		
		case FL_MIDDLE_MOUSE:
			
			return 1;

		case FL_RIGHT_MOUSE:
			
			
			clickX = Fl::event_x();
			clickY = Fl::event_y();
			clickX2 = clickX;
			clickY2 = clickY;
			mouseButton = 2;
			addObj = 1;
			

			return 1;
		}
   

	case FL_RELEASE:
		secWin = false;
		
		if(addObj == 2 && Fl::event_button() == FL_RIGHT_MOUSE)
			addObj = 3;
		else if(addObj == 2 && Fl::event_button() != FL_RIGHT_MOUSE)
			return 0;

		return 1;

	case FL_MOUSEWHEEL:
		if (cursorX > w()/2 && cursorY < h()/2 && !secWin && windowMode == 1) {
			camDist2target += 0.1*camDist2target*Fl::event_dy();
		}
		else {
			camDist1target += 0.1*camDist1target*Fl::event_dy();
		}
		

		return 1;

    //Handle key events
	case FL_KEYDOWN:
		shiftKey = (Fl::event_shift() > 0) ? 1 : 0;

		switch(Fl::event_key()) {
		case FL_F+1:
			//Decrease simulation speed
			timestep *= 0.8;
			return 1;

		case FL_F+2:
			//Increase simulation speed
			timestep *= 1.25;
			return 1;
		
		case FL_F+3:
            //Toggle info overlay
			showInfo = !showInfo;
			return 1;

		case FL_F+4:
            //Toggle grid
			showGrid = !showGrid;
			return 1;

		case FL_F+5:
            //Toggle force grid
			showForceGrid = !showForceGrid;
			return 1;

		case FL_F+6:
            //Toggle window mode
			windowMode = (windowMode+1)%2;
			return 1;

		case FL_F+7:
			//Toggle trail history
			showTrails = !showTrails;
			return 1;

		case FL_Home:
			//Set the origin as the focus
			theUniverse.selectObject(0,0,0);
			return 1;
		
		case FL_Left:
			forceGridSize = (int)ceil(forceGridSize * 0.8);
			return 1;

		case FL_Right:
			forceGridSize = (int)ceil(forceGridSize * 1.25);
			return 1;

		case FL_Down:
			forceGridResolution *= 0.8;
			return 1;

		case FL_Up:
			forceGridResolution *= 1.25;
			return 1;

        case ' ':
            theUniverse.addObject();
            return 1;

        //Update key variables
		case '-':
		case '_':
			keyMinus = 1;
			return 1;
		case '=':
		case '+':
			keyPlus = 1;
			return 1;
		case '[':
		case '{':
			keyLBracket = 1;
			return 1;
		case ']':
		case '}':
			keyRBracket = 1;
			return 1;
		case 'a':
		case 'A':
			keyA = 1;
			return 1;
		case 'd':
		case 'D':
			keyD = 1;
			return 1;
		case 'w':
		case 'W':
			keyW = 1;
			return 1;
		case 's':
		case 'S':
			keyS = 1;
			return 1;
		case 'f':
		case 'F':
			keyF = 1;
			return 1;
		case 'r':
		case 'R':
			keyR = 1;
			return 1;
        case 'q':
		case 'Q':
			keyQ = 1;
			return 1;
        case 'e':
		case 'E':
			keyE = 1;
			return 1;
        case 'z':
		case 'Z':
			keyZ = 1;
			return 1;
        case 'x':
		case 'X':
			keyX = 1;
			return 1;
        case 'c':
		case 'C':
			keyC = 1;
			return 1;
        case 'v':
		case 'V':
			keyV = 1;
			return 1;
		case 't':
		case 'T':
			keyT = 1;
			return 1;
        case 'g':
		case 'G':
			keyG = 1;
			return 1;
        case 'b':
		case 'B':
			keyB = 1;
			return 1;
		}

	case FL_KEYUP:
		shiftKey = (Fl::event_shift() > 0) ? 1 : 0;

		switch(Fl::event_key()) {
		case '-':
		case '_':
			keyMinus = 0;
			return 1;
		case '=':
		case '+':
			keyPlus = 0;
			return 1;
		case '[':
		case '{':
			keyLBracket = 0;
			return 1;
		case ']':
		case '}':
			keyRBracket = 0;
			return 1;
		case 'a':
		case 'A':
			keyA = 0;
			return 1;
		case 'd':
		case 'D':
			keyD = 0;
			return 1;
		case 'w':
		case 'W':
			keyW = 0;
			return 1;
		case 's':
		case 'S':
			keyS = 0;
			return 1;
		case 'f':
		case 'F':
			keyF = 0;
			return 1;
		case 'r':
		case 'R':
			keyR = 0;
			return 1;
        case 'q':
		case 'Q':
			keyQ = 0;
			return 1;
        case 'e':
		case 'E':
			keyE = 0;
			return 1;
        case 'z':
		case 'Z':
			keyZ = 0;
			return 1;
        case 'x':
		case 'X':
			keyX = 0;
			return 1;
        case 'c':
		case 'C':
			keyC = 0;
			return 1;
        case 'v':
		case 'V':
			keyV = 0;
			return 1;
        case 't':
		case 'T':
			keyT = 0;
			return 1;
        case 'g':
		case 'G':
			keyG = 0;
			return 1;
        case 'b':
		case 'B':
			keyB = 0;
			return 1;
		}
	}

	return Fl_Gl_Window::handle(Fl_event);
}


//Constructor
GlWindow::GlWindow(int X,int Y,int W,int H,const char*L) : Fl_Gl_Window(X,Y,W,H,L) {

    //Initialize sensitivity variables
    moveSpeed = 0.3;
	rotSpeed = 1.0;
	scaleSpeed = 0.05;
	mouseSensitivity = 0.25;
	fovSensitivity = 1.0;
	clipSensitivity = 0.05;

    //Initialize key variables
	keyW = keyS = keyA = keyD = keyR = keyF = 0;
	keyQ = keyE = keyZ = keyX = keyC = keyV = 0;
    keyT = keyG = keyB = 0;
	shiftKey = 0;
	keyLBracket = keyRBracket = 0;
	keyPlus = keyMinus = 0;

    //Initialize mouse position
    mouseX = mouseY = 0.0;
	mouseX2 = mouseY2 = 0.0;
	cursorX = cursorY = 0.0;
	secWin = false;
	clickX = clickY = 0.0;
	clickX2 = clickY2 = 0.0;
	mouseButton = 1;
	addObj = 0;

	windowMode = 0;

	camDist1 = camDist2 = 20.0;
	camDist1target = camDist2target = 20.0;
	updateFocus = false;
	

	timestep = (1.0/30.0)*10;

	forceGridSize = 51;
	forceGridResolution = 0.25;

    //Initialize other variables
	showInfo = true;
	showGrid = true;
	showForceGrid = false;
	showTrails = true;

    //Create initial timer
	Fl::add_timeout(1.0/30.0, animate, this);

	//Create and hide the help window
	helpWindow = new Fl_Window(350,400,"Key Bindings");
    disp = new Fl_Text_Display(0,0,350,400);
	buff = new Fl_Text_Buffer();
    disp->buffer(buff);
	disp->color(FL_BACKGROUND_COLOR);
	buff->text("Move Camera:\n"
			   "--------------------\n"
			   "Left: A\n"
			   "Right: D\n"
			   "Forward: W\n"
			   "Backward: S\n"
			   "Up: R\n"
			   "Down: F\n"
			   "\n"
			   "Rotate Camera:\n"
			   "--------------------\n"
			   "Yaw Left: Z or Drag Mouse Left\n"
			   "Yaw Right: V or Drag Mouse Right\n"
			   "Pitch Up: C or Drag Mouse Up\n"
			   "Pitch Down: X or Drag Mouse Down\n"
			   "Roll Clockwise: E\n"
			   "Roll Counter-clockwise: Q\n"
			   "\n"
			   "Adjust Clipping Planes:\n"
			   "--------------------\n"
			   "Increase FOV: +\n"
			   "Decrease FOV: -\n"
			   "Increase Far Clipping Plane: ]\n"
			   "Decrease Far Clipping Plane: [\n"
			   "Increase Near Clipping Plane: Shift + ]\n"
			   "Decrease Near Clipping Plane: Shift + [\n"
			   "\n"
			   "Other:\n"
			   "--------------------\n"
			   "Toggle Info: F3\n"
			   );

	helpWindow->hide();
}

//Show help window
void GlWindow::showHelp() {
	helpWindow->show();
}

//Close both the main window and the help window
void GlWindow::hide() {
	Fl_Gl_Window::hide();
	helpWindow->hide();
}

void GlWindow::resetCamera() {
	camDist1 = camDist2 = 20.0;
	camDist1target = camDist2target = 20.0;

	cam1.rotX = 45.0;
	cam1.rotY = -45.0;

	cam2.rotX = 45.0;
	cam2.rotY = -45.0;
}