/***
 Just some random Christmas stuff. Code is mostly from an older project of mine, which had integer coordinates and didn't use OpenGL.
 --

 To compile this:  gcc trees4.c -o trees4 -ffast-math -O -lGL -lglut -lm

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
#define POINT_SIZE_LIMIT_STRING "0.25"
#include "../flatground.h"

#define MAXDOTS 65536
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
  shadows[nShadows].size = size;
  nShadows++;
 }
 // add some gumdrops
 for (i=0; i<500; i++)
 {
  int x = rand()%30000 - 15000;
  int y = rand()%30000 - 15000;
  if (x*x+y*y>15000*15000) continue;
  int size = rand()%100+50;
  dots[nDots].x = x;
  dots[nDots].y = y;
  dots[nDots].z = size;
  dots[nDots].size = size;
  dots[nDots].red = (rand()&1)?0:255;
  dots[nDots].green = (rand()&1)?0:255;
  dots[nDots].blue = (rand()&1)?0:255;
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
 for (i=0; i<100; i++)
 {
  int x = rand()%30000 - 15000;
  int y = rand()%30000 - 15000;
  if (x*x+y*y>15000*15000) continue;
  #ifdef DISTRIBUTE_TREES
  int j; for (j=0; j<nTrees; j++) { if ((treepointsx[j]-x)*(treepointsx[j]-x)+(treepointsy[j]-y)*(treepointsy[j]-y)<2000*2000) break; } if(j<nTrees)continue; treepointsx[nTrees]=x;treepointsy[nTrees]=y; nTrees++;
  #endif
  int size = rand()%20+30;

  for (j=0; j<500; j++) {
   int xx = rand()%1000-500;
   int yy = rand()%1000-500;
   int zz = rand()%2000;
   if (xx*xx+yy*yy > (2000-zz)*(2000-zz)*0.0625) continue;
   dots[nDots].x = x+xx;
   dots[nDots].y = y+yy;
   dots[nDots].z = zz+300;
   dots[nDots].green = 160.0 - (double)(xx*xx+yy*yy)*(160.0/500.0/500.0);
   dots[nDots].size = 50;
   nDots++;
  }
  dots[nDots].x = x;
  dots[nDots].y = y;
  dots[nDots].z = 50;
  dots[nDots].green = 64;
  dots[nDots].size = 50;
  nDots++;
  dots[nDots].x = x;
  dots[nDots].y = y;
  dots[nDots].z = 150;
  dots[nDots].green = 96;
  dots[nDots].size = 50;
  nDots++;
  dots[nDots].x = x;
  dots[nDots].y = y;
  dots[nDots].z = 250;
  dots[nDots].green = 128;
  dots[nDots].size = 50;
  nDots++;
  shadows[nShadows].x = x;
  shadows[nShadows].y = y;
  shadows[nShadows].size = size*8;
  nShadows++;
 }
 #ifdef DISTRIBUTE_TREES
 printf("%d trees\n",nTrees);
 #endif


 for (i=0; i<100; i++)
 {
  int x = rand()%30000 - 15000;
  int y = rand()%30000 - 15000;
  if (x*x+y*y>15000*15000) continue;
  #ifdef DISTRIBUTE_TREES
  int j; for (j=0; j<nTrees; j++) { if ((treepointsx[j]-x)*(treepointsx[j]-x)+(treepointsy[j]-y)*(treepointsy[j]-y)<2000*2000) break; } if(j<nTrees)continue; treepointsx[nTrees]=x;treepointsy[nTrees]=y; nTrees++;
  #endif
  int size = rand()%20+30;

  for (j=0; j<500; j++) {
   int xx = rand()%1000-500;
   int yy = rand()%1000-500;
   int zz = rand()%2000;
   if (xx*xx+yy*yy > (2000-zz)*(2000-zz)*0.0625) continue;
   dots[nDots].x = x+xx;
   dots[nDots].y = y+yy;
   dots[nDots].z = zz+300;
   dots[nDots].green = 160.0 - (double)(xx*xx+yy*yy)*(160.0/500.0/500.0);
   dots[nDots].size = 50;
   nDots++;
  }
  dots[nDots].x = x;
  dots[nDots].y = y;
  dots[nDots].z = 50;
  dots[nDots].green = 64;
  dots[nDots].size = 50;
  nDots++;
  dots[nDots].x = x;
  dots[nDots].y = y;
  dots[nDots].z = 150;
  dots[nDots].green = 96;
  dots[nDots].size = 50;
  nDots++;
  dots[nDots].x = x;
  dots[nDots].y = y;
  dots[nDots].z = 250;
  dots[nDots].green = 128;
  dots[nDots].size = 50;
  nDots++;
  shadows[nShadows].x = x;
  shadows[nShadows].y = y;
  shadows[nShadows].size = size*8;
  nShadows++;
 }

 // add some candy canes
 for (i=0; i<100; i++)
 {
  int x = rand()%30000 - 15000;
  int y = rand()%30000 - 15000;
  if (x*x+y*y>15000*15000) continue;
  #ifdef DISTRIBUTE_TREES
  int j; for (j=0; j<nTrees; j++) { if ((treepointsx[j]-x)*(treepointsx[j]-x)+(treepointsy[j]-y)*(treepointsy[j]-y)<1000*1000) break; } if(j<nTrees)continue; treepointsx[nTrees]=x;treepointsy[nTrees]=y; nTrees++;
  #endif
  int size = rand()%50+5;
  double dx = RND();
  double dy = RND();
  double t = dx*dx+dy*dy;
  if (t <= 0 || t > 1) continue;
  t = 1.0 / sqrt(t);
  dx *= t;
  dy *= t;
  for (j=0; j<10; j++) {
   dots[nDots].x = x;
   dots[nDots].y = y;
   dots[nDots].z = j*size*2+size;
   dots[nDots].size = size;
   dots[nDots].red = 255;
   dots[nDots].green = (j&1)?255:0;
   dots[nDots].blue = (j&1)?255:0;
   nDots++;
  }
  for (j=0; j<12; j++) {
   double a = (1-cos(j*M_PI/10))*size*20/M_PI;
   dots[nDots].x = x + a*dx;
   dots[nDots].y = y + a*dy;
   dots[nDots].z = 21*size + sin(j*M_PI/10)*size*20/M_PI;
   dots[nDots].size = size;
   dots[nDots].red = 255;
   dots[nDots].green = (j&1)?255:0;
   dots[nDots].blue = (j&1)?255:0;
   nDots++;
  }
  shadows[nShadows].x = x;
  shadows[nShadows].y = y;
  shadows[nShadows].size = size*1.5;
  nShadows++;
 }
/*
 // add some snow
 for (i=0; i<1000; i++) {
  int x = rand()%30000 - 15000;
  int y = rand()%30000 - 15000;
  if (x*x+y*y>15000*15000) continue;
  dots[nDots].x = x;
  dots[nDots].y = y;
  dots[nDots].z = rand()%15000;
  dots[nDots].size = rand()%20+30;
  dots[nDots].red = 255;
  dots[nDots].green = 255;
  dots[nDots].blue = 255;
  nDots++;
 }*/
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
 _shadow_darkness = 0.25f;
 for (int i=0; i<nShadows; i++) {
  shadow(shadows[i].x    * (1.0f / VIEW_HEIGHT),
         shadows[i].y    * (1.0f / VIEW_HEIGHT),
         shadows[i].size * (1.0f / VIEW_HEIGHT));
 }
 shadow(0,0,40);
 glEndList();

 _view_direction=100;
 printf("%d dots\n", nDots);
}


void draw()
{
 update_view(0.07);
 shadow_mode();
 glCallList(dl_shadows);
 points_mode_ball(); //points_mode_soft();glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
 glCallList(dl_dots);
}


void done()
{
 glDeleteLists(dl_shadows, 1);
 glDeleteLists(dl_dots, 1);
 flatground_done();
}