/***
 I originally made this demo to test cross() and line()

 To compile this:  gcc box.c -o box -ffast-math -O -lGL -lglut -lm

 Author: Elie Goldman Smith
 This code is too simple to copyright, so it's in the public domain.
***/
#include "../flatground.h"
#include "../line.h"


// display-lists
GLuint dl_geometry;
GLuint dl_shadows;


void init() {
 flatground_init();
 _view_direction = 30;
 glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

 dl_shadows = glGenLists(1);
 glNewList(dl_shadows, GL_COMPILE);
 shadow(0,0,20.0f);
 shadow(0,0, 2.0f);
 shadow( 10, 10, 1);
 shadow(-10, 10, 1);
 shadow(-10,-10, 1);
 shadow( 10,-10, 1);
 glEndList();

 dl_geometry = glGenLists(1);
 glNewList(dl_geometry, GL_COMPILE);

 // box in the centre
 glBegin(GL_QUADS);
 glColor3f(1.f, 1.f, 1.f);
 glVertex3f(-1.f, -1.f,  0.f);
 glVertex3f( 1.f, -1.f,  0.f);
 glVertex3f( 1.f, -1.f,  2.f);
 glVertex3f(-1.f, -1.f,  2.f);
 glVertex3f( 1.f,  1.f,  0.f);
 glVertex3f(-1.f,  1.f,  0.f);
 glVertex3f(-1.f,  1.f,  2.f);
 glVertex3f( 1.f,  1.f,  2.f);
 glColor3f(0.f, 0.f, 0.f);
 glVertex3f( 1.f, -1.f,  0.f);
 glVertex3f( 1.f,  1.f,  0.f);
 glVertex3f( 1.f,  1.f,  2.f);
 glVertex3f( 1.f, -1.f,  2.f);
 glVertex3f(-1.f, -1.f,  2.f);
 glVertex3f(-1.f,  1.f,  2.f);
 glVertex3f(-1.f,  1.f,  0.f);
 glVertex3f(-1.f, -1.f,  0.f);
 glColor3f(0.8f, 0.8f, 0.8f);
 glVertex3f(-1.f, -1.f,  2.f);
 glVertex3f( 1.f, -1.f,  2.f);
 glVertex3f( 1.f,  1.f,  2.f);
 glVertex3f(-1.f,  1.f,  2.f);
 glEnd();

 // blue frame
 vec3 red  = new_vec3(1,0,0);
 vec3 green= new_vec3(0,1,0);
 vec3 blue = new_vec3(0,0,1);
 line(new_vec3( 10, 10, 0),
      new_vec3( 10, 10, 8.5f),
      blue, 0.5f);
 line(new_vec3(-10, 10, 0),
      new_vec3(-10, 10, 8.5f),
      blue, 0.5f);
 line(new_vec3(-10,-10, 0),
      new_vec3(-10,-10, 8.5f),
      blue, 0.5f);
 line(new_vec3( 10,-10, 0),
      new_vec3( 10,-10, 8.5f),
      blue, 0.5f);
 line_to(new_vec3( 10, 10, 8));
 line_to(new_vec3(-10, 10, 8));
 line_to(new_vec3(-10,-10, 8));
 line_to(new_vec3( 10,-10, 8));

 // axis objects
 for (int i=0; i<200; i++) {
  vec3 a = random_unit_vec3();
  vec3 b = random_unit_vec3();
  vec3 c = cross(a,b); c = normalize_vec3(c);
       a = cross(b,c); a = normalize_vec3(a);
  vec3 p; p.x = RND()*20;
          p.y = RND()*20;
          p.z = RND()*9 + 10;
  line(p, add_vec3(p,a), red,  0.05f);
  line(p, add_vec3(p,b), green,0.05f);
  line(p, add_vec3(p,c), blue, 0.05f);
 }

 glEndList();
 glEnable(GL_CULL_FACE);
}


void draw()
{
 update_view(0.07);
 shadow_mode();
 glCallList(dl_shadows);
 default_mode();
 glCallList(dl_geometry);
}


void done()
{
 glDeleteLists(dl_shadows, 1);
 glDeleteLists(dl_geometry, 1);
 flatground_done();
}