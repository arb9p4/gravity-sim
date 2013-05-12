/*
 *	Universe.cpp
 *
 *	Created on : Apr 13, 2013
 *	Course     : CS7610
 *	Instructor : Kannappan Palaniappan
 *	Author	   : Andrew Buck
 *	Purpose    : Class representing the simulation environment
 *
 *	Copyright University of Missouri-Columbia
 */

#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <list>
#include "Body.h"

class Universe {
public:

std::list<Body> objList;

std::list<Point> starList;

Body proxy;
Point proxyVector;
bool drawProxy;

Universe();

//Add object at random location
void addObject();

//Add object at specific location with no velocity
void addObject(double x, double y, double z);

//Add object with specific position, velocity, and mass
void addObject(double x, double y, double z,
			   double dx, double dy, double dz,
			   double m);


//Add object at position into orbit around specific body
void addObject(double x, double y, double z, double m, Body* b);


void addTime(double timestep);

double computeDistance(Body &a, Body &b);

Body mergeBodies(Body a, Body b);

void draw(bool showTrails, GLuint texture);

void clear();

void createStars();

void setProxy(double x, double y, double z, double m);

void setProxyVector(double x, double y, double z);

void clearProxy();

void drawSelectors();

void selectObject(double x, double y, double z);

//Computes the total force at a point
double computeForce(double x, double y, double z);

void nextObject();

void previousObject();
};

#endif
