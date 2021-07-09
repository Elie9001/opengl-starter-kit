/***
 Mathematically modelling an igloo.
 It ain't perfect but whatever.

 To compile this:  gcc igloo.c -o igloo -ffast-math -O -lGL -lglut -lm

 Copyright 2021, Elie Goldman Smith

 This program is FREE SOFTWARE: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
***/

#define USE_MULTISAMPLING
#include "../flatground.h"
#include "../line.h"


#define IGLOO_SCALE    10.000f // igloo radius. everything gets scaled by this
#define BLOCK_HEIGHT    0.084f // as a fraction of igloo radius
#define BLOCK_SHRINK    0.900f // as a fraction of the block size.
#define ENTRANCE_WIDTH  0.400f // as a fraction of igloo diameter
#define ENTRANCE_END    1.500f // coordinate as a multiple of igloo radius
#define BLOCK_GAIN_MAX  8 // must be a power of two. The number of blocks in the bottom rows of the dome, will be an integer multiple of this.



// draw a block
// pos = position of center
// face = dimension & direction of outward face
// side = dimension & direction of the side of the block (which will be mostly covered up by adjacent blocks)
// up   = dimension & direction of the top of the block
// essentially, this generates a parallellopiped.
void block(vec3 pos, vec3 face, vec3 side, vec3 up)
{
 float indicator = dot_vec3(cross(up,side),face);
 if (indicator == 0) return; // degenerate
 if (indicator  < 0) side = mul_vec3(side, -1); // flip to maintain winding
 vec3 p;

 glBegin(GL_QUADS);

 // top
 glColor3f(0.9f, 0.9f, 0.9f);
 p = add_vec3(add_vec3(add_vec3(pos,up),side),face); glVertex_vec3(p);
 p = add_vec3(sub_vec3(add_vec3(pos,up),side),face); glVertex_vec3(p);
 p = sub_vec3(sub_vec3(add_vec3(pos,up),side),face); glVertex_vec3(p);
 p = sub_vec3(add_vec3(add_vec3(pos,up),side),face); glVertex_vec3(p);

 // bottom
 glColor3f(0.4f, 0.4f, 0.4f);
 p = sub_vec3(add_vec3(sub_vec3(pos,up),side),face); glVertex_vec3(p);
 p = sub_vec3(sub_vec3(sub_vec3(pos,up),side),face); glVertex_vec3(p);
 glColor3f(0.3f, 0.3f, 0.3f);
 p = add_vec3(sub_vec3(sub_vec3(pos,up),side),face); glVertex_vec3(p);
 p = add_vec3(add_vec3(sub_vec3(pos,up),side),face); glVertex_vec3(p);

 // lateral
 glColor3f(0.0f, 0.0f, 0.0f);
 p = add_vec3(add_vec3(sub_vec3(pos,up),side),face); glVertex_vec3(p);
 p = add_vec3(add_vec3(add_vec3(pos,up),side),face); glVertex_vec3(p);
 p = sub_vec3(add_vec3(add_vec3(pos,up),side),face); glVertex_vec3(p);
 p = sub_vec3(add_vec3(sub_vec3(pos,up),side),face); glVertex_vec3(p);

 // lateral
 glColor3f(0.0f, 0.0f, 0.0f);
 p = sub_vec3(sub_vec3(sub_vec3(pos,up),side),face); glVertex_vec3(p);
 p = sub_vec3(sub_vec3(add_vec3(pos,up),side),face); glVertex_vec3(p);
 p = add_vec3(sub_vec3(add_vec3(pos,up),side),face); glVertex_vec3(p);
 p = add_vec3(sub_vec3(sub_vec3(pos,up),side),face); glVertex_vec3(p);

 // outer face
 glColor3f(0.8f, 0.8f, 0.8f);
 p = add_vec3(sub_vec3(sub_vec3(pos,up),side),face); glVertex_vec3(p);
 glColor3f(1.0f, 1.0f, 1.0f);
 p = add_vec3(sub_vec3(add_vec3(pos,up),side),face); glVertex_vec3(p);
 p = add_vec3(add_vec3(add_vec3(pos,up),side),face); glVertex_vec3(p);
 glColor3f(0.8f, 0.8f, 0.8f);
 p = add_vec3(add_vec3(sub_vec3(pos,up),side),face); glVertex_vec3(p);

 // inner face
 glColor3f(0.7f, 0.7f, 0.7f);
 p = sub_vec3(add_vec3(sub_vec3(pos,up),side),face); glVertex_vec3(p);
 glColor3f(0.5f, 0.5f, 0.5f);
 p = sub_vec3(add_vec3(add_vec3(pos,up),side),face); glVertex_vec3(p);
 p = sub_vec3(sub_vec3(add_vec3(pos,up),side),face); glVertex_vec3(p);
 glColor3f(0.7f, 0.7f, 0.7f);
 p = sub_vec3(sub_vec3(sub_vec3(pos,up),side),face); glVertex_vec3(p);

 glEnd();
}


