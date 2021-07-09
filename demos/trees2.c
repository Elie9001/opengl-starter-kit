/***
 This parametric tree generator code is mostly from an older project of mine, which had integer coordinates and didn't use OpenGL.
 --

 To compile this:  gcc trees2.c -o trees2 -ffast-math -O -lGL -lglut -lm

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
#include "../flatground.h"

#define MAXDOTS 65536
#define VIEW_HEIGHT 300

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


void make_tree(int x, int y, int z, float dx, float dy, float dz, int red, int green, int blue, int size, int step) {
 if (step > 0) {
  while (size > 0) {
   if (nDots >= MAXDOTS) return;
   double norm = sqrt(dx*dx+dy*dy+dz*dz);
   if (norm==0) dz=norm=1; else norm = 1.0 / norm;
   dx *= norm;
   dy *= norm;
   dz *= norm;
   x += dx*size;
   y += dy*size;
   z += dz*size;
   if (z < 0) return;
   dots[nDots].x = x;
   dots[nDots].y = y;
   dots[nDots].z = z;
   dots[nDots].size = size;
   dots[nDots].red = red;
   dots[nDots].green = green;
   dots[nDots].blue = blue;
   nDots++;
   red+=2;if(red>255)red=255;
   green+=2;if(green>255)green=255;
   blue+=2;if(blue>255)blue=255;
   dx += rand()*(0.2/RAND_MAX)-0.1;
   dy += rand()*(0.2/RAND_MAX)-0.1;
   dz += rand()*(0.2/RAND_MAX)-0.1;
   size-=step;
   if (rand()%16==0) make_tree(x,y,z,dx+rand()*(1.0/RAND_MAX)-0.5,dy+rand()*(1.0/RAND_MAX)-0.5,dz+rand()*(1.0/RAND_MAX)-0.5,red,green,blue,size,step+1);
  }
 }
}


void build_stuff()
{
 int i;
 // add some trees
 for (i=0; i<120; i++)
 {
  int x = rand()%15000 - 7500;
  int y = rand()%15000 - 7500;
  if (x*x+y*y>7500*7500) continue;
  int green = rand()%256;
  int rb = rand()%(green+1);
  int size = rand()%40+60;
  make_tree(x,y,0,0,0,1,rb,green,rb,size,1);
  size *= 0.75; if (size < 1) continue;
  shadows[nShadows].x = x;
  shadows[nShadows].y = y;
  shadows[nShadows].size = size*2;
  nShadows++;
  dots[nDots].x = x+size*0.5;
  dots[nDots].y = y+size*0.288675135;
  dots[nDots].z = size;
  dots[nDots].size = size;
  dots[nDots].red = rb;
  dots[nDots].green = green;
  dots[nDots].blue = rb;
  nDots++;
  dots[nDots].x = x-size*0.5;
  dots[nDots].y = y+size*0.288675135;
  dots[nDots].z = size;
  dots[nDots].size = size;
  dots[nDots].red = rb;
  dots[nDots].green = green;
  dots[nDots].blue = rb;
  nDots++;
  dots[nDots].x = x;
  dots[nDots].y = y-size*0.577350269;
  dots[nDots].z = size;
  dots[nDots].size = size;
  dots[nDots].red = rb;
  dots[nDots].green = green;
  dots[nDots].blue = rb;
  nDots++;
 }
 // add some dirt
 for (i=0; i<5000; i++)
 {
  int x = rand()%15000 - 7500;
  int y = rand()%15000 - 7500;
  if (x*x+y*y>7500*7500) continue;
  int red = rand()%256;
  int blue = rand()%(red+1);
  int size = rand()%16+16;
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
  shadows[nShadows].size = size*1.25;
  nShadows++;
 }
 // add some stars
 for (i=0; i<300; i++)
 {
  double a = rand() * M_PI * 2.0 / RAND_MAX;
  double z = rand() * M_PI * 0.25 / RAND_MAX;
  dots[nDots].x = cos(a)*cos(z) * 30000;
  dots[nDots].y = sin(a)*cos(z) * 30000;
  dots[nDots].z = sin(z) * 30000;
  dots[nDots].red = 255;
  dots[nDots].green = 192 + rand()%64;
  dots[nDots].blue = 128 + rand()%128;
  dots[nDots].size = 100;
  nDots++;
 }
 for (i=0; i<200; i++)
 {
  double a = rand() * M_PI * 2.0 / RAND_MAX;
  double z = rand() * M_PI * 0.2 / RAND_MAX + M_PI*0.25;
  dots[nDots].x = cos(a)*cos(z) * 30000;
  dots[nDots].y = sin(a)*cos(z) * 30000;
  dots[nDots].z = sin(z) * 30000;
  dots[nDots].blue = 255;
  dots[nDots].green = 192 + rand()%64;
  dots[nDots].red = 128 + rand()%128;
  dots[nDots].size = 100;
  nDots++;
 }

 // add some weeds
 for (i=0; i<200; i++)
 {
  int x = rand()%20000 - 10000;
  int y = rand()%20000 - 10000;
  int z = 50;
  if (x*x+y*y>10000*10000) continue;
  double dx = 0;
  double dy = 0;
  double dz = 1;
  int green = rand()%256;
  int rb = rand()%(green+1);
  dots[nDots].x = x+19;
  dots[nDots].y = y+11;
  dots[nDots].z = 37;
  dots[nDots].size = 37;
  dots[nDots].red = rb;
  dots[nDots].green = green;
  dots[nDots].blue = rb;
  nDots++;
  dots[nDots].x = x-19;
  dots[nDots].y = y+11;
  dots[nDots].z = 37;
  dots[nDots].size = 37;
  dots[nDots].red = rb;
  dots[nDots].green = green;
  dots[nDots].blue = rb;
  nDots++;
  dots[nDots].x = x;
  dots[nDots].y = y-22;
  dots[nDots].z = 37;
  dots[nDots].size = 37;
  dots[nDots].red = rb;
  dots[nDots].green = green;
  dots[nDots].blue = rb;
  nDots++;
  shadows[nShadows].x = x;
  shadows[nShadows].y = y;
  shadows[nShadows].size = 100;
  nShadows++;
  int j;
  for (j=50; j>0; j--) {
   dots[nDots].x = x;
   dots[nDots].y = y;
   dots[nDots].z = z;
   dots[nDots].size = j;
   dots[nDots].red = rb;
   dots[nDots].green = green;
   dots[nDots].blue = rb;
   nDots++;
   dx += rand()*(0.5/RAND_MAX)-0.25;
   dy += rand()*(0.5/RAND_MAX)-0.25;
   double correction = sqrt(dx*dx+dy*dy+dz*dz);
   if (correction==0) dz=correction=1; else correction = 1.0 / correction;
   dx *= correction;
   dy *= correction;
   dz *= correction;
   x += dx*j;
   y += dy*j;
   z += dz*j;
  }
 }
}







// display-lists
GLuint dl_dots;
GLuint dl_shadows;
GLuint dl_geom;

void init()
{
 build_stuff();

 flatground_init();
 glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

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
 for (int i=0; i<nShadows; i++) {
  shadow(shadows[i].x    * (1.0f / VIEW_HEIGHT),
         shadows[i].y    * (1.0f / VIEW_HEIGHT),
         shadows[i].size * (1.0f / VIEW_HEIGHT));
 }
 shadow(-16,-16, 2);
 glEndList();

 const int NPOINTS=16;
 dl_geom = glGenLists(1);
 glNewList(dl_geom, GL_COMPILE);
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

 _view_direction=90;
 printf("%d dots\n", nDots);
}


void draw()
{
 update_view(0.07);
 shadow_mode();
 glCallList(dl_shadows);
 points_mode_flat();
 glCallList(dl_dots);
 default_mode();
 glCallList(dl_geom);
}


void done()
{
 glDeleteLists(dl_shadows, 1);
 glDeleteLists(dl_dots, 1);
 glDeleteLists(dl_geom, 1);
 flatground_done();
}