/*
 *	Body.cpp
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

#include <FL/glu.h>
#include <FL/glut.H>
#include <cstdlib>
#include <cmath>
#include "Body.h"

//Constructor to place a body at a random location
Body::Body() {

    double maxDist = 2.0;

    //Set initial position
    Xpos = ((double)rand()/(double)RAND_MAX - 0.5)*2*maxDist;
    Ypos = ((double)rand()/(double)RAND_MAX - 0.5)*2*maxDist;
    Zpos = ((double)rand()/(double)RAND_MAX - 0.5)*2*maxDist;
    Xrot = 0.0;
    Yrot = 0.0;
    Zrot = 0.0;

    mass = 1.0;

	//Compute radius
    radius = pow(3*mass/(4*PI),0.33333);

    stop();

    //theAsteroid = Asteroid();
}

//Constructor to place a body at a specific location
Body::Body(double x, double y, double z) {

    //Set initial position
    Xpos = x;
    Ypos = y;
    Zpos = z;
    Xrot = 0.0;
    Yrot = 0.0;
    Zrot = 0.0;

    mass = 1.0;

	//Compute radius
    radius = pow(3*mass/(4*PI),0.33333);

    stop();

    //theAsteroid = Asteroid();
}

//Constructor to place a body at a specific location
//with a specific mass and velocity
Body::Body(double x, double y, double z,
     double dx, double dy, double dz,
     double m) {

    //Set initial position
    Xpos = x;
    Ypos = y;
    Zpos = z;
    Xrot = 0.0;
    Yrot = 0.0;
    Zrot = 0.0;

    mass = m;

	//Compute radius
    radius = pow(3*mass/(4*PI),0.33333);

    stop();

    dXpos = dx;
    dYpos = dy;
    dZpos = dz;
}

//Stops all movement
void Body::stop() {
    dXpos = 0.0;
    dYpos = 0.0;
    dZpos = 0.0;
    dXrot = 0.0;
    dYrot = 0.0;
    dZrot = 0.0;
}

//Gives the body random velocities
void Body::randVelocity() {

    double maxVelocity = 0.1;

    dXpos = ((double)rand()/(double)RAND_MAX - 0.5)*2*maxVelocity;
    dYpos = ((double)rand()/(double)RAND_MAX - 0.5)*2*maxVelocity;
    dZpos = ((double)rand()/(double)RAND_MAX - 0.5)*2*maxVelocity;
}

//Updates the velocity to reflect the force from another body
void Body::computeForce(Body b, double timestep) {

    double eps = 0.0001;
	double G = 0.1;

	double dX = Xpos - b.Xpos;
	double dY = Ypos - b.Ypos;
	double dZ = Zpos - b.Zpos;

	double dist2 = dX*dX + dY*dY + dZ*dZ;
	double dist = sqrt(dist2);

	double force = -G*b.mass/(dist2 + eps);

	dXpos += timestep*force*dX/(dist + eps);
	dYpos += timestep*force*dY/(dist + eps);
	dZpos += timestep*force*dZ/(dist + eps);
}

//Draws the body
void Body::draw() {

    //Save current transformation
    glPushMatrix();

    //drawHistory();

	//Move object
    glTranslatef(Xpos, Ypos, Zpos);
    glRotatef(Yrot, 0.0, 1.0, 0.0);
    glRotatef(Xrot, 1.0, 0.0, 0.0);
    glRotatef(Zrot, 0.0, 0.0, 1.0);

    drawShape();

    //Body is represented as a sphere
	//glColor3f(1.0, 1.0, 1.0);
	//glRotatef(90.0, 1.0, 0.0, 0.0);
	//glutWireSphere(0.2, 20, 16);
    //theAsteroid.draw();
    //glutWireTetrahedron();
    //Restore last transformation
    glPopMatrix();
}

void Body::drawShape() {
    glColor3f(1.0, 1.0, 1.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
    glutWireSphere(radius, 20, 16);
}

/*
//Draws the history trail
void Body::drawHistory() {

    //Update the trail history
    Point thisPoint;
    thisPoint.X = Xpos;
    thisPoint.Y = Ypos;
    thisPoint.Z = Zpos;
    trail.push_back(thisPoint);
    if(trail.size() > 300)
        trail.pop_front();

    glBegin(GL_LINE_STRIP);
    //Draw the trail history
    std::deque<Point>::iterator it;
    int count = 0;
    for(it = trail.begin(); it != trail.end(); ++it) {
        glColor4f(1.0, 1.0, 1.0, double(count++)/trail.size());
        glVertex3f((*it).X, (*it).Y, (*it).Z);
    }
    glEnd();

}
*/
