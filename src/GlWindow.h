/*
 *	GlWindow.h
 *
 *	Created on : Apr 10, 2013
 *	Course     : CS7610
 *	Instructor : Kannappan Palaniappan
 *	Author	   : Andrew Buck
 *	Purpose    : Class definition for main OpenGL window
 *
 *	Copyright University of Missouri-Columbia
 */

#include <FL/glu.h>
#include <FL/glut.H>
#include <FL/Fl_Timer.H>
#include <FL/Fl_Text_Display.H>
#include <cmath>

#include "Universe.h"

//OpenGL Window Class
class GlWindow : public Fl_Gl_Window {

	//Initialize Window
	void initialize(int W,int H);

	//Draw the scene
	void draw();

	//Print diagnostic information on the screen
	void printString(int x, int y, char* string);

	//Handle window resizing
	void resize(int X, int Y, int W, int H);

	//Handle events
	int handle(int Fl_event);

public:

	//Control sensitivity parameters
	double moveSpeed, rotSpeed, scaleSpeed;
	double mouseSensitivity, fovSensitivity, clipSensitivity;

	//Camera parameters
	double camX, camY, camZ;
	double rotX, rotY, rotZ;
	double fov, nearClip, farClip;

	//Key variables
	int keyW, keyS, keyA, keyD, keyR, keyF; //Translation keys
	int keyQ, keyE, keyZ, keyX, keyC, keyV; //Roation keys
	int keyT, keyG, keyB; //Scale keys
	int shiftKey;
	int keyLBracket, keyRBracket;
	int keyPlus, keyMinus;

	//Mouse position
    double mouseX, mouseY;		//Current position
    double mouseX2, mouseY2;	//Last position

	double timestep;

    /*
    int moveMode;	//Moving camera or object

    int selectedIndex;		//Currently selected object
	std::vector<SceneObj> objList;	//List of objects
    */

    Universe theUniverse;

	bool showInfo;	//Display diagnostic information on the screen

	//Help window objects
	Fl_Window *helpWindow;
    Fl_Text_Display *disp;
	Fl_Text_Buffer *buff;

	//Constructor
	GlWindow(int X,int Y,int W,int H,const char*L=0);

	//Show help dialog
	void showHelp();

	//Called on close to ensure that the help dialog is also closed
	void hide();
};
