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
bool drawProxyPath;

Universe();

//Add object at random location
void addObject();

//Adds a specific body
void addObject(Body b);

//Add object at specific location with no velocity
void addObject(double x, double y, double z);

//Add object with specific position, velocity, and mass
void addObject(double x, double y, double z,
			   double dx, double dy, double dz,
			   double m);


//Add object at position into orbit around specific body
void addObject(double x, double y, double z, double m, Body* b);

//Remove the current object. Returns 1 if it was a target
int removeObject();

//Adds time to the simulation. Returns the number of targets collected
int addTime(double timestep);

//Reverses all velocities
void reverse();

double computeDistance(Body &a, Body &b);

Body mergeBodies(Body a, Body b);

void draw(bool showTrails, double speedScale);

void clear();

void createStars();

void setProxy(double x, double y, double z, double m);

void setProxyVector(double x, double y, double z);

void clearProxy();

void drawSelectors();

void selectObject(double x, double y, double z);

//Computes the total force at a point
double computeForce(double x, double y, double z);

//textures
GLuint texture[6];

void nextObject();

void previousObject();

//Saves the objList to a file
void save(const char* filename);

//Loads the objList from a file. Returns the number of target objects
int load(const char* filename);

//Returns the first actor in the scene
Body getActor();

};

#endif
