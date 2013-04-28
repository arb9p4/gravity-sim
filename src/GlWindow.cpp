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
		pWindow->cam1.updateRotation(rotX, rotY, rotZ);

        pWindow->theUniverse.addTime(pWindow->timestep);

		// simple check to see if there is a planet, then follows the first planet in the list.
		if (pWindow->theUniverse.objList.size() > 0) {
			pWindow->cam2.camX = pWindow->theUniverse.objList.front().Xpos;
			pWindow->cam2.camY = pWindow->theUniverse.objList.front().Ypos;
			pWindow->cam2.camZ = pWindow->theUniverse.objList.front().Zpos;
			
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
	GLfloat light_position[] = { 1.0, 2.0, 3.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glDisable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	
}

// split out drawing function so that you could draw multiple cameras,
// then specifiy lookAt = true to follow a planet.
void GlWindow::displayMe(Camera camNew, bool lookAt) {
	
    //Initialize projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camNew.fov, double(w())/double(h()), camNew.nearClip, camNew.farClip);
	

    //Initialize modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (lookAt) {
		gluLookAt(
			cam2.camX, cam2.camY, cam2.camZ+5.0, 
			cam2.camX, cam2.camY, cam2.camZ, 
			0, 1, 0);
	} else {
		//Move camera
		glRotatef(camNew.rotZ, 0.0, 0.0, 1.0);
		glRotatef(camNew.rotX, 1.0, 0.0, 0.0);
		glRotatef(camNew.rotY, 0.0, 1.0, 0.0);
		glTranslatef(camNew.camX, camNew.camY, camNew.camZ);
	}

    //Draw grid
    glColor3f(0.3, 0.3, 0.4);
    glBegin(GL_LINES);
    for(GLfloat i = -100.0; i <= 100.0; ++i) {
        glVertex3f(i, 0.0, -100.0);
        glVertex3f(i, 0.0, 100.0);
    }
    for(GLfloat i = -100.0; i <= 100.0; ++i) {
        glVertex3f(-100.0, 0.0, i);
        glVertex3f(100.0, 0.0, i);
    }
    glEnd();

	//Draw objects
	theUniverse.draw();

	//Display info on screen
	if(showInfo && !lookAt) {

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
		sprintf(buffer, "# of Objects: %i", theUniverse.objList.size());
		printString(2, currentLine, buffer);

		currentLine += lineHeight;
		sprintf(buffer, "Simulation Speed: %.3f", timestep);
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
    GlWindow::displayMe(cam1, false);

	// draw second scene in the top right corner
	glViewport(w()/2, h()/2, w()/2, h()/2);
	// used so I could clear only the top right of the screen
	glEnable(GL_SCISSOR_TEST);
	glScissor(w()/2, h()/2, w()/2, h()/2);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GlWindow::displayMe(cam2, true);
	glDisable(GL_SCISSOR_TEST);

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

	case FL_DRAG:
        mouseX = Fl::event_x();
        mouseY = Fl::event_y();
        return 1;

    case FL_PUSH:
        mouseX = Fl::event_x();
        mouseY = Fl::event_y();
        mouseX2 = mouseX;
        mouseY2 = mouseY;
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
    moveSpeed = 0.1;
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

	timestep = 1.0/30.0;

    //Initialize other variables
	showInfo = true;

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