#define IGLOO_CATENARY_FACTOR 0.5f
float catenary(float x) {
 return IGLOO_CATENARY_FACTOR * coshf(x / IGLOO_CATENARY_FACTOR);
}
float inv_func_catenary(float y) {
 float b = y / IGLOO_CATENARY_FACTOR;
 if (b < 1.0f) return 0.0f;
 else return IGLOO_CATENARY_FACTOR * acoshf(b);
}

// for our dome model (ground radius = 1):
float dome_r(float h) { // return radius given height h
 float y = catenary(1.0f) - h;
 return inv_func_catenary(y);
}
float dome_h(float r) { // return height given radius r
 return catenary(1.0f) - catenary(r);
}



// display-lists
GLuint dl_geometry;
GLuint dl_shadows;
GLuint dl_dots;

void init() {
 flatground_init();
 glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

 dl_shadows = glGenLists(1);
 glNewList(dl_shadows, GL_COMPILE);
 _shadow_darkness = 0.22f;
 shadow(0,0,IGLOO_SCALE*2);
 glEndList();

 dl_geometry = glGenLists(1);
 glNewList(dl_geometry, GL_COMPILE);

 float entrance_y = sqrtf(1.0f - ENTRANCE_WIDTH*ENTRANCE_WIDTH);
 int entrance_n = (ENTRANCE_END - entrance_y) / (BLOCK_HEIGHT*2.0f);
 float arc = 2.0f*(M_PI-asin(ENTRANCE_WIDTH));
 int arc_n = arc / (BLOCK_HEIGHT*2.0f*BLOCK_GAIN_MAX) + 0.5f;
 arc_n *= BLOCK_GAIN_MAX;
 float r=1;
 int block_gain=1;
 float last_entrance_x = ENTRANCE_WIDTH;
 int odd=0;
 for (float z=BLOCK_HEIGHT*0.5f;  r>0;  z+=BLOCK_HEIGHT, odd = !odd)
 {
  r = dome_r(z);
  if (r*block_gain < M_SQRT1_2 && block_gain < BLOCK_GAIN_MAX) block_gain *= 2;

  // row of entrance way
  float x = dome_r(z/ENTRANCE_WIDTH) * ENTRANCE_WIDTH;
  float y = sqrtf(r*r - x*x);
  float dy = (ENTRANCE_END - y) / entrance_n;
  if (x > 0) {
   last_entrance_x = x;
   int i=0;
   if (odd) {
    y += dy*0.5f;
    i=1;
   }
   for (; i<entrance_n; i++, y+=dy) {
    vec3 p = new_vec3(x,                 y,                    z*0.99f); // the 0.99 is just to prevent z-fighting with the dome blocks
    vec3 f = new_vec3(BLOCK_HEIGHT*0.5f, 0,                    0);
    vec3 s = new_vec3(0,                 dy*0.5f*BLOCK_SHRINK, 0);
    vec3 u = new_vec3(0,                 0,                    BLOCK_HEIGHT*0.5f);
    block(mul_vec3(p, IGLOO_SCALE),
          mul_vec3(f, IGLOO_SCALE),
          mul_vec3(s, IGLOO_SCALE),
          mul_vec3(u, IGLOO_SCALE));
    p = new_vec3(-p.x, p.y, p.z);
    f = new_vec3(-f.x, f.y, f.z);
    block(mul_vec3(p, IGLOO_SCALE),
          mul_vec3(f, IGLOO_SCALE),
          mul_vec3(s, IGLOO_SCALE),
          mul_vec3(u, IGLOO_SCALE));
   }
   // stray blocks at the edge of entrance
   if (odd) {
    vec3 p = new_vec3(x,                 y-dy*0.25f,            z*0.99f);  // the 0.99 is just to prevent z-fighting with the dome blocks
    vec3 f = new_vec3(BLOCK_HEIGHT*0.5f, 0,                     0);
    vec3 s = new_vec3(0,                 dy*0.25f*BLOCK_SHRINK, 0);
    vec3 u = new_vec3(0,                 0,                     BLOCK_HEIGHT*0.5f);
    block(mul_vec3(p, IGLOO_SCALE),
          mul_vec3(f, IGLOO_SCALE),
          mul_vec3(s, IGLOO_SCALE),
          mul_vec3(u, IGLOO_SCALE));
    p = new_vec3(-p.x, p.y, p.z);
    f = new_vec3(-f.x, f.y, f.z);
    block(mul_vec3(p, IGLOO_SCALE),
          mul_vec3(f, IGLOO_SCALE),
          mul_vec3(s, IGLOO_SCALE),
          mul_vec3(u, IGLOO_SCALE));
   }
  }/*
  else if (last_entrance_x) { // keystone of entrance way
   vec3 p = new_vec3(last_entrance_x*0.5f, (r+ENTRANCE_END-dy*0.5f)*0.5f, z);
   vec3 f = new_vec3(last_entrance_x*0.5f, 0,                             0);
   vec3 s = new_vec3(0,                    (r-ENTRANCE_END+dy*0.5f)*0.4f, 0);
   vec3 u = new_vec3(0,                    0,                             BLOCK_HEIGHT*0.5f);
   block(mul_vec3(p, IGLOO_SCALE),
         mul_vec3(f, IGLOO_SCALE),
         mul_vec3(s, IGLOO_SCALE),
         mul_vec3(u, IGLOO_SCALE));
   p = new_vec3(-p.x, p.y, p.z);
   f = new_vec3(-f.x, f.y, f.z);
   block(mul_vec3(p, IGLOO_SCALE),
         mul_vec3(f, IGLOO_SCALE),
         mul_vec3(s, IGLOO_SCALE),
         mul_vec3(u, IGLOO_SCALE));
   last_entrance_x = 0;
  }*/

  // row of dome
  float a = asin(x/r);
  arc = 2.0f*(M_PI-a);
  float da = block_gain * arc / arc_n;
  int i=0;
  if (odd) {
   a += da*0.5f;
   if (x > 0) i=1;
  }
  for (; i<=arc_n; i+=block_gain, a+=da) {
   float co = cosf(a);
   float si = sinf(a);
   vec3 p = new_vec3(-si*r,                                     co*r,                                      z);
   vec3 f = new_vec3(-si*BLOCK_HEIGHT*0.5f,                     co*BLOCK_HEIGHT*0.5f,                      0);
   vec3 s = new_vec3(co*r*block_gain*BLOCK_HEIGHT*BLOCK_SHRINK, si*r*block_gain*BLOCK_HEIGHT*BLOCK_SHRINK, 0);
   vec3 u = new_vec3(0,                                         0,                                         BLOCK_HEIGHT*0.5f);
   block(mul_vec3(p, IGLOO_SCALE),
         mul_vec3(f, IGLOO_SCALE), // FIXME: somehow make the blocks at the top thicker, so they don't look like they're floating
         mul_vec3(s, IGLOO_SCALE),
         mul_vec3(u, IGLOO_SCALE));
  }
 }

 glEndList();

 dl_dots = glGenLists(1);
 glNewList(dl_dots, GL_COMPILE);
 glBegin(GL_POINTS);
 glColor3f(0.2f,0.5f,0.2f);
 point_size(BLOCK_HEIGHT*0.4f*IGLOO_SCALE);
 for (float x=BLOCK_HEIGHT*0.5f; x<1.0f; x+=BLOCK_HEIGHT) {
  glVertex3f( x*IGLOO_SCALE*ENTRANCE_WIDTH, (ENTRANCE_END-BLOCK_HEIGHT*1.25f)*IGLOO_SCALE, dome_h(x)*IGLOO_SCALE*ENTRANCE_WIDTH);
  glVertex3f(-x*IGLOO_SCALE*ENTRANCE_WIDTH, (ENTRANCE_END-BLOCK_HEIGHT*1.25f)*IGLOO_SCALE, dome_h(x)*IGLOO_SCALE*ENTRANCE_WIDTH);
 }
 glEnd();
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
 points_mode_ball();
 glCallList(dl_dots);
}


void done()
{
 glDeleteLists(dl_geometry, 1);
 glDeleteLists(dl_shadows, 1);
 glDeleteLists(dl_dots, 1);
 flatground_done();
}
