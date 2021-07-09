/***
 This is my design for a raised bed.
 The mattress goes on top, and there's room for a desk at the bottom.
 In real life, you could build this out of wood or steel. But I haven't made any spec for how the pieces should be joined.
 Also, panels could be added: on the railing, for better safety & privacy; and on the lower structure, to enable storage/shelving.
 Weights might need to be added to the bottom corners, on the side opposite the ladders, so the whole structure doesn't tip over when you're climbing in or out of bed.
 --

 To compile this:  gcc bed.c -o bed -ffast-math -O -lGL -lglut -lm

 Copyright 2021, Elie Goldman Smith

 This code is FREE SOFTWARE: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this.  If not, see <https://www.gnu.org/licenses/>.
***/
#define USE_MULTISAMPLING
#include "../flatground.h"
#define TALLY_LINES
#define LINES_INVENTORY_MAX_SIZE 256
#include "../line.h"

#define INCH (1.f/48.f) // Normally I would set it to 1/64, because a typical 5-foot-11 person would have eyes at 64 inches above the ground.  But for this model, 1/48 looks better.
#define FOOT (12.f*INCH)

#define MAXSHADOWS 256
typedef struct {
 float x, y, size;
} Shadow;
Shadow shadows[MAXSHADOWS];
int nShadows=0;

GLuint dl_geometry; // display-lists
GLuint dl_shadows;


void vertical(float x, float y, float height, vec3 color, float thickness) {
 line(new_vec3(x,y,0), new_vec3(x,y,height), color, thickness);
 shadows[nShadows].x = x;
 shadows[nShadows].y = y;
 shadows[nShadows].size = thickness*2.5f;
 nShadows++;
}


void box(vec3 a, vec3 b, vec3 c) // a and b are corner coords; c is the color
{
 glBegin(GL_QUADS);

 glColor3f(c.x+0.2f, c.y+0.2f, c.z+0.2f);
 glVertex3f(a.x, a.y, a.z);
 glVertex3f(a.x, b.y, a.z);
 glVertex3f(b.x, b.y, a.z);
 glVertex3f(b.x, a.y, a.z);

 glColor3f(c.x-0.2f, c.y-0.2f, c.z-0.2f);
 glVertex3f(b.x, b.y, b.z);
 glVertex3f(a.x, b.y, b.z);
 glVertex3f(a.x, a.y, b.z);
 glVertex3f(b.x, a.y, b.z);

 glColor3f(c.x+0.1f, c.y+0.1f, c.z+0.1f);
 glVertex3f(a.x, a.y, a.z);
 glVertex3f(a.x, a.y, b.z);
 glVertex3f(a.x, b.y, b.z);
 glVertex3f(a.x, b.y, a.z);

 glVertex3f(b.x, b.y, a.z);
 glVertex3f(b.x, b.y, b.z);
 glVertex3f(b.x, a.y, b.z);
 glVertex3f(b.x, a.y, a.z);

 glColor3f(c.x-0.1f, c.y-0.1f, c.z-0.1f);
 glVertex3f(a.x, a.y, a.z);
 glVertex3f(b.x, a.y, a.z);
 glVertex3f(b.x, a.y, b.z);
 glVertex3f(a.x, a.y, b.z);

 glVertex3f(a.x, b.y, b.z);
 glVertex3f(b.x, b.y, b.z);
 glVertex3f(b.x, b.y, a.z);
 glVertex3f(a.x, b.y, a.z);

 glEnd();
}


int compare(const void* a, const void* b) { // for qsort
 float diff = (*(vec2*)a).x - (*(vec2*)b).x;
 if (diff < -0.1f*INCH) return -1;
 if (diff >  0.1f*INCH) return  1;
 diff =       (*(vec2*)a).y - (*(vec2*)b).y;
 if (diff < 0) return -1;
 if (diff > 0) return  1;
 return 0;
}



