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
#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <FL/glu.h>
#include <FL/glut.H>
#include <FL/Fl_Timer.H>
#include <FL/Fl_Text_Display.H>
#include <cmath>

#include "Universe.h"
#include "Camera.h"


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

	void displayMe(Camera, bool, bool, double);

public:

	//Control sensitivity parameters
	double moveSpeed, rotSpeed, scaleSpeed;
	double mouseSensitivity, fovSensitivity, clipSensitivity;

	//Key variables
	int keyW, keyS, keyA, keyD, keyR, keyF; //Translation keys
	int keyQ, keyE, keyZ, keyX, keyC, keyV; //Roation keys
	int keyT, keyG, keyB; //Scale keys
	int shiftKey;
	int keyLBracket, keyRBracket;
	int keyPlus, keyMinus;
	int keyLeft, keyRight, keyUp, keyDown;

	//Mouse position
    double mouseX, mouseY;		//Current position
    double mouseX2, mouseY2;	//Last position
	double cursorX, cursorY;	//Cursor position
	bool secWin;
	double clickX, clickY;		//Clicked point
	double clickX2, clickY2;	//Clicked point
	int mouseButton;			//Which mouse button was last clicked
	int addObj;					//0: not adding; 1: clicked; 2: released

	//Camera parameters
	Camera cam1;
	Camera cam2;
	int windowMode;		//0: Single window; 1: Add overlay

	double camDist1;		//Distance of the camera from object (set by mouse wheel)
	double camDist2;		//Distance of the camera from object (set by mouse wheel)
	double camDist1target;
	double camDist2target;
	bool updateFocus;	//True if the focus object should be updated

	double timestep;

	int forceGridSize;
	double forceGridResolution;

    /*
    int moveMode;	//Moving camera or object

    int selectedIndex;		//Currently selected object
	std::vector<SceneObj> objList;	//List of objects
    */

    Universe theUniverse;

	bool showInfo;	//Display diagnostic information on the screen
	bool showGrid;	//Display the grid
	bool showForceGrid;	//Display the force grid
	bool showTrails;	//True if trails should be drawn

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

	void resetCamera();
};

#endif
