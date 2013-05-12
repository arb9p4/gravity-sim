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

const int GlWindow::PRIMARY_CAM = 0;
const int GlWindow::CHASE_CAM = 1;

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
		pWindow->cam1.updateRotation(rotX, rotY, rotZ);
		

        pWindow->theUniverse.addTime(pWindow->timestep);

		// simple check to see if there is a planet, then follows the first planet in the list.
		//Tweening
		double tweenRate = 0.5;
		if (pWindow->theUniverse.objList.size() > 0) {
			
			Body body = pWindow->theUniverse.objList.front();

			// only calculate the camera if its turned on
			if (pWindow->showChaseCamera) {
				// camera 2 updates
				pWindow->cam2.setTarget(
					body.Xpos,
					body.Ypos,
					body.Zpos
				);
				
				pWindow->cam2.updateTranslation(
					(pWindow->cam2.camXtarget - pWindow->cam2.camX) * tweenRate,
					(pWindow->cam2.camYtarget - pWindow->cam2.camY) * tweenRate,
					(pWindow->cam2.camZtarget - pWindow->cam2.camZ) * tweenRate
				);
				
				pWindow->cam2.dx = body.dXpos;
				pWindow->cam2.dy = body.dYpos;
				pWindow->cam2.dz = body.dZpos;
				
				pWindow->cam2.camDist += (pWindow->cam2.camDistTarget - pWindow->cam2.camDist) * tweenRate;
			}
			
			// cam1 updates
			pWindow->cam1.setTarget(
				body.Xpos,
				body.Ypos,
				body.Zpos
			);
			// update tweens
			pWindow->cam1.updateTranslation(
				(pWindow->cam1.camXtarget - pWindow->cam1.camX) * tweenRate,
				(pWindow->cam1.camYtarget - pWindow->cam1.camY) * tweenRate,
				(pWindow->cam1.camZtarget - pWindow->cam1.camZ) * tweenRate
			);
			pWindow->cam1.camDist += (pWindow->cam1.camDistTarget - pWindow->cam1.camDist) * tweenRate;
			
		} else {
			pWindow->cam1.setTranslation(0.0, 0.0, 0.0);
			pWindow->cam2.setTranslation(0.0, 0.0, 0.0);
		}

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

	glEnable( GL_TEXTURE_2D );
	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);


	texture[0] = LoadTextureRAW( "earth.raw" );
	theUniverse.texture[0] = texture[0];
	texture[1] = LoadTextureRAW( "asteroid.raw" );
	theUniverse.texture[1] = texture[1];
}

double normalize(double x, double y, double z) {
	return sqrt((x*x) + (y*y) + (z*z));
}

