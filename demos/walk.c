/***
 Just messing around with building stuff out of dots.

 To compile this:  gcc walk.c -o walk -ffast-math -O -lGL -lglut -lm

 Author: Elie Goldman Smith
 This code isn't worth copyrighting, so it's in the public domain.
***/
#include "../flatground.h"


// display-lists
GLuint dl_dots;
GLuint dl_shadows;
GLuint dl_geometry;


// shadow data is temporarily stored here
#define MAXSHADOWS 200
typedef struct {
 float x, y, size;
} Shadow;
Shadow shadows[MAXSHADOWS] = {0};
int nShadows = 0;


void init()
{
 flatground_init();
 _view_direction=45;
 glClearColor(0.5f, 0.5f, 0.5f, 1.0f);


 dl_dots = glGenLists(1);
 glNewList(dl_dots, GL_COMPILE);
 glBegin(GL_POINTS);
 for (int i=0; i<200; i++)
 {
  point_size(0.05 + 0.05*!(i%4));
  glColor3ub(!(i%4)*128,  0,  255);
  glVertex3f(cosf(i*M_PI*0.01)*5.f,  sinf(i*M_PI*0.01)*5.f,  0);
 }
 for (int i=0; i<200; i++)
 {
  point_size(0.5 + 0.5*!(i%4));
  glColor3ub(!(i%4)*128,  0,  255);
  glVertex3f(cosf(i*M_PI*0.01)*35.35f,  sinf(i*M_PI*0.01)*35.35f,  35.35f);
 }
 for (int i=0; i<200; i++)
 {
  point_size(0.1 + 0.1*!(i%4));
  glColor3ub(!(i%4)*128,  0,  255);
  glVertex3f(cosf(i*M_PI*0.01)*50.f,  sinf(i*M_PI*0.01)*50.f,  0);
 }
 for (int i=0; i<300; i++)
 {
  float a = rand() * M_PI * 2.0 / RAND_MAX;
  float z = rand() * M_PI * 0.25 / RAND_MAX;
  point_size(0.25);
  glColor3ub(255,  192+rand()%64,  128+rand()%128);
  glVertex3f(cosf(a)*cosf(z)*50.f,  sinf(a)*cosf(z)*50.f,  sinf(z)*50.f);
 }
 for (int i=0; i<=200; i++)
 {
  point_size(0.5 + 0.5*(i>0 && i<200 && !(i%4)));
  glColor3ub(!(i%4)*255, 128, 128);
  glVertex3f(cosf(i*M_PI*0.005)*50.f,  0,  sinf(i*M_PI*0.005)*50.f);
 }
 for (int i=0; i<=200; i++)
 {
  point_size(0.5 + 0.5*(i>0 && i<200 && !(i%4)));
  glColor3ub(255,  128 + !(i%4)*128,  64);
  glVertex3f(0,  cosf(i*M_PI*0.005)*50.f,  sinf(i*M_PI*0.005)*50.f);
 }
 srand(3);
 for (int i=0; i<50; i++)
 {
  float x = RND()*50;
  float y = RND()*50;
  if (x*x+y*y > 2500) continue;
  glColor3ub(32,96,32);
  point_size(0.30f);
  glVertex3f(x-0.20f, y-0.12f, 0.30f);
  glVertex3f(x+0.20f, y-0.12f, 0.30f);
  glVertex3f(x,       y+0.23f, 0.30f);
  point_size(0.27f);
  glVertex3f(x,y,0.60f);
  point_size(0.24f);
  glVertex3f(x,y,0.87f);
  point_size(0.21f);
  glVertex3f(x,y,1.11f);
  point_size(0.18f);
  glVertex3f(x,y,1.32f);
  point_size(0.15f);
  glVertex3f(x,y,1.50f);
  glVertex3f(x,y,1.65f);
  glVertex3f(x,y,1.80f);
  glVertex3f(x,y,1.95f);
  glVertex3f(x,y,2.10f);
  glVertex3f(x,y,2.25f);
  glVertex3f(x,y,2.40f);
  glVertex3f(x,y,2.55f);
  glVertex3f(x,y,2.70f);
  point_size(0.60f);
  glColor3ub(192,255,96);
  glVertex3f(x,y,3.35f);
  shadows[nShadows].x = x;
  shadows[nShadows].y = y;
  shadows[nShadows].size = 0.9;
  nShadows++;
 }
 glEnd();
 glEndList();


 dl_shadows = glGenLists(1);
 glNewList(dl_shadows, GL_COMPILE);
 for (int i=0; i<nShadows; i++) shadow(shadows[i].x, shadows[i].y, shadows[i].size);
 shadow(-16,-16, 2);
 glEndList();


 dl_geometry = glGenLists(1);
 glNewList(dl_geometry, GL_COMPILE);
 const int NPOINTS=16;
 glBegin(GL_TRIANGLE_STRIP);
 for (int i=0; i<NPOINTS+2; i++) {
  glColor3f(0.35f + 0.65f*(i&1),
            0.35f + 0.65f*(i&1),
            0.35f + 0.65f*(i&1));
  glVertex3f(-16.0f+cosf(i*2.0f*M_PI/NPOINTS),
             -16.0f+sinf(i*2.0f*M_PI/NPOINTS),
              2.0f*(i&1));
 }
 glEnd();
 glEndList();
}



void draw()
{
 update_view(DEFAULT_MOVE_INTERVAL);
 shadow_mode();
 glCallList(dl_shadows);
 points_mode_flat();
 glCallList(dl_dots);
 default_mode();
 glCallList(dl_geometry);
}



void done()
{
 glDeleteLists(dl_geometry, 1);
 glDeleteLists(dl_shadows, 1);
 glDeleteLists(dl_dots, 1);
 flatground_done();
}