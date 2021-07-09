/***
 I made a cool looking track lol
 You could probably use this as a screensaver.

 To compile this code:  gcc track.c -o track -ffast-math -O -lGL -lglut -lm

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


#define WHITE_BACKGROUND
#define VIEWPORT_SCALE 1.2
#include "../hypersphere.h"

#define TRACK_TURN  0.14
#define TRACK_TWIST 0.05
#define TRACK_SPEED 1.00
#define VIEW_STICKINESS   0.10
#define VIEW_CAMERA_BIND  0.02
#define VIEW_UPRIGHTING   0.08
#define HEART_STEP 0.25
#define MAXDOTS 65536


typedef struct
{
float x,y,z,w;
float size;
unsigned char red,green,blue;
} Dot;

Dot dots[MAXDOTS] = {0};
int nDots = 0;

typedef struct
{
 float x,y,z,w;
 float ry_x,ry_y,ry_z,ry_w;
 float rz_x,rz_y,rz_z,rz_w;
 float speed;
} PathPoint;

PathPoint path[MAXDOTS] = {0};
int pathSize = 0;

GLuint vbo=0;








// My only regret is not using vector data types for this. Oh well, too late!
void init() {
 int i;
 srand(34);

 // make the track
 float w=1,x=0,y=0,z=0;
 float size = 0.01;
 int green = 160;
 int rb = 64;
 float dw = 0;
 float dx = 0;
 float dy = 0;
 float dz = 1;
 float tw = 0;
 float tx = 1;
 float ty = 0;
 float tz = 0;
 float uw = 0;
 float ux = 0;
 float uy = 1;
 float uz = 0;
 float speed = 1;
 for (i=0; i<9000; i++) {
  // update the direction
  dw += RND()*TRACK_TURN;
  dx += RND()*TRACK_TURN;
  dy += RND()*TRACK_TURN;
  dz += RND()*TRACK_TURN;
  // perpendicularize the direction
  float t = dw*w + dx*x + dy*y + dz*z;
  dw -= w*t;
  dx -= x*t;
  dy -= y*t;
  dz -= z*t;
  // normalize the direction
  t = dw*dw+dx*dx+dy*dy+dz*dz;
  if (t==0) continue;
  t = 1.0 / sqrtf(t);
  dw *= t; dx *= t; dy *= t; dz *= t;

  // update the position
  w += dw*size;
  x += dx*size;
  y += dy*size;
  z += dz*size;
  // normalize the position
  t = w*w+x*x+y*y+z*z;
  if (t==0) continue;
  t = 1.0 / sqrtf(t);
  w *= t; x *= t; y *= t; z *= t;

  // update the track orientation
  tw += RND()*TRACK_TWIST;
  tx += RND()*TRACK_TWIST;
  ty += RND()*TRACK_TWIST;
  tz += RND()*TRACK_TWIST;
  // perpendicularize the track orientation
  t = dw*tw + dx*tx + dy*ty + dz*tz;
  tw -= dw*t;
  tx -= dx*t;
  ty -= dy*t;
  tz -= dz*t;
  t = w*tw + x*tx + y*ty + z*tz;
  tw -= w*t;
  tx -= x*t;
  ty -= y*t;
  tz -= z*t;
  // normalize the track orientation
  t = tw*tw + tx*tx + ty*ty + tz*tz;
  if (t==0) continue;
  t = 1.0 / sqrtf(t);
  tw *= t;
  tx *= t;
  ty *= t;
  tz *= t;

  // perpendicularize the up vector
  t = w*uw + x*ux + y*uy + z*uz;
  uw -= w*t;
  ux -= x*t;
  uy -= y*t;
  uz -= z*t;
  t = tw*uw + tx*ux + ty*uy + tz*uz;
  uw -= tw*t;
  ux -= tx*t;
  uy -= ty*t;
  uz -= tz*t;
  t = dw*uw + dx*ux + dy*uy + dz*uz;
  uw -= dw*t;
  ux -= dx*t;
  uy -= dy*t;
  uz -= dz*t;
  // update speed based on track curvature
  speed = speed*0.997 + 0.003 - t*0.5; if(speed<=0.5)speed=0.5;
  // normalize the up vector
  t = uw*uw + ux*ux + uy*uy + uz*uz;
  if (t==0) continue;
  t = 1.0 / sqrtf(t);
  uw *= t;
  ux *= t;
  uy *= t;
  uz *= t;

  dots[nDots].w = w+tw*size*4;
  dots[nDots].x = x+tx*size*4;
  dots[nDots].y = y+ty*size*4;
  dots[nDots].z = z+tz*size*4;
  dots[nDots].size = size*0.8;
  dots[nDots].red = dots[nDots].blue = rb;
  dots[nDots].green = green;
  nDots++;
  dots[nDots].w = w-tw*size*4;
  dots[nDots].x = x-tx*size*4;
  dots[nDots].y = y-ty*size*4;
  dots[nDots].z = z-tz*size*4;
  dots[nDots].size = size*0.8;
  dots[nDots].red = dots[nDots].blue = rb;
  dots[nDots].green = green;
  nDots++;
  if (i%8==0) {
   dots[nDots].w = w;
   dots[nDots].x = x;
   dots[nDots].y = y;
   dots[nDots].z = z;
   dots[nDots].size = size*0.4;
   dots[nDots].red = dots[nDots].blue = rb;
   dots[nDots].green = green;
   nDots++;
   dots[nDots].w = w - dw*size*2 - tw*size;
   dots[nDots].x = x - dx*size*2 - tx*size;
   dots[nDots].y = y - dy*size*2 - ty*size;
   dots[nDots].z = z - dz*size*2 - tz*size;
   dots[nDots].size = size*0.4;
   dots[nDots].red = dots[nDots].blue = rb;
   dots[nDots].green = green;
   nDots++;
   dots[nDots].w = w - dw*size*2 + tw*size;
   dots[nDots].x = x - dx*size*2 + tx*size;
   dots[nDots].y = y - dy*size*2 + ty*size;
   dots[nDots].z = z - dz*size*2 + tz*size;
   dots[nDots].size = size*0.4;
   dots[nDots].red = dots[nDots].blue = rb;
   dots[nDots].green = green;
   nDots++;
  }
  path[pathSize].w = w + uw*size*7;
  path[pathSize].x = x + ux*size*7;
  path[pathSize].y = y + uy*size*7;
  path[pathSize].z = z + uz*size*7;
  path[pathSize].rz_w = dw;
  path[pathSize].rz_x = dx;
  path[pathSize].rz_y = dy;
  path[pathSize].rz_z = dz;
  path[pathSize].ry_w = uw;
  path[pathSize].ry_x = ux;
  path[pathSize].ry_y = uy;
  path[pathSize].ry_z = uz;
  path[pathSize].speed = speed;
  pathSize++;

  green += rand()%9-4; if (green<0)green=0; if(green>216)green=216;
  rb += rand()%9-4; if (rb<0)rb=0; if(rb>green)rb=green;
 }
 // marker at the end of the track
 dots[nDots].w = w + uw*size*5;
 dots[nDots].x = x + ux*size*5;
 dots[nDots].y = y + uy*size*5;
 dots[nDots].z = z + uz*size*5;
 dots[nDots].size = size*5;
 dots[nDots].red = 255;
 dots[nDots].green = dots[nDots].blue = 0;
 nDots++;
 // make some random particles
 for (i=0; i<1000; i++) {
  float w=RND()-1, x=RND(), y=RND(), z=RND();
  float t = w*w+x*x+y*y+z*z;
  if (t>1 || t==0) continue;
  t = 1.0 / sqrtf(t);
  dots[nDots].w = w*t;
  dots[nDots].x = x*t;
  dots[nDots].y = y*t;
  dots[nDots].z = z*t;
  dots[nDots].size = rand()*(0.01/RAND_MAX);
  dots[nDots].red = dots[nDots].green = dots[nDots].blue = 0;
  nDots++;
 }
 srand(9);

 // make some hearts
 for (i=0; i<5000; i++) {
  // random location
  float w = RND()+1;
  float x = RND();
  float y = RND();
  float z = RND();
  float t = w*w+x*x+y*y+z*z;
  if (t>1 || t<=0) continue;
  t = 1.0 / sqrtf(t);
  w *= t;
  x *= t;
  y *= t;
  z *= t;
  // random size
  float size = (RND()+2)*0.02;
  // random pink color
  int blue = rand()%256;
  int green = rand()%(blue+1);
  // random orientation
  float rx_w = RND(); // random vector
  float rx_x = RND(); //
  float rx_y = RND(); //
  float rx_z = RND(); //
  t = rx_w*rx_w + rx_x*rx_x + rx_y*rx_y + rx_z*rx_z;
  if (t>1 || t<=0) continue; // filter out defunct possibilities
  t = w*rx_w + x*rx_x + y*rx_y + z*rx_z;
  rx_w -= w*t; // orthogonalize
  rx_x -= x*t; //
  rx_y -= y*t; //
  rx_z -= z*t; //
  t = rx_w*rx_w + rx_x*rx_x + rx_y*rx_y + rx_z*rx_z;
  if (t==0) continue;
  t = 1.0 / sqrtf(t);
  rx_w *= t; // normalize
  rx_x *= t; //
  rx_y *= t; //
  rx_z *= t; //
  float ry_w = RND(); // another random vector
  float ry_x = RND(); //
  float ry_y = RND(); //
  float ry_z = RND(); //
  t = ry_w*ry_w + ry_x*ry_x + ry_y*ry_y + ry_z*ry_z;
  if (t>1 || t<=0) continue; // filter out defunct possibilities
  t = rx_w*ry_w + rx_x*ry_x + rx_y*ry_y + rx_z*ry_z;
  ry_w -= rx_w*t; // orthogonalize
  ry_x -= rx_x*t; //
  ry_y -= rx_y*t; //
  ry_z -= rx_z*t; //
  t = w*ry_w + x*ry_x + y*ry_y + z*ry_z;
  ry_w -= w*t; // orthogonalize
  ry_x -= x*t; //
  ry_y -= y*t; //
  ry_z -= z*t; //
  t = ry_w*ry_w + ry_x*ry_x + ry_y*ry_y + ry_z*ry_z;
  if (t==0) continue;
  t = size / sqrtf(t);
  ry_w *= t; // normalize and scale
  ry_x *= t; //
  ry_y *= t; //
  ry_z *= t; //
  rx_w *= size; // scale
  rx_x *= size; //
  rx_y *= size; //
  rx_z *= size; //

  // draw the heart shape
  size *= HEART_STEP;
  for (t=0; t<1.25; t+=HEART_STEP) {
   dots[nDots].w = w + t*rx_w;
   dots[nDots].x = x + t*rx_x;
   dots[nDots].y = y + t*rx_y;
   dots[nDots].z = z + t*rx_z;
   dots[nDots].size = size;
   dots[nDots].red = 255;
   dots[nDots].green = 0;
   dots[nDots].blue = blue;
   nDots++;
  }
  for (t=HEART_STEP; t<1.25; t+=HEART_STEP) {
   dots[nDots].w = w + t*ry_w;
   dots[nDots].x = x + t*ry_x;
   dots[nDots].y = y + t*ry_y;
   dots[nDots].z = z + t*ry_z;
   dots[nDots].size = size;
   dots[nDots].red = 255;
   dots[nDots].green = 0;
   dots[nDots].blue = blue;
   nDots++;
  }
  for (t=1.25; t>=1; t-=HEART_STEP) {
   dots[nDots].w = w + t*rx_w + ry_w;
   dots[nDots].x = x + t*rx_x + ry_x;
   dots[nDots].y = y + t*rx_y + ry_y;
   dots[nDots].z = z + t*rx_z + ry_z;
   dots[nDots].size = size;
   dots[nDots].red = 255;
   dots[nDots].green = 0;
   dots[nDots].blue = blue;
   nDots++;
  }
  for (t=1.25; t>=1; t-=HEART_STEP) {
   dots[nDots].w = w + t*ry_w + rx_w;
   dots[nDots].x = x + t*ry_x + rx_x;
   dots[nDots].y = y + t*ry_y + rx_y;
   dots[nDots].z = z + t*ry_z + rx_z;
   dots[nDots].size = size;
   dots[nDots].red = 255;
   dots[nDots].green = 0;
   dots[nDots].blue = blue;
   nDots++;
  }
  for (t=0; t<M_PI; t+=HEART_STEP*2) {
   dots[nDots].w = w + (0.5-0.5*cosf(t))*rx_w + (1.25+0.5*sinf(t))*ry_w;
   dots[nDots].x = x + (0.5-0.5*cosf(t))*rx_x + (1.25+0.5*sinf(t))*ry_x;
   dots[nDots].y = y + (0.5-0.5*cosf(t))*rx_y + (1.25+0.5*sinf(t))*ry_y;
   dots[nDots].z = z + (0.5-0.5*cosf(t))*rx_z + (1.25+0.5*sinf(t))*ry_z;
   dots[nDots].size = size;
   dots[nDots].red = 255;
   dots[nDots].green = 0;
   dots[nDots].blue = blue;
   nDots++;
  }
  for (t=0; t<M_PI; t+=HEART_STEP*2) {
   dots[nDots].w = w + (0.5-0.5*cosf(t))*ry_w + (1.25+0.5*sinf(t))*rx_w;
   dots[nDots].x = x + (0.5-0.5*cosf(t))*ry_x + (1.25+0.5*sinf(t))*rx_x;
   dots[nDots].y = y + (0.5-0.5*cosf(t))*ry_y + (1.25+0.5*sinf(t))*rx_y;
   dots[nDots].z = z + (0.5-0.5*cosf(t))*ry_z + (1.25+0.5*sinf(t))*rx_z;
   dots[nDots].size = size;
   dots[nDots].red = 255;
   dots[nDots].green = 0;
   dots[nDots].blue = blue;
   nDots++;
  }
 }

 // make some squares
 for (i=0; i<5000; i++) {
  // random location
  float w = RND()-1;
  float x = RND();
  float y = RND();
  float z = RND();
  float t = w*w+x*x+y*y+z*z;
  if (t>1 || t<=0) continue;
  t = 1.0 / sqrtf(t);
  w *= t;
  x *= t;
  y *= t;
  z *= t;
  // random size
  float size = (RND()+2)*0.02;
  // random bluish-purplish color
  int red = rand()%256;
  int green = rand()%(red+1);
  // random orientation
  float rx_w = RND(); // random vector
  float rx_x = RND(); //
  float rx_y = RND(); //
  float rx_z = RND(); //
  t = rx_w*rx_w + rx_x*rx_x + rx_y*rx_y + rx_z*rx_z;
  if (t>1 || t<=0) continue; // filter out defunct possibilities
  t = w*rx_w + x*rx_x + y*rx_y + z*rx_z;
  rx_w -= w*t; // orthogonalize
  rx_x -= x*t; //
  rx_y -= y*t; //
  rx_z -= z*t; //
  t = rx_w*rx_w + rx_x*rx_x + rx_y*rx_y + rx_z*rx_z;
  if (t==0) continue;
  t = 1.0 / sqrtf(t);
  rx_w *= t; // normalize
  rx_x *= t; //
  rx_y *= t; //
  rx_z *= t; //
  float ry_w = RND(); // another random vector
  float ry_x = RND(); //
  float ry_y = RND(); //
  float ry_z = RND(); //
  t = ry_w*ry_w + ry_x*ry_x + ry_y*ry_y + ry_z*ry_z;
  if (t>1 || t<=0) continue; // filter out defunct possibilities
  t = rx_w*ry_w + rx_x*ry_x + rx_y*ry_y + rx_z*ry_z;
  ry_w -= rx_w*t; // orthogonalize
  ry_x -= rx_x*t; //
  ry_y -= rx_y*t; //
  ry_z -= rx_z*t; //
  t = w*ry_w + x*ry_x + y*ry_y + z*ry_z;
  ry_w -= w*t; // orthogonalize
  ry_x -= x*t; //
  ry_y -= y*t; //
  ry_z -= z*t; //
  t = ry_w*ry_w + ry_x*ry_x + ry_y*ry_y + ry_z*ry_z;
  if (t==0) continue;
  t = size / sqrtf(t);
  ry_w *= t; // normalize and scale
  ry_x *= t; //
  ry_y *= t; //
  ry_z *= t; //
  rx_w *= size; // scale
  rx_x *= size; //
  rx_y *= size; //
  rx_z *= size; //

  // draw the square shape
  size *= HEART_STEP;
  for (t=0; t<1; t+=HEART_STEP) {
   dots[nDots].w = w + t*rx_w;
   dots[nDots].x = x + t*rx_x;
   dots[nDots].y = y + t*rx_y;
   dots[nDots].z = z + t*rx_z;
   dots[nDots].size = size;
   dots[nDots].blue = 255;
   dots[nDots].green = green;
   dots[nDots].red = 0;
   nDots++;
  }
  for (t=0; t<1; t+=HEART_STEP) {
   dots[nDots].w = w + t*rx_w + ry_w; // change to rx_w for an interesting F shape
   dots[nDots].x = x + t*rx_x + ry_x;
   dots[nDots].y = y + t*rx_y + ry_y;
   dots[nDots].z = z + t*rx_z + ry_z;
   dots[nDots].size = size;
   dots[nDots].blue = 255;
   dots[nDots].green = green;
   dots[nDots].red = 0;
   nDots++;
  }
  for (t=1-HEART_STEP; t>0; t-=HEART_STEP) {
   dots[nDots].w = w + t*ry_w;
   dots[nDots].x = x + t*ry_x;
   dots[nDots].y = y + t*ry_y;
   dots[nDots].z = z + t*ry_z;
   dots[nDots].size = size;
   dots[nDots].blue = 255;
   dots[nDots].green = green;
   dots[nDots].red = 0;
   nDots++;
  }
  for (t=1; t>=0; t-=HEART_STEP) { // change the >= to > for a diamondish shape
   dots[nDots].w = w + t*ry_w + rx_w;
   dots[nDots].x = x + t*ry_x + rx_x;
   dots[nDots].y = y + t*ry_y + rx_y;
   dots[nDots].z = z + t*ry_z + rx_z;
   dots[nDots].size = size;
   dots[nDots].blue = 255;
   dots[nDots].green = green;
   dots[nDots].red = 0;
   nDots++;
  }
 }

 // make some rings
 for (i=0; i<5000; i++) {
  // random location
  float w = RND()-1;
  float x = RND();
  float y = RND();
  float z = RND();
  float t = w*w+x*x+y*y+z*z;
  if (t>1 || t<=0) continue;
  t = 1.0 / sqrtf(t);
  w *= t;
  x *= t;
  y *= t;
  z *= t;
  // random size
  float size = (RND()+2)*0.02;
  // random yellowish color
  int green = rand()%128+128;
  int blue = rand()%(green+1);
  // random orientation
  float rx_w = RND(); // random vector
  float rx_x = RND(); //
  float rx_y = RND(); //
  float rx_z = RND(); //
  t = rx_w*rx_w + rx_x*rx_x + rx_y*rx_y + rx_z*rx_z;
  if (t>1 || t<=0) continue; // filter out defunct possibilities
  t = w*rx_w + x*rx_x + y*rx_y + z*rx_z;
  rx_w -= w*t; // orthogonalize
  rx_x -= x*t; //
  rx_y -= y*t; //
  rx_z -= z*t; //
  t = rx_w*rx_w + rx_x*rx_x + rx_y*rx_y + rx_z*rx_z;
  if (t==0) continue;
  t = 1.0 / sqrtf(t);
  rx_w *= t; // normalize
  rx_x *= t; //
  rx_y *= t; //
  rx_z *= t; //
  float ry_w = RND(); // another random vector
  float ry_x = RND(); //
  float ry_y = RND(); //
  float ry_z = RND(); //
  t = ry_w*ry_w + ry_x*ry_x + ry_y*ry_y + ry_z*ry_z;
  if (t>1 || t<=0) continue; // filter out defunct possibilities
  t = rx_w*ry_w + rx_x*ry_x + rx_y*ry_y + rx_z*ry_z;
  ry_w -= rx_w*t; // orthogonalize
  ry_x -= rx_x*t; //
  ry_y -= rx_y*t; //
  ry_z -= rx_z*t; //
  t = w*ry_w + x*ry_x + y*ry_y + z*ry_z;
  ry_w -= w*t; // orthogonalize
  ry_x -= x*t; //
  ry_y -= y*t; //
  ry_z -= z*t; //
  t = ry_w*ry_w + ry_x*ry_x + ry_y*ry_y + ry_z*ry_z;
  if (t==0) continue;
  t = size / sqrtf(t);
  ry_w *= t; // normalize and scale
  ry_x *= t; //
  ry_y *= t; //
  ry_z *= t; //
  rx_w *= size; // scale
  rx_x *= size; //
  rx_y *= size; //
  rx_z *= size; //

  // draw the circle shape
  size *= HEART_STEP;
  for (t=0; t<M_PI*2; t+=HEART_STEP) {
   dots[nDots].w = w + cosf(t)*rx_w + sinf(t)*ry_w;
   dots[nDots].x = x + cosf(t)*rx_x + sinf(t)*ry_x;
   dots[nDots].y = y + cosf(t)*rx_y + sinf(t)*ry_y;
   dots[nDots].z = z + cosf(t)*rx_z + sinf(t)*ry_z;
   dots[nDots].size = size;
   dots[nDots].red = 255;
   dots[nDots].green = green;
   dots[nDots].blue = 0;
   nDots++;
  }
 }
 printf("%d dots\n", nDots);

 //////////////////
 hypersphere_init();
 points_mode_ball();

 // set up the VBO
 glGenBuffers(1, &vbo);
 glBindBuffer(GL_ARRAY_BUFFER, vbo);
 glBufferData(GL_ARRAY_BUFFER, sizeof(Dot)*nDots, dots, GL_STATIC_DRAW);
 glEnableClientState(GL_VERTEX_ARRAY);
 glEnableClientState(GL_COLOR_ARRAY);
 glEnableVertexAttribArray(POINT_SIZE_ATTRIB);
 glVertexPointer(4, GL_FLOAT, sizeof(Dot), (void*)offsetof(Dot,x));
 glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(Dot), (void*)offsetof(Dot,red));
 glVertexAttribPointer(POINT_SIZE_ATTRIB, 1, GL_FLOAT, GL_FALSE, sizeof(Dot), (void*)offsetof(Dot,size)); // offsetof() could also be expressed as (void*)((void*)&dots[0].size - (void*)&dots[0]), for example.
}












void draw()
{
 // follow path
 static float pathPoint=0;
 int pp = pathPoint;
 float a = pathPoint-pp;
 _view_data.Wpos   += VIEW_STICKINESS*(  (1-a)*path[pp].w    + a*path[pp+1].w    );
 _view_data.Xpos   += VIEW_STICKINESS*(  (1-a)*path[pp].x    + a*path[pp+1].x    );
 _view_data.Ypos   += VIEW_STICKINESS*(  (1-a)*path[pp].y    + a*path[pp+1].y    );
 _view_data.Zpos   += VIEW_STICKINESS*(  (1-a)*path[pp].z    + a*path[pp+1].z    );
 _view_data.relz_w += VIEW_CAMERA_BIND*( (1-a)*path[pp].rz_w + a*path[pp+1].rz_w );
 _view_data.relz_x += VIEW_CAMERA_BIND*( (1-a)*path[pp].rz_x + a*path[pp+1].rz_x );
 _view_data.relz_y += VIEW_CAMERA_BIND*( (1-a)*path[pp].rz_y + a*path[pp+1].rz_y );
 _view_data.relz_z += VIEW_CAMERA_BIND*( (1-a)*path[pp].rz_z + a*path[pp+1].rz_z );
 _view_data.rely_w += VIEW_UPRIGHTING*(  (1-a)*path[pp].ry_w + a*path[pp+1].ry_w );
 _view_data.rely_x += VIEW_UPRIGHTING*(  (1-a)*path[pp].ry_x + a*path[pp+1].ry_x );
 _view_data.rely_y += VIEW_UPRIGHTING*(  (1-a)*path[pp].ry_y + a*path[pp+1].ry_y );
 _view_data.rely_z += VIEW_UPRIGHTING*(  (1-a)*path[pp].ry_z + a*path[pp+1].ry_z );
 pathPoint         += TRACK_SPEED*(      (1-a)*path[pp].speed+ a*path[pp+1].speed);
 if (pathPoint >= pathSize-1) pathPoint=0;

 // update_view() but with some features overridden
 _view_dw = _view_dx = _view_dy = _view_dz = 0;
 _view_roll_speed = 0.04;
 _view_anchor = 1;
 keymap[8] = 0;
 update_view(0.014);

 // draw the dots
 glDrawArrays(GL_POINTS, 0, nDots);
}












void done()
{
 // deactivate/delete the VBO
 glDisableVertexAttribArray(POINT_SIZE_ATTRIB);
 glDisableClientState(GL_COLOR_ARRAY);
 glDisableClientState(GL_VERTEX_ARRAY);
 glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
 glDeleteBuffers(1, &vbo);

 hypersphere_done();
}
