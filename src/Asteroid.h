#ifndef ASTEROID_H
#define ASTEROID_H

#include <FL/glu.h>
#include <FL/glut.H>
#include <vector>
#include <cstdlib>

class Triangle {
GLfloat va[3], vb[3], vc[3];

public:

Triangle(GLfloat *a, GLfloat *b, GLfloat *c) {
    int i;
    for(i=0; i<3; ++i) va[i] = a[i];
    for(i=0; i<3; ++i) vb[i] = b[i];
    for(i=0; i<3; ++i) vc[i] = c[i];
}

void draw() {
    //glBegin(GL_TRIANGLES);
    glBegin(GL_LINE_LOOP);
    glVertex3fv(va);
    glVertex3fv(vb);
    glVertex3fv(vc);
    glEnd();
}

};

class Asteroid : public Body {

int numSplits;
std::vector<Triangle> tList;

//Draw a triangle
void triangle(GLfloat *va, GLfloat *vb, GLfloat *vc)
{
    glBegin(GL_TRIANGLES);
    glVertex3fv(va);
    glVertex3fv(vb);
    glVertex3fv(vc);
    glEnd();
}



void divide_triangle(GLfloat *a, GLfloat *b, GLfloat *c, int level) {

    if(level == 0) {
        //triangle(a,b,c);
        tList.push_back(Triangle(a,b,c));
    }
    else {

        GLfloat mid[3][3];
        int j;

        double scale = 1.0;
        for(j=0; j<level; j++) scale *= 2;

        for(j=0; j<3; j++) mid[0][j]=(a[j]+b[j])/2 + ((double)rand()/RAND_MAX-0.5)/scale;
        for(j=0; j<3; j++) mid[1][j]=(a[j]+c[j])/2 + ((double)rand()/RAND_MAX-0.5)/scale;
        for(j=0; j<3; j++) mid[2][j]=(b[j]+c[j])/2 + ((double)rand()/RAND_MAX-0.5)/scale;

        divide_triangle(a,mid[0],mid[1], level-1);
        divide_triangle(b,mid[0],mid[2], level-1);
        divide_triangle(c,mid[1],mid[2], level-1);
        divide_triangle(mid[0],mid[1],mid[2], level-1);

    }

}

//Draw a tetrahedron from four points
void tetra(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d, int numSplits)
{
    divide_triangle(a, b, c, numSplits);
    divide_triangle(a, c, d, numSplits);
    divide_triangle(a, d, b, numSplits);
    divide_triangle(b, d, c, numSplits);
}

public:

//Drawing commands to create an asteroid
Asteroid:Body() {

    numSplits = 0;

    /*
    //Generate 4 random points
    double p1 = ((double)rand()/(double)RAND_MAX - 0.5);
    double p2 = ((double)rand()/(double)RAND_MAX - 0.5);
    double p3 = ((double)rand()/(double)RAND_MAX - 0.5);
    double p4 = ((double)rand()/(double)RAND_MAX - 0.5);
    */

    GLfloat v[4][3]={{0.0, 0.0, 1.0}, {0.0, 0.942809, -0.33333},
                     {-0.816497, -0.471405, -0.333333}, {0.816497, -0.471405, -0.333333}};

    //Draw
    tetra(v[0],v[1],v[2],v[3],numSplits);
}

void draw() {
    for(unsigned int i = 0; i < tList.size(); i++)
        tList[i].draw();
}

};

#endif