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

#include <FL/glu.h>
#include <FL/glut.H>
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

    std::list<Body>::iterator it1, it2;

	//Merge objects that are near one another
	bool noMerges = false;
	while(!noMerges) {
		noMerges = true;
		for(it1 = objList.begin(); it1 != objList.end(); ++it1) {
			for(it2 = objList.begin(); it2 != objList.end(); ++it2) {
				if(it1 != it2) {
					if(computeDistance(*it1,*it2) < (*it1).radius + (*it2).radius) {
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
				(*it1).computeForce(*it2, timestep);
			}
        }
    }

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

	double m = a.mass + b.mass;

	double x = (a.mass*a.Xpos + b.mass*b.Xpos)/m;
    double y = (a.mass*a.Ypos + b.mass*b.Ypos)/m;
    double z = (a.mass*a.Zpos + b.mass*b.Zpos)/m;

    double dx = (a.mass*a.dXpos + b.mass*b.dXpos)/m;
    double dy = (a.mass*a.dYpos + b.mass*b.dYpos)/m;
    double dz = (a.mass*a.dZpos + b.mass*b.dZpos)/m;

    return Body(x,y,z,dx,dy,dz,m);
}

void Universe::draw() {

	glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POINTS);
	std::list<Point>::iterator starit;
    for(starit = starList.begin(); starit != starList.end(); ++starit) {
        glVertex3f((*starit).X, (*starit).Y, (*starit).Z);
    }
	glEnd();

    std::list<Body>::iterator it;
    for(it = objList.begin(); it != objList.end(); ++it) {
        (*it).draw();
    }

	if(drawProxy) {
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);
		glVertex3f(proxy.Xpos, proxy.Ypos, proxy.Zpos);
		glVertex3f(proxyVector.X, proxyVector.Y, proxyVector.Z);
		glEnd();
		proxy.draw();
	}
}

void Universe::clear() {
    objList.clear();
}

void Universe::createStars() {
	
	double minDist = 1000;
	double maxDist = 100000;

	starList.clear();
	while(starList.size() < 10000) {
	
		Point s;
		
		s.X = ((double)rand()/(double)RAND_MAX - 0.5)*2*maxDist;
		s.Y = ((double)rand()/(double)RAND_MAX - 0.5)*2*maxDist;
		s.Z = ((double)rand()/(double)RAND_MAX - 0.5)*2*maxDist;
		
		if(s.X*s.X + s.Y*s.Y + s.Z*s.Z > minDist)
			starList.push_back(s);
    }
}

void Universe::setProxy(double x, double y, double z, double m) {
	proxy = Body(x,y,z,0,0,0,m);
	proxyVector.X = x;
	proxyVector.Y = y;
	proxyVector.Z = z;
	drawProxy = true;
}

void Universe::setProxyVector(double x, double y, double z) {
	Point p;
	p.X = x;
	p.Y = y;
	p.Z = z;
	proxyVector = p;
}

void Universe::clearProxy() {
	drawProxy = false;
}