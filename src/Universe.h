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

Universe() {
	drawProxy = false;
	createStars();

	//Add default object
	addObject(0,0,0);
}

void addObject();

void addObject(double x, double y, double z);

void addObject(double x, double y, double z,
			   double dx, double dy, double dz,
			   double m);

void addTime(double timestep);

double computeDistance(Body a, Body b);

Body mergeBodies(Body a, Body b);

void draw();

void clear();

void createStars();

void setProxy(double x, double y, double z, double m);

void setProxyVector(double x, double y, double z);

void clearProxy();

void drawSelectors();

void selectObject(double x, double y, double z);

};

#endif