// split out drawing function so that you could draw multiple cameras,
// then specifiy lookAt = true to follow a planet.
void GlWindow::displayMe(Camera camNew) {
	
    //Initialize projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camNew.fov, double(w())/double(h()), camNew.nearClip, camNew.farClip);
	

    //Initialize modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	switch (camNew.cameraType) {
		double normalizedVector; 

		case GlWindow::CHASE_CAM:
			normalizedVector = normalize(camNew.dx, camNew.dy, camNew.dz);
			normalizedVector += 0.000000000000000000000001;
			gluLookAt(
				camNew.camX - (camNew.dx/normalizedVector) * camNew.camDistTarget,
				camNew.camY - (camNew.dy/normalizedVector) * camNew.camDistTarget,
				camNew.camZ - (camNew.dz/normalizedVector) * camNew.camDistTarget,
				camNew.camX, camNew.camY, camNew.camZ,
				0, 1, 0);

			// debug statement
			//cout << camNew.camX - (camNew.dx/normalizedVector) * camNew.camDistTarget << ", " <<
			//	camNew.camY - (camNew.dy/normalizedVector) * camNew.camDistTarget << ", " <<
			//	camNew.camZ - (camNew.dz/normalizedVector) * camNew.camDistTarget << endl;

			break;

		case GlWindow::PRIMARY_CAM:

			gluLookAt(
				camNew.camX, camNew.camY, camNew.camZ+camNew.camDist, 
				camNew.camX, camNew.camY, camNew.camZ, 
				0, 1, 0);
			glTranslatef(camNew.camX, camNew.camY, camNew.camZ);
			glRotatef(camNew.rotX, 1.0, 0.0, 0.0);
			glRotatef(camNew.rotY, 0.0, 1.0, 0.0);
			glTranslatef(-camNew.camX, -camNew.camY, -camNew.camZ);

			break;

	}

    //Draw grid
	if(camNew.showGrid) {
		
		glBegin(GL_LINES);
		glColor3f(0.3, 0.3, 0.4);
		for(GLfloat i = -100.0; i <= 100.0; ++i) {
			glVertex3f(i, gridHeight, -100.0);
			glVertex3f(i, gridHeight, 100.0);
		}
		for(GLfloat i = -100.0; i <= 100.0; ++i) {
			glVertex3f(-100.0, gridHeight, i);
			glVertex3f(100.0, gridHeight, i);
		}
		glEnd();
		
		//Draw axes
		double axisWidth = 0.03;
		double axisLength = 100;

		glBegin(GL_QUADS);
		glColor3f(0.5, 0.5, 0.5);
		glVertex3f(-axisWidth, -axisWidth, -axisWidth);
		glVertex3f(-axisWidth, -axisWidth,  axisWidth);
		glVertex3f(-axisWidth,  axisWidth,  axisWidth);
		glVertex3f(-axisWidth,  axisWidth, -axisWidth);
		glVertex3f(-axisWidth, -axisWidth, -axisWidth);
		glVertex3f( axisWidth, -axisWidth, -axisWidth);
		glVertex3f( axisWidth, -axisWidth,  axisWidth);
		glVertex3f(-axisWidth, -axisWidth,  axisWidth);
		glVertex3f(-axisWidth,  axisWidth, -axisWidth);
		glVertex3f( axisWidth,  axisWidth, -axisWidth);
		glVertex3f( axisWidth, -axisWidth, -axisWidth);
		glVertex3f(-axisWidth, -axisWidth, -axisWidth);
		glEnd();

		glColor3f(0.6, 0.3, 0.3);
		glBegin(GL_QUAD_STRIP);
		glVertex3f(axisWidth , -axisWidth, -axisWidth);
		glVertex3f(axisLength, -axisWidth, -axisWidth);
		glVertex3f(axisWidth , -axisWidth,  axisWidth);
		glVertex3f(axisLength, -axisWidth,  axisWidth);
		glVertex3f(axisWidth ,  axisWidth,  axisWidth);
		glVertex3f(axisLength,  axisWidth,  axisWidth);	
		glVertex3f(axisWidth ,  axisWidth, -axisWidth);
		glVertex3f(axisLength,  axisWidth, -axisWidth);	
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
		glVertex3f(-axisWidth,  axisWidth, axisWidth );
		glVertex3f(-axisWidth,  axisWidth, axisLength);
		glVertex3f( axisWidth,  axisWidth, axisWidth );
		glVertex3f( axisWidth,  axisWidth, axisLength);	
		glVertex3f( axisWidth, -axisWidth, axisWidth );
		glVertex3f( axisWidth, -axisWidth, axisLength);	
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

	if(camNew.showForceGrid) {

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
	theUniverse.draw(camNew.showTrails, speedScale);
	

	if(camNew.showGrid)
		glColor4f(0.3, 0.3, 0.4, 0.4);	//Give selection plane some transparency
	else
		glColor4f(0.3, 0.3, 0.4, 0.0);	//Make selection plane completely invisible

	glBegin(GL_QUADS);
		glVertex3f(-100, gridHeight, -100);
		glVertex3f(-100, gridHeight, 100);
		glVertex3f(100, gridHeight, 100);
		glVertex3f(100, gridHeight, -100);
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

	GLdouble posX, posY, posZ;              // Hold The Final Values
	GLdouble posX2, posY2, posZ2;

	posX = posY = posZ = 0.0;
	posX2 = posY2 = posZ2 = 0.0;

	if(addObj > 0 && camNew.showProxy) {

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
		
		gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
		gluUnProject( winX2, winY2, winZ2, modelview, projection, viewport, &posX2, &posY2, &posZ2);


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

			//cout << winX2 << " " << winY2 << " " << winZ2 << endl;
			//cout << posX2 << " " << posY2 << " " << posZ2 << endl;

			double dx = posX2 - posX;
			double dy = posY2+vectorHeight - posY;
			double dz = posZ2 - posZ;

			theUniverse.setProxyVector(posX+dx, posY+dy, posZ+dz);

			/*
			double u = normalize(dx, dy, dz);
			if(u > 0.01) {
				double l = sqrt(dx*dx + dz*dz);
				theUniverse.setProxyVector(posX+l*dx/u, posY+l*dy/u, posZ+l*dz/u);
			}
			*/
		}
	}

	//Display info on screen
	if(camNew.showInfoOverlay) {

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

		currentLine += lineHeight;
		sprintf(buffer, "PosX PosY PosZ: %.1f %.1f %.1f", posX, posY, posZ);
		printString(2, currentLine, buffer);

		currentLine += lineHeight;
		sprintf(buffer, "PosX2 PosY2 PosZ2: %.1f %.1f %.1f", posX2, posY2, posZ2);
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
	if(cam1.cameraType == GlWindow::PRIMARY_CAM)
		GlWindow::displayMe(cam1);
	else
		GlWindow::displayMe(cam2);

	if(showChaseCamera) {

		// draw second scene in the top right corner
		glViewport(w()/4*3, h()/4*3, w()/4, h()/4);
		// used so I could clear only the top right of the screen
		glEnable(GL_SCISSOR_TEST);
		glScissor(w()/4*3, h()/4*3, w()/4, h()/4);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// draw chase camera
		GlWindow::displayMe(cam2);
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
		//cout << "Focus" << endl;
	case FL_UNFOCUS:
		//cout << "Unfocus" << endl;
		return 1;

	case FL_ENTER:
		//cout << "Enter" << endl;
        Fl::focus(this);
        return 1;

	case FL_MOVE:
	case FL_DRAG:
		//cout << "Move" << endl;
		cursorX2 = cursorX;
		cursorY2 = cursorY;
		cursorX = Fl::event_x();
		cursorY = Fl::event_y();
		//return 1;

		if(Fl::event_button2() && addObj == 0) {
			//Move grid height
			gridHeight += (cursorY2 - cursorY) * 0.1;
		}
		
		if(Fl::event_button1() || Fl::event_button3()) {
			
			//cout << "Drag " << addObj << endl;
			
			if(addObj > 0) {

				//Adding object

				if(Fl::event_button1()) {
					//Add vertical vector component
					vectorHeight += (cursorY2 - cursorY) * 0.05;

				}
				else {
					//Move in plane
					
					clickX2 = Fl::event_x();
					clickY2 = Fl::event_y();
				}

			} 
			else {

				//Rotating camera
				mouseX = Fl::event_x();
				mouseY = Fl::event_y();

			}
		}

        return 1;

    case FL_PUSH:

		switch(Fl::event_button()) {
		case FL_LEFT_MOUSE:
			//cout << "Push Left Mouse" << endl;
			mouseX = Fl::event_x();
			mouseY = Fl::event_y();
			mouseX2 = mouseX;
			mouseY2 = mouseY;
			vectorHeight = 0.0;

			mouseButton = 1;

			updateFocus = true;
			return 1;
		
		case FL_MIDDLE_MOUSE:
			//cout << "Push Middle Mouse" << endl;

			

			return 1;

		case FL_RIGHT_MOUSE:
			//cout << "Push Right Mouse" << endl;
			
			clickX = Fl::event_x();
			clickY = Fl::event_y();
			clickX2 = clickX;
			clickY2 = clickY;

			vectorHeight = 0.0;
			mouseButton = 2;

			
			addObj = 1;
			

			return 1;
		}
   

	case FL_RELEASE:
		//cout << "Release" << Fl::event_button() << endl;
		if(addObj == 2 && Fl::event_button() == FL_RIGHT_MOUSE) {
		//if(addObj == 2) {

			double dx = theUniverse.proxyVector.X - theUniverse.proxy.Xpos;
			double dy = theUniverse.proxyVector.Y - theUniverse.proxy.Ypos;
			double dz = theUniverse.proxyVector.Z - theUniverse.proxy.Zpos;

			double m = theUniverse.proxy.mass;

			Body b(theUniverse.proxy.Xpos, theUniverse.proxy.Ypos, theUniverse.proxy.Zpos,
				                  dx*speedScale, dy*speedScale, dz*speedScale, m);

			if(Fl::event_ctrl()) {
				b.isStatic = true;
				b.stop();
			}

			theUniverse.addObject(b);
			theUniverse.clearProxy();

			mouseX = mouseX2 = cursorX;
			mouseY = mouseY2 = cursorY;

			addObj = 0;
		}

		return 1;

	case FL_MOUSEWHEEL:
		if (cursorX > w()/4*3 && cursorY < h()/4*3 && showChaseCamera) {
			cam2.camDistTarget += 0.1*cam2.camDistTarget*Fl::event_dy();
		}
		else {
			cam1.camDistTarget += 0.1*cam1.camDistTarget*Fl::event_dy();
		}
		

		return 1;

    //Handle key events
	case FL_KEYDOWN:
		shiftKey = (Fl::event_shift() > 0) ? 1 : 0;
		theUniverse.drawProxyPath = (Fl::event_ctrl() > 0) ? false : true;

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
			cam1.showInfoOverlay = !cam1.showInfoOverlay;
			return 1;

		case FL_F+4:
            //Toggle grid
			cam1.showGrid = !cam1.showGrid;
			cam2.showGrid = !cam2.showGrid;
			return 1;

		case FL_F+5:
            //Toggle force grid
			cam1.showForceGrid = !cam1.showForceGrid;
			cam2.showForceGrid = !cam2.showForceGrid;
			return 1;

		case FL_F+6:
            //Toggle window mode
			showChaseCamera = !showChaseCamera;
			return 1;

		case FL_F+7:
			//Toggle trail history
			cam1.showTrails = !cam1.showTrails;
			cam2.showTrails = !cam2.showTrails;
			return 1;

		case FL_Home:
			//Set the origin as the focus
			theUniverse.selectObject(0,0,0);
			return 1;

		case FL_End:
			//Reset the grid height
			gridHeight = 0.0;
			return 1;
		
		case FL_Tab:
			if(cam1.cameraType == GlWindow::CHASE_CAM) {
				cam1.cameraType = GlWindow::PRIMARY_CAM;
			}
			else {
				cam1.cameraType = GlWindow::CHASE_CAM;
			}

		case FL_Left:
			theUniverse.previousObject();
			return 1;

		case FL_Right:
			theUniverse.nextObject();
			return 1;

		case FL_Page_Down:
			forceGridSize = (int)ceil(forceGridSize * 0.8);
			return 1;

		case FL_Page_Up:
			forceGridSize = (int)ceil(forceGridSize * 1.25);
			return 1;

		case FL_Down:
			forceGridResolution *= 0.8;
			return 1;

		case FL_Up:
			forceGridResolution *= 1.25;
			return 1;

		case FL_Pause:
			//Pause the simulation
			if(timestep > 0) {
				oldTimestep = timestep;
				timestep = 0.0;
			}
			else {
				timestep = oldTimestep;
			}
			return 1;

		case FL_Insert:
			//Reverse the simulation
			theUniverse.reverse();
			return 1;

		case FL_Escape:
			//Cancel adding object
			if(addObj > 0) {
				theUniverse.clearProxy();
				mouseX = mouseX2 = cursorX;
				mouseY = mouseY2 = cursorY;
				addObj = 0;
			}
			return 1;

		case FL_Delete:
			//Delete current object
			theUniverse.removeObject();
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
		break;

	case FL_KEYUP:
		shiftKey = (Fl::event_shift() > 0) ? 1 : 0;
		theUniverse.drawProxyPath = (Fl::event_ctrl() > 0) ? false : true;

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
	cursorX2 = cursorY2 = 0.0;
	clickX = clickY = 0.0;
	clickX2 = clickY2 = 0.0;
	mouseButton = 1;
	addObj = 0;
	vectorHeight = 0.0;
	gridHeight = 0.0;

	cam1.camDist = 20.0;
	cam2.camDist = 5.0;
	cam1.camDistTarget = 20.0;
	cam2.camDistTarget = 5.0;
	updateFocus = false;
	

	timestep = (1.0/30.0)*10;
	oldTimestep = timestep;
	speedScale = 0.1;

	forceGridSize = 51;
	forceGridResolution = 0.25;

    //Initialize other variables
	cam1.showInfoOverlay = true;
	cam1.cameraType = GlWindow::PRIMARY_CAM;
	cam2.cameraType = GlWindow::CHASE_CAM;
	cam2.showProxy = false;
	showChaseCamera = false;

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

	// set textures
	//texture[0] = LoadTextureRAW( "earth.raw" );
	//texture[1] = LoadTextureRAW( "asteroid.raw" );
	//theUniverse.setTexture(texture[0], 0);
	//theUniverse.setTexture(texture[1], 1);
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
	//cam1.camDist = 20.0;
	//cam2.camDist = 5.0;
	cam1.camDistTarget = 20.0;
	cam2.camDistTarget = 5.0;

	cam1.rotX = 45.0;
	cam1.rotY = -45.0;

	cam2.rotX = 45.0;
	cam2.rotY = -45.0;
}

void GlWindow::clear() {
	theUniverse.clear();
	resetCamera();
	gridHeight = 0.0;
	timestep = (1.0/30.0)*10;
}

GLuint GlWindow::LoadTextureRAW( const char * filename ) {

	GLuint texture;
    
	int width, height;
   
	unsigned char * data;
    
	FILE * file;
	file = fopen( filename, "rb" );
	if ( file == NULL ) return 0;
	width = 1024;
	height = 512;
	data = (unsigned char *)malloc( width * height * 3 );
	fread( data, width * height * 3, 1, file );
	fclose( file );

	glGenTextures( 1, &texture ); 
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); 
    
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data );
    
	free( data );
    
	return texture;
    
}