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
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int textureCount = 6;
int i = 0;

Universe::Universe() {
	drawProxy = false;
	drawProxyPath = true;
	createStars();
	clear();
}

void Universe::clear() {
    objList.clear();

	//Add home object
	addObject(0,0,0,0,0,0,0);
	objList.front().isOrigin = true;
	objList.front().isStatic = true;
	objList.front().collidable = false;
}

void Universe::addObject() {
    objList.push_back(Body());
    objList.back().randVelocity();
	objList.back().texture[0] = texture[i++%textureCount];
}

void Universe::addObject(Body b) {
	objList.push_back(b);
	objList.back().texture[0] = texture[i++%textureCount];
}

void Universe::addObject(double x, double y, double z) {
    objList.push_back(Body(x, y, z));
	objList.back().texture[0] = texture[i++%textureCount];
}

void Universe::addObject(double x, double y, double z,
			   double dx, double dy, double dz,
			   double m) {
	objList.push_back(Body(x,y,z,dx,dy,dz,m));
	objList.back().texture[0] = texture[i++%textureCount];
}

//Add object in auto-computed orbit around a body
void Universe::addObject(double x, double y, double z,
			   double m, Body* b) {

	

	//objList.push_back(Body(x,y,z,dx,dy,dz,m));
}

int Universe::removeObject() {

	int numTargets = 0;

	if(!objList.front().isOrigin) {
		if(objList.front().isTarget) numTargets++;
		objList.pop_front();
	}

	return numTargets;
}

