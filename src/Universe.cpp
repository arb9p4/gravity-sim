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

#include <cmath>
#include "Universe.h"

void Universe::addObject() {
    objList.push_back(Body());
    objList.back().randVelocity();
}

void Universe::addObject(double x, double y, double z) {
    objList.push_back(Body(x, y, z));
}

void Universe::addObject(double x, double y, double z,
			   double dx, double dy, double dz,
			   double m) {
	objList.push_back(Body(x,y,z,dx,dy,dz,m));
}

void Universe::addTime(double timestep) {

    updateForces();

    std::list<Body>::iterator it;
    for(it = objList.begin(); it != objList.end(); ++it) {

        //Update position
        (*it).Xpos += (*it).dXpos * timestep;
        (*it).Ypos += (*it).dYpos * timestep;
        (*it).Zpos += (*it).dZpos * timestep;

        //Update orientation
        (*it).Xrot += (*it).dXrot * timestep;
        (*it).Yrot += (*it).dYrot * timestep;
        (*it).Zrot += (*it).dZrot * timestep;
    }
}

double Universe::computeDistance(Body a, Body b) {
    double dX = a.Xpos - b.Xpos;
    double dY = a.Ypos - b.Ypos;
    double dZ = a.Zpos - b.Zpos;
    return sqrt(dX*dX + dY*dY + dZ*dZ);
}

Body Universe::mergeBodies(Body a, Body b) {

    double x = (a.Xpos + b.Xpos) / 2.0;
    double y = (a.Ypos + b.Ypos) / 2.0;
    double z = (a.Zpos + b.Zpos) / 2.0;

    double m = a.mass + b.mass;

    double dx = (a.mass*a.dXpos + b.mass*b.dXpos)/m;
    double dy = (a.mass*a.dYpos + b.mass*b.dYpos)/m;
    double dz = (a.mass*a.dZpos + b.mass*b.dZpos)/m;

    return Body(x,y,z,dx,dy,dz,m);
}

void Universe::updateForces() {

	std::list<Body>::iterator it1, it2;

	//Merge objects that are near one another
	bool noMerges = false;
	while(!noMerges) {
		noMerges = true;
		for(it1 = objList.begin(); it1 != objList.end(); ++it1) {
			for(it2 = objList.begin(); it2 != objList.end(); ++it2) {
				if(it1 != it2) {
					if(computeDistance(*it1,*it2) < 0.5) {
						objList.push_back(mergeBodies(*it1,*it2));
						objList.erase(it1);
						objList.erase(it2);
						noMerges = false;
						break;
					}
				}
			}
			if(!noMerges) break;
		}
	}

    //Compute forces
    for(it1 = objList.begin(); it1 != objList.end(); ++it1) {
        for(it2 = objList.begin(); it2 != objList.end(); ++it2) {
			if(it1 != it2) {
				(*it1).computeForce(*it2);
			}
        }
    }
}

void Universe::draw() {

    std::list<Body>::iterator it;
    for(it = objList.begin(); it != objList.end(); ++it) {
        (*it).draw();
        //objList[i].drawHistory();
    }
}

void Universe::clear() {
    objList.clear();
}
