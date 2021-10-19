/***
 Basic stars demo.

 To compile this:  gcc stars.c -o stars -ffast-math -O -lGL -lglut -lm

 Author: Elie Goldman Smith
 This code is too simple to copyright, so it is in the public domain.
***/
#include "../hypersphere.h"

GLuint dl_dots; // display-list

void init()
{
 hypersphere_init();
 points_mode_soft();

 dl_dots = glGenLists(1);
 glNewList(dl_dots, GL_COMPILE);
 glBegin(GL_POINTS);
 for (int i=0; i<400000; i++)
 {
  // star position: a random 4D unit vector
  float w=RND(), x=RND(), y=RND(), z=RND();
  float t = w*w+x*x+y*y+z*z;
  if (t>1 || t==0) continue;
  t = 1.0 / sqrt(t);
  w *= t; x *= t; y *= t; z *= t;

  // star size: random, but with a power-law distribution
  point_size(0.0016*pow(RND01()+0.0001, -0.33));

  // star color: random but bright
  glColor4f(1.0f+RND(),  1.0f,  1.0f+RND(),  1.0f);

  // create the star
  glVertex4f(x,y,z,w);
 }
 glEnd();
 glEndList();
}

void draw()
{
 update_view(DEFAULT_MOVE_INTERVAL);
 glCallList(dl_dots);
}

void done()
{
 glDeleteLists(dl_dots, 1);
 hypersphere_done();
}
