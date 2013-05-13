/*
 *	Main.cpp
 *
 *	Created on : Apr 10, 2013
 *	Course     : CS7610
 *	Instructor : Kannappan Palaniappan
 *	Author	   : Andrew Buck
 *	Purpose    : Main program startup for Assignment 4
 *
 *	Copyright University of Missouri-Columbia
 */

#include <FL/glu.h>
#include <FL/glut.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_File_Chooser.H>
#include <cstdlib>
#include <cmath>
#include "GlWindow.h"


//************************************************************************
// Main application class
//************************************************************************
class GravitySimApp {

    Fl_Window *win;		//Application window
    Fl_Menu_Bar *menu;	//Menu bar
    GlWindow *view;		//OpenGL window

	//Quit the program
    static void quitProgram(Fl_Widget *w, void *data) {
        exit(0);
    }

	//Clear all objects in the scene
	static void saveScene(Fl_Widget *w, void *data) {
        GravitySimApp *o = (GravitySimApp*)data;
        
		//Create the file chooser, and show it
        Fl_File_Chooser chooser(".",                        // directory
								 "",                    // filter
								Fl_File_Chooser::CREATE,    // chooser type
								 "Save As...");           // title
        chooser.preview(0);
		chooser.show();

        //Block until user picks something.
        while(chooser.shown())
            { Fl::wait(); }

		//Get filename and create a new object
        if ( chooser.value() != NULL ) {
			o->view->theUniverse.save(chooser.value());
		}	
    }

	//Clear all objects in the scene
	static void loadScene(Fl_Widget *w, void *data) {
        GravitySimApp *o = (GravitySimApp*)data;
        
		//Create the file chooser, and show it
        Fl_File_Chooser chooser(".",                        // directory
								 "",                    // filter
								Fl_File_Chooser::SINGLE,    // chooser type
								 "Load...");           // title
        chooser.preview(0);
		chooser.show();

        //Block until user picks something.
        while(chooser.shown())
            { Fl::wait(); }

		//Get filename and create a new object
        if ( chooser.value() != NULL ) {
			o->view->numTargets =  o->view->theUniverse.load(chooser.value());
		}	
    }

	//Clear all objects in the scene
	static void clearScene(Fl_Widget *w, void *data) {
        GravitySimApp *o = (GravitySimApp*)data;
        o->view->clear();
    }

	/*** Menu callback functions ***/

    static void addObjects(Fl_Widget *w, void *data) {
        GravitySimApp *o = (GravitySimApp*)data;
        o->view->theUniverse.addObject(-1,0,0);
        o->view->theUniverse.addObject(1,0,0);
    }

	static void addOrbit(Fl_Widget *w, void *data) {
        GravitySimApp *o = (GravitySimApp*)data;
        o->view->theUniverse.addObject(0,0,0,0,0,0,10);
        o->view->theUniverse.addObject(10,0,0,0,0,0.25,1);
    }

    static void addDisk(Fl_Widget *w, void *data) {
        GravitySimApp *o = (GravitySimApp*)data;

        double speed = 0.3;

		o->view->theUniverse.addObject(0,0,0,0,0,0,10);

        for(int i = 0; i < 100; i++) {
            double angle = ((double)rand()/(double)RAND_MAX)*2*PI;
            double radius = ((double)rand()/(double)RAND_MAX)*3+10;
            double x = radius*cos(angle);
            double z = radius*sin(angle);

            double dx = -sin(angle)*speed;
            double dz = cos(angle)*speed;
            o->view->theUniverse.addObject(x,0,z,dx,0,dz,0.1);

        }

    }

	static void addEarthMoon(Fl_Widget *w, void *data) {
        GravitySimApp *o = (GravitySimApp*)data;

		//Earth
        o->view->theUniverse.addObject(0,0,0,0,0,0,10);

		//Moon
		o->view->theUniverse.addObject(10,0,0,0,0,.29,0.123);
    }

	static void helpMenu(Fl_Widget *w, void *data) {
        GravitySimApp *o = (GravitySimApp*)data;
		o->helpMenu_c();
    }

	//Show the help menu
	void helpMenu_c() {
		view->showHelp();
    }

public:

	//Constructor
    GravitySimApp() {
        win = new Fl_Window(1280, 800, "Gravity Simulator");

		menu = new Fl_Menu_Bar(0, 0, win->w(), 25);
		menu->add("File/Load", 0, loadScene, (void*)this);
		menu->add("File/Save", 0, saveScene, (void*)this);
		menu->add("File/Clear", 0, clearScene, (void*)this);
        menu->add("File/Quit", 0, quitProgram, 0);
        menu->add("Add/Two Objects", 0, addObjects, (void*)this);
		menu->add("Add/Orbiting Objects", 0, addOrbit, (void*)this);
		menu->add("Add/Disk", 0, addDisk, (void*)this);
		//menu->add("Add/Earth and Moon", 0, addEarthMoon, (void*)this);
		menu->add("Help/Key Bindings", 0, helpMenu, (void*)this);

        view = new GlWindow(0, 25, win->w(), win->h()-25);
        win->end();
        win->resizable(view);
        win->show();
    }
};


//************************************************************************
// Main program startup
//************************************************************************
int main() {

    GravitySimApp app;

	return(Fl::run());
}
