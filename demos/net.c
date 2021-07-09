/***
 Network graph demo
 1. Generates a random set of points,
 2. Then adds random connections between them,
 3. Then shrinks the physical distance of the connections.
 When you like the result, press 'F' to freeze it into place and get a faster framerate.

 To compile this:  gcc net.c -o net -ffast-math -O -lGL -lglut -lm

 Author: Elie Goldman Smith
 This code is too simple to copyright, so it's in the public domain.
***/

#define POINT_SIZE_LIMIT_STRING "0.5"
#define VIEWPORT_SCALE 1.2
#include "../hypersphere.h"
#include "../line.h"

#define N_NODES 5000
#define NODE_FINAL_DISTANCE 0.03f
#define ELASTIC_INTERVAL 0.2f
#define DAMPING_INTERVAL 0.2f
#define WIND_RESISTANCE 10.0f


#define MAXDOTS 65536
typedef struct {
 vec4 pos;
 vec4 vel;
 vec3 color;
 float size;
} Dot;
Dot dots[MAXDOTS];
int nDots=0;

#define MAXRELATIONS 65536
typedef struct {
 int index1;
 int index2;
} Relation;
Relation relations[MAXRELATIONS];
int nRelations=0;

GLuint vbo_dots; // vertex buffer object handle
GLuint dl_lines=0; // display list
int stopped=0;


void init()
{
 hypersphere_init();

 for (int i=0; i<N_NODES; i++) {
  dots[nDots].pos = random_unit_vec4();
  dots[nDots].vel = new_vec4(0,0,0,0);
  dots[nDots].size = 0.004*pow(RND01()+0.001, -0.33);
  dots[nDots].color = new_vec3(1.0+RND(), 1.0+RND(), 1.0+RND());
  nDots++;
  relations[nRelations].index1 = i;
  relations[nRelations].index2 = rand()%N_NODES;
  nRelations++;
 }
 printf("%d dots, %d relations\n", nDots, nRelations);


 // set up the VBO
 glGenBuffers(1, &vbo_dots);
 glBindBuffer(GL_ARRAY_BUFFER, vbo_dots);
 glBufferData(GL_ARRAY_BUFFER, sizeof(Dot)*nDots, dots, GL_DYNAMIC_DRAW);
 glVertexPointer(4, GL_FLOAT, sizeof(Dot), (void*)offsetof(Dot,pos));
 glColorPointer(3, GL_FLOAT, sizeof(Dot), (void*)offsetof(Dot,color));
 glVertexAttribPointer(POINT_SIZE_ATTRIB, 1, GL_FLOAT, GL_FALSE, sizeof(Dot), (void*)offsetof(Dot,size)); // offsetof() could also be expressed as (void*)((void*)&dots[0].size - (void*)&dots[0]), for example.

 glEnable(GL_CULL_FACE);
}


void draw()
{
 update_view(DEFAULT_MOVE_INTERVAL);
 Dot* mb = dots; // i originally had this as a mapped buffer, but the problem is you'd have to unmap it before drawing the dots, and then you can't draw the lines using the same data. So I decided to use glBufferSubData() instead.

 // update the nodes and then draw them
 points_mode_ball();
 glBindBuffer(GL_ARRAY_BUFFER, vbo_dots);
 glEnableClientState(GL_VERTEX_ARRAY);
 glEnableClientState(GL_COLOR_ARRAY);
 glEnableVertexAttribArray(POINT_SIZE_ATTRIB);
 if (!stopped) {
  // the connections exert a force on the nodes (pulling together mostly)
  for (int i=0; i<nRelations; i++) {
   int i1 = relations[i].index1;
   int i2 = relations[i].index2;
   if (i1 >= 0 && i2 >= 0) {
    vec4 diff = sub_vec4(mb[i1].pos, mb[i2].pos);
    float distSq = dot_vec4(diff,diff);
    float invDist = 1.0f/sqrtf(distSq + 1e-18f);
    float dist = distSq * invDist;
    vec4 dir = mul_vec4(diff, invDist);
    if (dist < 1.9f) {
     vec4 dv = sub_vec4(mb[i1].vel, mb[i2].vel);
     vec4 accel = mul_vec4(dir, ELASTIC_INTERVAL*(dist-NODE_FINAL_DISTANCE)
                               +DAMPING_INTERVAL*dot_vec4(dv,dir));
     mb[i1].vel = sub_vec4(mb[i1].vel, accel);
     mb[i2].vel = add_vec4(mb[i2].vel, accel);
    }
    else relations[i].index1 = relations[i].index2 = -1; // break the connection if it's too long
   }
  }
  for (int i=0; i<nDots; i++) {
   // motion (inertia) (and normalize to maintain hypersphere rules)
   mb[i].pos = normalize_vec4(add_vec4(mb[i].pos, mb[i].vel));
   // orthogonalize velocity (to maintain hypersphere rules)
   mb[i].vel = sub_vec4(mb[i].vel, mul_vec4(mb[i].pos, dot_vec4(mb[i].vel, mb[i].pos)));
   // wind resistance
   float loss = length_vec4(mb[i].vel) * WIND_RESISTANCE;
   if (loss > 0.75f) loss = 0.75f;
   mb[i].vel = mul_vec4(mb[i].vel, 1.0f-loss);
  }

  glBufferSubData(GL_ARRAY_BUFFER, 0, nDots*sizeof(Dot), mb);
 }
 glDrawArrays(GL_POINTS, 0, nDots);
 glDisableVertexAttribArray(POINT_SIZE_ATTRIB);
 glDisableClientState(GL_COLOR_ARRAY);
 glDisableClientState(GL_VERTEX_ARRAY);
 glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind


 // draw the connections as lines
 default_mode();
 if (!stopped) {
  int freeze = (keymap['F']==KEY_FRESHLY_PRESSED); // 'f' for finish, finalize, freeze, fix, for a faster framerate
  if (freeze) {
   dl_lines = glGenLists(1);
   glNewList(dl_lines, GL_COMPILE);
  }
  for (int i=0; i<nRelations; i++) {
   int i1 = relations[i].index1;
   int i2 = relations[i].index2;
   if (i1 >= 0 && i2 >= 0) hypersphere_line(mb[i1].pos, mb[i2].pos,
                                            mul_vec3(add_vec3(mb[i1].color, mb[i2].color), 0.25f),
                                            0.5f*(mb[i1].size < mb[i2].size ? mb[i1].size : mb[i2].size));
  }
  if (freeze) {
   glEndList();
   stopped = 1;
  }
 }
 if (stopped) glCallList(dl_lines);
}


void done()
{
 if (stopped) glDeleteLists(dl_lines, 1);
 glDeleteBuffers(1, &vbo_dots);
 hypersphere_done();
}