int Universe::addTime(double timestep) {

	int collected = 0;

    std::list<Body>::iterator it1, it2;

	//Merge objects that are near one another
	bool noMerges = false;
	while(!noMerges) {
		noMerges = true;
		for(it1 = objList.begin(); it1 != objList.end(); ++it1) {
			
			//Ignore home object
			if(!(*it1).collidable) continue;

			//Only consider target objects as the second object
			if((*it1).isTarget) continue;
			
			for(it2 = objList.begin(); it2 != objList.end(); ++it2) {
				
				//Ignore home object
				if(!(*it2).collidable) continue;
				
				if(it1 != it2) {
					if(computeDistance(*it1,*it2) < (*it1).radius + (*it2).radius) {
						
						noMerges = false;

						if((*it1).isActor) {
							if((*it2).isTarget) {	//If a target is hit, remove the target
								objList.erase(it2);
								collected++;
							}
							else if((*it2).isActor) {	//If actor hits another actor, remove both
								objList.erase(it1);
								objList.erase(it2);
							}
							else {	//If an object is hit, remove the actor
								objList.erase(it1);
							}
						}
						else if((*it2).isTarget) {
							noMerges = true;
							continue;	//Otherwise, ignore targets
						}
						else {
							//Erase the non-static object
							if((*it1).isStatic) {
								objList.erase(it2);
							}
							else if((*it2).isStatic) {
								objList.erase(it1);
							}
							else {

								//Update variables
								double m = (*it1).mass + (*it2).mass;
								(*it1).Xpos = ((*it1).mass*(*it1).Xpos + (*it2).mass*(*it2).Xpos)/m;
								(*it1).Ypos = ((*it1).mass*(*it1).Ypos + (*it2).mass*(*it2).Ypos)/m;
								(*it1).Zpos = ((*it1).mass*(*it1).Zpos + (*it2).mass*(*it2).Zpos)/m;
								(*it1).dXpos = ((*it1).mass*(*it1).dXpos + (*it2).mass*(*it2).dXpos)/m;
								(*it1).dYpos = ((*it1).mass*(*it1).dYpos + (*it2).mass*(*it2).dYpos)/m;
								(*it1).dZpos = ((*it1).mass*(*it1).dZpos + (*it2).mass*(*it2).dZpos)/m;
								(*it1).mass = m;
								(*it1).updateRadius();

								//Clear the trail
								(*it1).trailIndex = 0;
								(*it1).trailLength = 0;

								//Erase the last object
						
								//(*it2).mass = 0;
								//(*it2).updateRadius();
								//(*it2).isStatic = true;
						
								objList.erase(it2);
							}			
						}
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
		(*it).applyForces(timestep);
    }

	return collected;
}

void Universe::reverse() {
	std::list<Body>::iterator it;
    for(it = objList.begin(); it != objList.end(); ++it) {
        (*it).dXpos *= -1;
		(*it).dYpos *= -1;
		(*it).dZpos *= -1;
    }
}

double Universe::computeDistance(Body &a, Body &b) {
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

//Draws all objects in the universe
void Universe::draw(bool showTrails, double speedScale) {

	//Draw stars
	glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POINTS);
	std::list<Point>::iterator starit;
    for(starit = starList.begin(); starit != starList.end(); ++starit) {
        glVertex3f((*starit).X, (*starit).Y, (*starit).Z);
    }
	glEnd();

	//Draw all objects
    std::list<Body>::iterator it;
    for(it = objList.begin(); it != objList.end(); ++it) {
        (*it).draw(showTrails);
    }

	//Draw proxy if necessary
	if(drawProxy) {

		//Draw future path
		if(drawProxyPath) {

			glDepthMask(GL_FALSE);

			glBegin(GL_LINE_STRIP);
			glColor3f(1.0, 0.7, 0.1);
			glVertex3f(proxy.Xpos, proxy.Ypos, proxy.Zpos);
			Body p = proxy;
			p.dXpos = (proxyVector.X - p.Xpos)*speedScale;
			p.dYpos = (proxyVector.Y - p.Ypos)*speedScale;
			p.dZpos = (proxyVector.Z - p.Zpos)*speedScale;
			std::list<Body>::iterator it;
			for(int i = 0; i < 100; i++) {
				for(it = objList.begin(); it != objList.end(); ++it) {
					p.computeForce((*it), 1);
				}

				p.Xpos += p.dXpos;
				p.Ypos += p.dYpos;
				p.Zpos += p.dZpos;

				std::list<Body>::iterator it1, it2;
				bool collision = false;
				for(it1 = objList.begin(); it1 != objList.end(); ++it1) {
			
					//Ignore static objects
					if(!(*it1).collidable || (*it1).isTarget) continue;

					if(computeDistance(*it1,p) < (*it1).radius + p.radius) {
						collision = true;
						break;
					}
				}
			
				if(collision) break;

				glVertex3f(p.Xpos, p.Ypos, p.Zpos);
			}
			glEnd();
		

			//Draw vector
			glBegin(GL_LINES);
			glColor3f(1.0, 1.0, 1.0);
			glVertex3f(proxy.Xpos, proxy.Ypos, proxy.Zpos);
			glVertex3f(proxyVector.X, proxyVector.Y, proxyVector.Z);
			glColor3f(1.0, 1.0, 0.0);
			glVertex3f(proxyVector.X, proxy.Ypos, proxyVector.Z);
			glVertex3f(proxyVector.X, proxyVector.Y, proxyVector.Z);
			glEnd();
		
			glDepthMask(GL_TRUE);

		}
		
		proxy.draw(showTrails);
	
	}
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
	proxy.texture[0] = texture[i%textureCount];
	proxyVector.X = x;
	proxyVector.Y = y;
	proxyVector.Z = z;
	drawProxy = true;
}

void Universe::setProxyVector(double x, double y, double z) {
	Point p(x,y,z);
	proxyVector = p;
	proxy.dXpos = p.X - proxy.Xpos;
	proxy.dYpos = p.Y - proxy.Ypos;
	proxy.dZpos = p.Z - proxy.Zpos;
}

void Universe::clearProxy() {
	drawProxy = false;
}

void Universe::drawSelectors() {
	//Draw all selector objects
    std::list<Body>::iterator it;
    for(it = objList.begin(); it != objList.end(); ++it) {
		(*it).drawSelector();
    }
}

void Universe::selectObject(double x, double y, double z) {
	
	if(objList.size() > 0) {
	
		double minDist = 10000000000;
		std::list<Body>::iterator closest = objList.begin();
		Point p1(x,y,z);

		std::list<Body>::iterator it;
		for(it = objList.begin(); it != objList.end(); ++it) {
			Point p2((*it).Xpos, (*it).Ypos, (*it).Zpos);
			double d = p1.dist(p2);
			if(d < minDist) {
				minDist = d;
				closest = it;
			}
		}

		if(minDist <= (*closest).radius) {
	
			//Move closest object to the front of the list
			Body temp = *closest;
			objList.erase(closest);
			objList.push_front(temp);

		}
	}
}

double Universe::computeForce(double x, double y, double z) {

	double f = 0;

    std::list<Body>::iterator it;
    for(it = objList.begin(); it != objList.end(); ++it) {
		
		
		double eps = 0.0001;
		double G = 0.1;

		double dX = x - (*it).Xpos;
		double dY = y - (*it).Ypos;
		double dZ = z - (*it).Zpos;

		double dist2 = dX*dX + dY*dY + dZ*dZ;
		double dist = sqrt(dist2);

		f += -G*(*it).mass/(dist2 + eps);
    }

	return f;
}

void Universe::nextObject() {
	Body b = objList.front();
	objList.pop_front();
	objList.push_back(b);
}

void Universe::previousObject() {
	Body b = objList.back();
	objList.pop_back();
	objList.push_front(b);
}

void Universe::save(const char* filename) {
	cout << "Writing to " << filename << endl;

	std::ofstream file(filename);
	if(file.is_open()) {

		std::list<Body>::iterator it = objList.begin();
		while(++it != objList.end()) {

			file << (*it).Xpos << ",";
			file << (*it).Ypos << ",";
			file << (*it).Zpos << ",";
			file << (*it).Xrot << ",";
			file << (*it).Yrot << ",";
			file << (*it).Zrot << ",";
			file << (*it).dXpos << ",";
			file << (*it).dYpos << ",";
			file << (*it).dZpos << ",";
			file << (*it).dXrot << ",";
			file << (*it).dYrot << ",";
			file << (*it).dZrot << ",";
			file << (*it).mass << ",";
			file << (*it).selected << ",";
			file << (*it).isOrigin << ",";
			file << (*it).isStatic << ",";
			file << (*it).collidable << ",";
			file << (*it).isTarget << ",";
			file << (*it).isActor << ",";
			file << (*it).texture[0] << "\n";

		}

		file.close();
	}
}

/**************************************************************************************************
Tokenize a string into a vector
Inputs: str - Input string to be tokenized
		tokens - Vector of string tokens for output
		delimiters - String of delimiter characters
**************************************************************************************************/
void Tokenize(const string &str, vector<string> &tokens, const string &delimiters) {

	//Skip delimiters at beginning
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);

	//Find first "non-delimiter"
	string::size_type pos = str.find_first_of(delimiters, lastPos);

	while(string::npos != pos || string::npos != lastPos) {

		//Found a token, add it to the vector
		tokens.push_back(str.substr(lastPos, pos - lastPos));

		//Skip delimiters
		lastPos = str.find_first_not_of(delimiters, pos);

		//Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

int Universe::load(const char* filename) {
	cout << "Loading from " << filename << endl;

	int numTargets = 0;

	std::ifstream file(filename);
	if(file.is_open()) {

		clear();

		while(!file.eof()) {

			string line;
			vector<string> tokens;
			getline(file, line);
			Tokenize(line, tokens, ",");

			if(tokens.size() == 20) {

				double Xpos = atof(tokens[0].c_str());
				double Ypos = atof(tokens[1].c_str());
				double Zpos = atof(tokens[2].c_str());
				double Xrot = atof(tokens[3].c_str());
				double Yrot = atof(tokens[4].c_str());
				double Zrot = atof(tokens[5].c_str());
				double dXpos = atof(tokens[6].c_str());
				double dYpos = atof(tokens[7].c_str());
				double dZpos = atof(tokens[8].c_str());
				double dXrot = atof(tokens[9].c_str());
				double dYrot = atof(tokens[10].c_str());
				double dZrot = atof(tokens[11].c_str());
				double mass = atof(tokens[12].c_str());
				bool selected = (atoi(tokens[13].c_str()) > 0);
				bool isOrigin = (atoi(tokens[14].c_str()) > 0);
				bool isStatic = (atoi(tokens[15].c_str()) > 0);
				bool collidable = (atoi(tokens[16].c_str()) > 0);
				bool isTarget = (atoi(tokens[17].c_str()) > 0);
				bool isActor = (atoi(tokens[18].c_str()) > 0);
				GLuint tex = atoi(tokens[19].c_str());

				Body b(Xpos,Ypos,Zpos,dXpos,dYpos,dZpos,mass);
				b.Xrot = Xrot;
				b.Yrot = Yrot;
				b.Zrot = Zrot;
				b.dXrot = dXrot;
				b.dYrot = dYrot;
				b.dZrot = dZrot;
				b.selected = selected;
				b.isOrigin = isOrigin;
				b.isStatic = isStatic;
				b.collidable = collidable;
				b.isTarget = isTarget;
				b.isActor = isActor;
				b.texture[0] = tex;

				objList.push_back(b);

				if(isTarget) numTargets++;
			}
		}

		file.close();
	}

	return numTargets;
}

Body Universe::getActor() {

	std::list<Body>::iterator it;
    for(it = objList.begin(); it != objList.end(); ++it) {
		if((*it).isActor)
			return (*it);
	}

	if(objList.size() > 0)
		return objList.front();
	else
		return Body();
}