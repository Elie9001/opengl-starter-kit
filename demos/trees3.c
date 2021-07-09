/***
 This parametric tree generator code is mostly from an older project of mine, which had integer coordinates and didn't use OpenGL.
 --

 To compile this:  gcc trees3.c -o trees3 -ffast-math -O -lGL -lglut -lm

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

#define VIEWPORT_SCALE 1.2
#define POINT_SIZE_LIMIT_STRING "0.5"
// #define DONT_ANTIALIAS_OPAQUE_POINTS // if you uncomment this line, you *might* get a better frame rate, depending on your graphics card
#include "../flatground.h"

#define MAXDOTS 262144
#define VIEW_HEIGHT 400
#define DISTRIBUTE_TREES

typedef struct {
 int x,y,z;
 float size;
 unsigned char red,green,blue;
} Dot;
Dot dots[MAXDOTS] = {0};
int nDots = 0;

typedef struct {
 int x, y, size;
} Shadow;
Shadow shadows[MAXDOTS] = {0};
int nShadows = 0;


void make_tree(int x, int y, int z, float dx, float dy, float dz, int red, int green, int blue, int thickness, int length, int brightness)
{
 if (length<1) return;
 if (nDots >= MAXDOTS) return;

 if (length==1 && rand()%16==0) {
  // simulate a leaf on the ground
  dots[nDots].x = x+rand()%(thickness*4+1)-thickness*2;
  dots[nDots].y = y+rand()%(thickness*4+1)-thickness*2;
  dots[nDots].z = thickness;
  dots[nDots].size = thickness;
  dots[nDots].red = red/2+64;
  dots[nDots].green = green/2+rand()%128;
  dots[nDots].blue = blue/2+64;
  shadows[nShadows].x = dots[nDots].x;
  shadows[nShadows].y = dots[nDots].y;
  shadows[nShadows].size = dots[nDots].size;
  nShadows++;
  nDots++;
 }

 // make a section (just a squiggly line of points)
 int i;
 for (i=0; i<length; i++) {
   if (nDots >= MAXDOTS) return;

   // normalize vector [dx,dy,dz]
   float norm = sqrt(dx*dx+dy*dy+dz*dz);
   if (norm==0) {
    dx = 0; dy = 0; dz = 1;
   }
   else {
    norm = 1.0 / norm;
    if (length==1) norm *= 2; // a hack for making leaves float off the branches
    dx *= norm;
    dy *= norm;
    dz *= norm;
   }
   // update positions, generate a new dot
   x += dx*thickness;
   y += dy*thickness;
   z += dz*thickness;
   if (z < 1) return; // can't be below ground level
   dots[nDots].x = x;
   dots[nDots].y = y;
   dots[nDots].z = z;
   dots[nDots].size = thickness;
   dots[nDots].red = red;
   dots[nDots].green = green;
   dots[nDots].blue = blue;
   nDots++;
   // fuck with the colour semi-randomly
   red += rand()%brightness+1-(brightness/2);
   green += rand()%brightness-1;
   blue += rand()%brightness+1-(brightness/2);
   if (red<0) red=0;
   if (green<0) green=0;
   if (blue<0) blue=0;
   if (red>255) red=255;
   if (green>255) green=255;
   if (blue>255) blue=255;
   // random-walk the vector for squiggly effect
   dx += rand()*(0.1/RAND_MAX)-0.05;
   dy += rand()*(0.1/RAND_MAX)-0.05;
   dz += rand()*(0.1/RAND_MAX)-0.05;
 }
 // generate a random vector
 float xx = rand()*(2.0/RAND_MAX)-1.0;
 float yy = rand()*(2.0/RAND_MAX)-1.0;
 float zz = rand()*(2.0/RAND_MAX)-1.0;
 // cross product with current vector
 float ddx = dy*zz - dz*yy;
 float ddy = dz*xx - dx*zz;
 float ddz = dx*yy - dy*xx;
 // normalize
 float norm = sqrt(ddx*ddx + ddy*ddy + ddz*ddz);
 if (norm==0) {
  ddx = 0; ddy = 0; ddz = 0.6;
 }
 else {
  norm = 0.6 / norm;
  ddx *= norm;
  ddy *= norm;
  ddz *= norm;
 }
 // branch into two directions, plus and minus the vector
 make_tree(x,y,z,dx+ddx,dy+ddy,dz+ddz,red,green,blue,thickness*0.95,length*0.9,brightness);
 make_tree(x,y,z,dx-ddx,dy-ddy,dz-ddz,red,green,blue,thickness*0.95,length*0.9,brightness);
}


void build_stuff()
{
 int i;
 // add some dirt
 for (i=0; i<5000; i++)
 {
  int x = rand()%30000 - 15000;
  int y = rand()%30000 - 15000;
  if (x*x+y*y>15000*15000) continue;
  int red = rand()%256;
  int blue = rand()%(red+1);
  int size = rand()%12+16;
  dots[nDots].x = x;
  dots[nDots].y = y;
  dots[nDots].z = size;
  dots[nDots].size = size;
  dots[nDots].red = red;
  dots[nDots].green = (red+blue)*0.5;
  dots[nDots].blue = blue;
  nDots++;
  shadows[nShadows].x = x;
  shadows[nShadows].y = y;
  shadows[nShadows].size = size;
  nShadows++;
 }
 // add some trees
 #ifdef DISTRIBUTE_TREES
 int treepointsx[1000];
 int treepointsy[1000];
 int nTrees=0;
 #endif
 for (i=0; i<30; i++)
 {
  int x = rand()%15000 - 7500;
  int y = rand()%15000 - 7500;
  if (x*x+y*y>7500*7500) continue;
  #ifdef DISTRIBUTE_TREES
  int j; for (j=0; j<nTrees; j++) { if ((treepointsx[j]-x)*(treepointsx[j]-x)+(treepointsy[j]-y)*(treepointsy[j]-y)<1500*1500) break; } if(j<nTrees)continue; treepointsx[nTrees]=x;treepointsy[nTrees]=y; nTrees++;
  #endif
  int size = rand()%40+60;
  make_tree(x,y,0,0,0,1,64,32,32,size,12,4+rand()%7);
  size *= 0.75; if (size < 1) continue;
  if (nDots > MAXDOTS-3) break;
  dots[nDots].x = x+size*0.5;
  dots[nDots].y = y+size*0.288675135;
  dots[nDots].z = size;
  dots[nDots].size = size;
  dots[nDots].red = 64;
  dots[nDots].green = 32;
  dots[nDots].blue = 32;
  nDots++;
  dots[nDots].x = x-size*0.5;
  dots[nDots].y = y+size*0.288675135;
  dots[nDots].z = size;
  dots[nDots].size = size;
  dots[nDots].red = 64;
  dots[nDots].green = 32;
  dots[nDots].blue = 32;
  nDots++;
  dots[nDots].x = x;
  dots[nDots].y = y-size*0.577350269;
  dots[nDots].z = size;
  dots[nDots].size = size;
  dots[nDots].red = 64;
  dots[nDots].green = 32;
  dots[nDots].blue = 32;
  nDots++;
  shadows[nShadows].x = x;
  shadows[nShadows].y = y;
  shadows[nShadows].size = size*2;
  nShadows++;
 }
 for (i=0; i<30; i++)
 {
  int x = rand()%30000 - 15000;
  int y = rand()%30000 - 15000;
  if (x*x+y*y<7500*7500 || x*x+y*y>15000*15000) continue;
  #ifdef DISTRIBUTE_TREES
  int j; for (j=0; j<nTrees; j++) { if ((treepointsx[j]-x)*(treepointsx[j]-x)+(treepointsy[j]-y)*(treepointsy[j]-y)<2000*2000) break; } if(j<nTrees)continue; treepointsx[nTrees]=x;treepointsy[nTrees]=y; nTrees++;
  #endif
  int size = rand()%20+30;
  make_tree(x,y,0,0,0,1,64,32,32,size,9,5+rand()%7);
  size *= 0.75; if (size < 1) continue;
  if (nDots > MAXDOTS-3) break;
  dots[nDots].x = x+size*0.5;
  dots[nDots].y = y+size*0.288675135;
  dots[nDots].z = size;
  dots[nDots].size = size;
  dots[nDots].red = 64;
  dots[nDots].green = 32;
  dots[nDots].blue = 32;
  nDots++;
  dots[nDots].x = x-size*0.5;
  dots[nDots].y = y+size*0.288675135;
  dots[nDots].z = size;
  dots[nDots].size = size;
  dots[nDots].red = 64;
  dots[nDots].green = 32;
  dots[nDots].blue = 32;
  nDots++;
  dots[nDots].x = x;
  dots[nDots].y = y-size*0.577350269;
  dots[nDots].z = size;
  dots[nDots].size = size;
  dots[nDots].red = 64;
  dots[nDots].green = 32;
  dots[nDots].blue = 32;
  nDots++;
  shadows[nShadows].x = x;
  shadows[nShadows].y = y;
  shadows[nShadows].size = size*2;
  nShadows++;
 }
 #ifdef DISTRIBUTE_TREES
 printf("%d trees\n",nTrees);
 #endif
}







// display-lists
GLuint dl_dots;
GLuint dl_shadows;

void init()
{
 build_stuff();

 flatground_init();
 glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

 dl_dots = glGenLists(1);
 glNewList(dl_dots, GL_COMPILE);
 glBegin(GL_POINTS);
 for (int i=0; i<nDots; i++) {
  point_size(dots[i].size * (1.0f / VIEW_HEIGHT));
  glColor3ub(dots[i].red, dots[i].green, dots[i].blue);
  glVertex3f(dots[i].x    * (1.0f / VIEW_HEIGHT),
             dots[i].y    * (1.0f / VIEW_HEIGHT),
             dots[i].z    * (1.0f / VIEW_HEIGHT));
 }
 glEnd();
 glEndList();

 dl_shadows = glGenLists(1);
 glNewList(dl_shadows, GL_COMPILE);
 _shadow_darkness = 0.34f;
 for (int i=0; i<nShadows; i++) {
  shadow(shadows[i].x    * (1.0f / VIEW_HEIGHT),
         shadows[i].y    * (1.0f / VIEW_HEIGHT),
         shadows[i].size * (1.0f / VIEW_HEIGHT));
 }
 shadow(0,0,40);
 glEndList();

 _view_direction=275;
 _view_eyelevel=85;
 printf("%d dots\n", nDots);
}


void draw()
{
 update_view(0.07);
 shadow_mode();
 glCallList(dl_shadows);
 points_mode_flat(); //points_mode_soft();glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
 glCallList(dl_dots);
}


void done()
{
 glDeleteLists(dl_shadows, 1);
 glDeleteLists(dl_dots, 1);
 flatground_done();
}