/*
 *	Body.h
 *
 *	Created on : Apr 13, 2013
 *	Course     : CS7610
 *	Instructor : Kannappan Palaniappan
 *	Author	   : Andrew Buck
 *	Purpose    : Represents a physical body having mass and
 *               interacting with other objects
 *
 *	Copyright University of Missouri-Columbia
 */

#ifndef BODY_H
#define BODY_H


#include <deque>
//#include "Asteroid.h"

#define PI 3.14159265359

//Defines a point in space for trail history
class Point {
public:
    double X, Y, Z;

	Point() {X = Y = Z = 0.0;}
	
	Point(double x, double y, double z) {
		X = x;
		Y = y; 
		Z = z;
	}

	//Returns the distance to another point
	double dist(Point p) {
		return sqrt((X-p.X)*(X-p.X) + (Y-p.Y)*(Y-p.Y) + (Z-p.Z)*(Z-p.Z));
	}
};

class Body {

//Asteroid theAsteroid;

public:

//Current position of the body
double Xpos, Ypos, Zpos;    //Translation
double Xrot, Yrot, Zrot;    //Rotation

//Velocity of the body
double dXpos, dYpos, dZpos; //Translation
double dXrot, dYrot, dZrot; //Rotation

//Mass of the body
double mass;

double radius;

bool selected;

//History trail
std::deque<Point> trail;

//Constructor to place a body at a random location
Body();

//Constructor to place a body at a specific location
Body(double x, double y, double z);

//Constructor to place a body at a specific location
//with a specific mass and velocity
Body(double x, double y, double z,
     double dx, double dy, double dz,
     double m);

//Stops all movement
void stop();

//Gives the body a random velocity
void randVelocity();

//Updates the velocity to reflect the force from another body
double computeForce(Body &b, double timestep);

//Draws the body
void draw();

virtual void drawShape();

//Draws the history trail
void drawHistory();

//Draws a transparent hull around the body used for selection
void drawSelector();

};
#endif