void init()
{
 flatground_init();
 glEnable(GL_CULL_FACE);

 // design specs
 const float W = 54*INCH; // bed width
 const float L = 74*INCH; // bed length
 const float H = 64*INCH; // bed height
 const float P = 0.2*L; // pillar/ladder width
 const float R = 0.2*L; // railing height

 const float T = 0.75*INCH; // frame thickness (radius)
 const vec3  C = new_vec3(0,0,0); // frame color

 // Generate the main 3D model...

 dl_geometry = glGenLists(1);
 glNewList(dl_geometry, GL_COMPILE);

 // verticals
 vertical( W/2,   -L/2,    H+R, C,T*2);
 vertical( W/2,    L/2,    H+R, C,T*2);
 vertical(-W/2,    L/2,    H+R, C,T);
 vertical(-W/2,   -L/2,    H+R, C,T);
 vertical(-W/2,   -L/2+P,  H+R, C,T);
 vertical(-W/2,    L/2-P,  H+R, C,T);
 vertical(-W/2+P,  L/2,    H+R, C,T);
 vertical(-W/2+P, -L/2,    H+R, C,T);
 vertical(-W/2+P, -L/2+P,  H,   C,T);
 vertical(-W/2+P,  L/2-P,  H,   C,T);

 // big cross bracing
 line(new_vec3( W/2, -L/2,  H),
      new_vec3( W/2,  L/2,  0),
      C, T);
 line(new_vec3( W/2,  L/2,  H),
      new_vec3( W/2, -L/2,  0),
      C, T);
 line(new_vec3(-W/2,  L/2,  H),
      new_vec3( W/2,  L/2,  0),
      C, T);
 line(new_vec3( W/2,  L/2,  H),
      new_vec3(-W/2,  L/2,  0),
      C, T);
 line(new_vec3(-W/2, -L/2,  H),
      new_vec3( W/2, -L/2,  0),
      C, T);
 line(new_vec3( W/2, -L/2,  H),
      new_vec3(-W/2, -L/2,  0),
      C, T);

 // small cross bracing
 line(new_vec3(-W/2,   -L/2+P,  P),
      new_vec3(-W/2+P, -L/2+P,  0),
      C, T/2);
 line(new_vec3(-W/2+P, -L/2+P,  P),
      new_vec3(-W/2,   -L/2+P,  0),
      C, T/2);
 line(new_vec3(-W/2,   -L/2+P,  P),
      new_vec3(-W/2,   -L/2,    0),
      C, T/2);
 line(new_vec3(-W/2+P, -L/2+P,  P),
      new_vec3(-W/2+P, -L/2,    0),
      C, T/2);
 line(new_vec3(-W/2,   -L/2,    P),
      new_vec3(-W/2,   -L/2+P,  0),
      C, T/2);
 line(new_vec3(-W/2+P, -L/2,    P),
      new_vec3(-W/2+P, -L/2+P,  0),
      C, T/2);
 line(new_vec3(-W/2,   -L/2+P,  H-P),
      new_vec3(-W/2+P, -L/2+P,  H),
      C, T/2);
 line(new_vec3(-W/2+P, -L/2+P,  H-P),
      new_vec3(-W/2,   -L/2+P,  H),
      C, T/2);
 line(new_vec3(-W/2,   -L/2+P,  H+R),
      new_vec3(-W/2,   -L/2,    H),
      C, T/2);
 line(new_vec3(-W/2+P, -L/2+P,  H-P),
      new_vec3(-W/2+P, -L/2,    H),
      C, T/2);
 line(new_vec3(-W/2,   -L/2,    H+R),
      new_vec3(-W/2,   -L/2+P,  H),
      C, T/2);
 line(new_vec3(-W/2+P, -L/2,    H-P),
      new_vec3(-W/2+P, -L/2+P,  H),
      C, T/2);
 line(new_vec3(-W/2,   -L/2,    0),
      new_vec3(-W/2+P, -L/2+P,  0),
      C, T/2);
 line(new_vec3(-W/2+P, -L/2,    0),
      new_vec3(-W/2,   -L/2+P,  0),
      C, T/2);

 line(new_vec3(-W/2,    L/2-P,  P),
      new_vec3(-W/2+P,  L/2-P,  0),
      C, T/2);
 line(new_vec3(-W/2+P,  L/2-P,  P),
      new_vec3(-W/2,    L/2-P,  0),
      C, T/2);
 line(new_vec3(-W/2,    L/2-P,  P),
      new_vec3(-W/2,    L/2,    0),
      C, T/2);
 line(new_vec3(-W/2+P,  L/2-P,  P),
      new_vec3(-W/2+P,  L/2,    0),
      C, T/2);
 line(new_vec3(-W/2,    L/2,    P),
      new_vec3(-W/2,    L/2-P,  0),
      C, T/2);
 line(new_vec3(-W/2+P,  L/2,    P),
      new_vec3(-W/2+P,  L/2-P,  0),
      C, T/2);
 line(new_vec3(-W/2,    L/2-P,  H-P),
      new_vec3(-W/2+P,  L/2-P,  H),
      C, T/2);
 line(new_vec3(-W/2+P,  L/2-P,  H-P),
      new_vec3(-W/2,    L/2-P,  H),
      C, T/2);
 line(new_vec3(-W/2,    L/2-P,  H+R),
      new_vec3(-W/2,    L/2,    H),
      C, T/2);
 line(new_vec3(-W/2+P,  L/2-P,  H-P),
      new_vec3(-W/2+P,  L/2,    H),
      C, T/2);
 line(new_vec3(-W/2,    L/2,    H+R),
      new_vec3(-W/2,    L/2-P,  H),
      C, T/2);
 line(new_vec3(-W/2+P,  L/2,    H-P),
      new_vec3(-W/2+P,  L/2-P,  H),
      C, T/2);
 line(new_vec3(-W/2,    L/2,    0),
      new_vec3(-W/2+P,  L/2-P,  0),
      C, T/2);
 line(new_vec3(-W/2+P,  L/2,    0),
      new_vec3(-W/2,    L/2-P,  0),
      C, T/2);

 // ladders
 line(new_vec3(-W/2, -L/2,    H*0.3),
      new_vec3(-W/2, -L/2+P,  H*0.3),
      C, T);
 line(new_vec3(-W/2, -L/2,    H*0.5),
      new_vec3(-W/2, -L/2+P,  H*0.5),
      C, T);
 line(new_vec3(-W/2, -L/2,    H*0.7),
      new_vec3(-W/2, -L/2+P,  H*0.7),
      C, T);
 line(new_vec3(-W/2, -L/2,    H*0.9),
      new_vec3(-W/2, -L/2+P,  H*0.9),
      C, T);
 line(new_vec3(-W/2,  L/2,    H*0.3),
      new_vec3(-W/2,  L/2-P,  H*0.3),
      C, T);
 line(new_vec3(-W/2,  L/2,    H*0.5),
      new_vec3(-W/2,  L/2-P,  H*0.5),
      C, T);
 line(new_vec3(-W/2,  L/2,    H*0.7),
      new_vec3(-W/2,  L/2-P,  H*0.7),
      C, T);
 line(new_vec3(-W/2,  L/2,    H*0.9),
      new_vec3(-W/2,  L/2-P,  H*0.9),
      C, T);
 line(new_vec3(-W/2,   L/2,   H*0.3),
      new_vec3(-W/2+P, L/2,   H*0.3),
      C, T);
 line(new_vec3(-W/2,   L/2,   H*0.5),
      new_vec3(-W/2+P, L/2,   H*0.5),
      C, T);
 line(new_vec3(-W/2,   L/2,   H*0.7),
      new_vec3(-W/2+P, L/2,   H*0.7),
      C, T);
 line(new_vec3(-W/2,   L/2-P, H*0.3),
      new_vec3(-W/2+P, L/2-P, H*0.3),
      C, T);
 line(new_vec3(-W/2,   L/2-P, H*0.5),
      new_vec3(-W/2+P, L/2-P, H*0.5),
      C, T);
 line(new_vec3(-W/2,   L/2-P, H*0.7),
      new_vec3(-W/2+P, L/2-P, H*0.7),
      C, T);
 line(new_vec3(-W/2,  -L/2+P, H*0.3),
      new_vec3(-W/2+P,-L/2+P, H*0.3),
      C, T);
 line(new_vec3(-W/2,  -L/2+P, H*0.5),
      new_vec3(-W/2+P,-L/2+P, H*0.5),
      C, T);
 line(new_vec3(-W/2,  -L/2+P, H*0.7),
      new_vec3(-W/2+P,-L/2+P, H*0.7),
      C, T);
 line(new_vec3(-W/2,  -L/2,   H*0.3),
      new_vec3(-W/2+P,-L/2,   H*0.3),
      C, T);
 line(new_vec3(-W/2,  -L/2,   H*0.5),
      new_vec3(-W/2+P,-L/2,   H*0.5),
      C, T);
 line(new_vec3(-W/2,  -L/2,   H*0.7),
      new_vec3(-W/2+P,-L/2,   H*0.7),
      C, T);

 // top frame
 line(new_vec3( W/2, -L/2,  H),
      new_vec3( W/2,  L/2,  H),
      C, T);
 line(new_vec3(-W/2, -L/2,  H),
      new_vec3(-W/2,  L/2,  H),
      C, T);
 line(new_vec3(-W/2, -0.5*L,  H),
      new_vec3( W/2, -0.5*L,  H),
      C, T);
 line(new_vec3(-W/2, -0.4*L,  H),
      new_vec3( W/2, -0.4*L,  H),
      C, T);
 line(new_vec3(-W/2, -0.3*L,  H),
      new_vec3( W/2, -0.3*L,  H),
      C, T);
 line(new_vec3(-W/2, -0.2*L,  H),
      new_vec3( W/2, -0.2*L,  H),
      C, T);
 line(new_vec3(-W/2, -0.1*L,  H),
      new_vec3( W/2, -0.1*L,  H),
      C, T);
 line(new_vec3(-W/2,  0.0*L,  H),
      new_vec3( W/2,  0.0*L,  H),
      C, T);
 line(new_vec3(-W/2,  0.1*L,  H),
      new_vec3( W/2,  0.1*L,  H),
      C, T);
 line(new_vec3(-W/2,  0.2*L,  H),
      new_vec3( W/2,  0.2*L,  H),
      C, T);
 line(new_vec3(-W/2,  0.3*L,  H),
      new_vec3( W/2,  0.3*L,  H),
      C, T);
 line(new_vec3(-W/2,  0.4*L,  H),
      new_vec3( W/2,  0.4*L,  H),
      C, T);
 line(new_vec3(-W/2,  0.5*L,  H),
      new_vec3( W/2,  0.5*L,  H),
      C, T);
 line(new_vec3(-W/2, -L/2,  H),
      new_vec3( W/2,  L/2,  H),
      C, T);
 line(new_vec3(-W/2,  L/2,  H),
      new_vec3( W/2, -L/2,  H),
      C, T);

 // rails
 line(new_vec3(-W/2, -L/2,  H+R),
      new_vec3(-W/2,  L/2,  H+R),
      C, T);
 line(new_vec3( W/2, -L/2,  H+R),
      new_vec3( W/2,  L/2,  H+R),
      C, T);
 line(new_vec3(-W/2,  L/2,  H+R),
      new_vec3( W/2,  L/2,  H+R),
      C, T);
 line(new_vec3(-W/2, -L/2,  H+R),
      new_vec3( W/2, -L/2,  H+R),
      C, T);
 line(new_vec3( W/2, -L/2,  H+R),
      new_vec3( W/2,  0,    H),
      C, T);
 line(new_vec3( W/2,  L/2,  H+R),
      new_vec3( W/2,  0,    H),
      C, T);
 line(new_vec3(-W/2, -L/2+P,H+R),
      new_vec3(-W/2,  0,    H),
      C, T*0.7);
 line(new_vec3(-W/2,  L/2-P,H+R),
      new_vec3(-W/2,  0,    H),
      C, T*0.7);
 line(new_vec3( W/2,  L/2,  H+R),
      new_vec3( P/2,  L/2,    H),
      C, T*0.7);
 line(new_vec3( W/2, -L/2,  H+R),
      new_vec3( P/2, -L/2,    H),
      C, T*0.7);

 // mattress
 box(new_vec3(-W/2+T, -L/2+T, H+T),
     new_vec3( W/2-T,  L/2-T, H+T+9*INCH),
     new_vec3( 0.4f, 0.4f, 0.6f ));

 glEndList(); // Done generating 3D model.


 // soft shadows
 dl_shadows = glGenLists(1);
 glNewList(dl_shadows, GL_COMPILE);
 for (int i=0; i<nShadows; i++) shadow(shadows[i].x, shadows[i].y, shadows[i].size);
 glEndList();


 // set the viewpoint
 _view_Xpos = -1.25*L;
 _view_Ypos = 0;
 _view_direction = 90;
 _view_eyelevel = 96;


 // use some fog because otherwise all the lines are shaded the same
 float bgcolor[4] = {1,1,1,1};
 glClearColor(bgcolor[0], bgcolor[1], bgcolor[2], bgcolor[3]);
 glEnable(GL_FOG);
 glFogfv(GL_FOG_COLOR, (GLfloat*)&bgcolor);
 glFogf(GL_FOG_DENSITY, 0.25f);
 glFogi(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH);


 // materials usage report
 qsort(_lines_inventory, _lines_inventory_size, sizeof(vec2), compare);
 for (int i=0; i<_lines_inventory_size; i++)
  printf("%d-inch piece (%.2f inch diameter)\n",
   (int)(0.5f+ _lines_inventory[i].x/INCH),
   2.0f*_lines_inventory[i].y/INCH);

 const float density = 0.036; // density of water (in lbs per cubic inch), which is also approx the density of hollow steel tubes when you include the air.
 printf("Set: %d frame pieces (not including joints)\n", _lines_inventory_size);
 printf("Total length of frame pieces: %d feet\n", (int)(0.5f+ _total_line_length/FOOT));
 printf("Approximate weight of frame : %d lbs\n", (int)(0.5f+ density*_total_line_volume/(INCH*INCH*INCH)));
}



void draw()
{
 update_view(0.02);
 shadow_mode();
 glCallList(dl_shadows);
 default_mode();
 glCallList(dl_geometry);
}



void done()
{
 flatground_done();
 glDeleteLists(dl_shadows, 1);
 glDeleteLists(dl_geometry, 1);
}
