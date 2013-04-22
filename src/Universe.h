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

#include <list>
//#include <vector>
#include "Body.h"

class Universe {
public:

std::list<Body> objList;

Universe() {}

void addObject();

void addObject(double x, double y, double z);

void addObject(double x, double y, double z,
			   double dx, double dy, double dz,
			   double m);

void addTime(double timestep);

double computeDistance(Body a, Body b);

Body mergeBodies(Body a, Body b);

void updateForces();

void draw();

void clear();

};
