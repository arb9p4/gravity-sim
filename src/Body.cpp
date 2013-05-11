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

#define INITIAL_TRAIL_LENGTH 100

//Constructor to place a body at a random location
Body::Body() {

    double maxDist = 10.0;

    //Set initial position
    Xpos = ((double)rand()/(double)RAND_MAX - 0.5)*2*maxDist;
    Ypos = ((double)rand()/(double)RAND_MAX - 0.5)*2*maxDist;
    Zpos = ((double)rand()/(double)RAND_MAX - 0.5)*2*maxDist;
    Xrot = 0.0;
    Yrot = 0.0;
    Zrot = 0.0;

    mass = 1.0;

	//Compute radius
    updateRadius();

	selected = false;
	isStatic = false;

	trail.resize(INITIAL_TRAIL_LENGTH, Point());
	trailAlpha.clear();
	for(int i = 0; i < trail.size(); i++)
		trailAlpha.push_back(1.0 - double(i)/double(trail.size()));
	trailIndex = trail.size()-1;
	trailLength = 0;

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
    updateRadius();

	selected = false;
	isStatic = false;

	trail.resize(INITIAL_TRAIL_LENGTH, Point());
	trailAlpha.clear();
	for(int i = 0; i < trail.size(); i++)
		trailAlpha.push_back(1.0 - double(i)/double(trail.size()));
	trailIndex = trail.size()-1;
	trailLength = 0;

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
    updateRadius();

	selected = false;
	isStatic = false;

	trail.resize(INITIAL_TRAIL_LENGTH, Point());
	trailAlpha.clear();
	for(int i = 0; i < trail.size(); i++)
		trailAlpha.push_back(1.0 - double(i)/double(trail.size()));
	trailIndex = trail.size()-1;
	trailLength = 0;

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

void Body::updateRadius() {
	radius = pow(3*mass/(4*PI),0.33333);
}

//Gives the body random velocities
void Body::randVelocity() {

    double maxVelocity = 1;

    dXpos = ((double)rand()/(double)RAND_MAX - 0.5)*2*maxVelocity;
    dYpos = ((double)rand()/(double)RAND_MAX - 0.5)*2*maxVelocity;
    dZpos = ((double)rand()/(double)RAND_MAX - 0.5)*2*maxVelocity;
}

//Updates the velocity to reflect the force from another body
double Body::computeForce(Body &b, double timestep) {

	if(isStatic)
		return 0;

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

	return force;
}

//Draws the body
void Body::draw(bool showTrails, GLuint texture) {

    //Save current transformation
    glPushMatrix();

	if(showTrails && !isStatic)
		drawHistory();
	else {
		//Clear the trail
		trailIndex = trail.size()-1;
		trailLength = 0;;
	}

	//Move object
    glTranslatef(Xpos, Ypos, Zpos);
    glRotatef(Yrot, 0.0, 1.0, 0.0);
    glRotatef(Xrot, 1.0, 0.0, 0.0);
    glRotatef(Zrot, 0.0, 0.0, 1.0);

    drawShape(texture);

    //Body is represented as a sphere
	//glColor3f(1.0, 1.0, 1.0);
	//glRotatef(90.0, 1.0, 0.0, 0.0);
	//glutWireSphere(0.2, 20, 16);
    //theAsteroid.draw();
    //glutWireTetrahedron();

    //Restore last transformation
    glPopMatrix();
}

//texture stuff
typedef struct 
{
    
int X;
    
int Y;
    
int Z;
    

    
double U;
    
double V;
}VERTICES;

const int space = 10;
const int VertexCount = (90 / space) * (360 / space) * 4;
VERTICES VERTEX[VertexCount];


void CreateSphere (double R, double H, double K, double Z) {
	int n;
	double a;
	double b;
	n = 0;
	for( b = 0; b <= 90 - space; b+=space){
		for( a = 0; a <= 360 - space; a+=space){     
			VERTEX[n].X = R * sin((a) / 180 * PI) * sin((b) / 180 * PI) - H;
			VERTEX[n].Y = R * cos((a) / 180 * PI) * sin((b) / 180 * PI) + K;
			VERTEX[n].Z = R * cos((b) / 180 * PI) - Z;
			VERTEX[n].V = (2 * b) / 360;
			VERTEX[n].U = (a) / 360;
			n++;   
			VERTEX[n].X = R * sin((a) / 180 * PI) * sin((b + space) / 180 * PI) - H;
			VERTEX[n].Y = R * cos((a) / 180 * PI) * sin((b + space) / 180 * PI) + K;
			VERTEX[n].Z = R * cos((b + space) / 180 * PI) - Z;
			VERTEX[n].V = (2 * (b + space)) / 360;
			VERTEX[n].U = (a) / 360;
			n++;
			VERTEX[n].X = R * sin((a + space) / 180 * PI) * sin((b) / 180 * PI) - H;
			VERTEX[n].Y = R * cos((a + space) / 180 * PI) * sin((b) / 180 * PI) + K;
			VERTEX[n].Z = R * cos((b) / 180 * PI) - Z;
			VERTEX[n].V = (2 * b) / 360;
			VERTEX[n].U = (a + space) / 360;
			n++;
            
			VERTEX[n].X = R * sin((a + space) / 180 * PI) * sin((b + space) / 180 * PI) - H;
			VERTEX[n].Y = R * cos((a + space) / 180 * PI) * sin((b + space) / 
			180 * PI) + K;
			VERTEX[n].Z = R * cos((b + space) / 180 * PI) - Z;
			VERTEX[n].V = (2 * (b + space)) / 360;
			VERTEX[n].U = (a + space) / 360;
			n++; 
		}
	}
}

void Body::DisplaySphere (double R, GLuint texture){
    
	int b;
	glScalef (0.0125 * R, 0.0125 * R, 0.0125 * R);
	glRotatef (90, 1, 0, 0);
	glBindTexture (GL_TEXTURE_2D, texture);
	glBegin (GL_TRIANGLE_STRIP);
	for ( b = 0; b <= VertexCount; b++){
		glTexCoord2f (VERTEX[b].U, VERTEX[b].V);
		glVertex3f (VERTEX[b].X, VERTEX[b].Y, -VERTEX[b].Z);
	}
	for ( b = 0; b <= VertexCount; b++){
		glTexCoord2f (VERTEX[b].U, -VERTEX[b].V);
		glVertex3f (VERTEX[b].X, VERTEX[b].Y, VERTEX[b].Z);
	}
    
	glEnd();
}

void Body::drawShape(GLuint texture) {

	CreateSphere(70,0,0,0);

	// change 10 to radius
	DisplaySphere(10, texture);
	

	/*
	glEnable(GL_LIGHTING);
    //glColor3f(1.0, 0.0, 1.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
    //glutSolidSphere(radius, 20, 16);

	GLUquadricObj *qObj = gluNewQuadric();
	gluQuadricNormals(qObj, GLU_SMOOTH);
	gluQuadricTexture(qObj, GL_TRUE);
	glEnable(GL_TEXTURE_2D);

	gluSphere(qObj, radius, 20, 16);



	glDisable(GL_LIGHTING);

	*/
}


//Draws the history trail
void Body::drawHistory() {

    //Update the trail history
    //Point thisPoint(Xpos, Ypos, Zpos);
	//trail[trailIndex] = thisPoint;

	trail[trailIndex].X = Xpos;
	trail[trailIndex].Y = Ypos;
	trail[trailIndex].Z = Zpos;
	if(trailLength < trail.size())
		trailLength++;

	//Draw trail
	glBegin(GL_LINE_STRIP);
	for(int i = 0; i < trailLength; ++i) {
		int index = (trailIndex+i)%trail.size();
		glColor4f(1.0, 1.0, 1.0, trailAlpha[i]);
        glVertex3f(trail[index].X, trail[index].Y, trail[index].Z);
	}
	glEnd();

	//Update index
	trailIndex--;
	if(trailIndex < 0)
		trailIndex = trail.size() - 1;

	/*
	bool addPoint = false;
	
	if(trail.empty()) {
		addPoint = true;
	}
	else if(thisPoint.dist(trail.back()) > 1) {
		addPoint = true;
	}

	if(addPoint) {

		trail.push_back(thisPoint);
		if(trail.size() > 100)
			trail.pop_front();

	}
	

	trail.push_back(thisPoint);
	if(trail.size() > 100)
		trail.pop_front();

    glBegin(GL_LINE_STRIP);
    //Draw the trail history
    std::deque<Point>::iterator it;
    int count = 0;
	glColor3f(1.0, 1.0, 1.0);
    for(it = trail.begin(); it != trail.end(); ++it) {
        glColor4f(1.0, 1.0, 1.0, double(count++)/trail.size());
        glVertex3f((*it).X, (*it).Y, (*it).Z);
    }
    glEnd();
	*/
}


void Body::drawSelector() {
	//Save current transformation
    glPushMatrix();

	//Move object
    glTranslatef(Xpos, Ypos, Zpos);
    glRotatef(Yrot, 0.0, 1.0, 0.0);
    glRotatef(Xrot, 1.0, 0.0, 0.0);
    glRotatef(Zrot, 0.0, 0.0, 1.0);


    //Body is represented as a sphere
	glColor4f(1.0, 1.0, 1.0, 0.5);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glutSolidSphere(radius*0.75, 20, 16);

    //Restore last transformation
    glPopMatrix();
}