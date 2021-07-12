/***
 Similar to the original spaceweed, but you can change some of the parameters on the fly, by pressing I,J,K,L
 To compile this: gcc spaceweed2.c -o spaceweed2 -ffast-math -O -lGL -lglut -lm

 Author: Elie Goldman Smith
 This code is too simple to copyright, so it is in the public domain.
***/
#define VIEWPORT_SCALE 1.5
#define POINT_SIZE_LIMIT_STRING "0.5"
#include "../hypersphere.h"
#include "../vectors.h"

#define MAXDOTS 65536
#define INITIAL_DOT_SIZE 0.02
#define SIZE_INCREMENT 0.0001
#define N_BRANCHES 80
#define DIRECTION_DEVIATION_1 0.16
#define DIRECTION_DEVIATION_2 0.02
#define LIGHTNESS_DEVIATION 0.1
#define N_STARS 2000

typedef struct {
 vec4 pos;
 vec3 color;
 float size;
} Dot;

Dot dots[MAXDOTS];
int nDots=0;

GLuint vbo_dots; // vertex buffer object handle
Dot* mb_dots=dots; // pointer to mapped buffer

float direction_deviation_1 = DIRECTION_DEVIATION_1;
float direction_deviation_2 = DIRECTION_DEVIATION_2;

void generate() {
 srand(12);
 mb_dots[0].pos = new_vec4(0, sqrt(0.01), sqrt(0.99), 0);
 mb_dots[0].size = INITIAL_DOT_SIZE;
 mb_dots[0].color = new_vec3(1,1,1);
 int i=1;
 for (int branch=0; branch<N_BRANCHES; branch++) {
  int index = rand()%i;
  vec4 p = mb_dots[index].pos;     // position
  vec4 d = random_unit_vec4();     // direction
  vec4 dd = new_vec4(0,0,0,0);     // delta direction
  float l = mb_dots[index].color.x;// lightness
  for (float s=mb_dots[index].size;  s>0 && i<MAXDOTS;  s-=SIZE_INCREMENT) {
   // move p in direction d (distance s)
   p = add_vec4(mul_vec4(d,s), p);
   // move d in direction dd
   d = add_vec4(mul_vec4(dd,direction_deviation_1), d);
   // deviate dd randomly
   dd = add_vec4(mul_vec4(random_unit_vec4(), direction_deviation_2), dd);
   // make dd perpendicular to d again
   dd = sub_vec4(dd, mul_vec4(d, dot_vec4(dd,d)));
   // make dd and d perpendicular to p again
   dd = sub_vec4(dd, mul_vec4(p, dot_vec4(p,dd)));
   d  = sub_vec4( d, mul_vec4(p, dot_vec4(p, d)));
   // normalize both d and p
   dd = normalize_vec4(dd);
   d = normalize_vec4(d);
   p = normalize_vec4(p);
   // update lightness
   l += RND()*LIGHTNESS_DEVIATION; if (l < 0.0f) l = 0.0f;
                              else if (l > 1.0f) l = 1.0f;
   // save the dot
   mb_dots[i].pos = p;
   mb_dots[i].size = s;
   mb_dots[i].color = new_vec3(l,0.6,l);
   if (++i >= nDots) {
    if (mb_dots==dots) nDots=i;
    else return;
   }
  }
 }
 mb_dots[0].size = INITIAL_DOT_SIZE*2.0f;
}




void init()
{
 hypersphere_init();
 points_mode_ball();

 // grow the stuff
 generate();

 // add some stars
 for (int i=0; i<N_STARS; i++) {
  dots[nDots].size = 0.004*pow(RND01()+0.001, -0.33);
  dots[nDots].color = new_vec3(1.0+RND(), 1.0, 1.0+RND());
  dots[nDots].pos = random_unit_vec4();
  nDots++;
 }
 printf("%d dots\n", nDots);


 // set up the VBO
 glGenBuffers(1, &vbo_dots);
 glBindBuffer(GL_ARRAY_BUFFER, vbo_dots);
 glBufferData(GL_ARRAY_BUFFER, sizeof(Dot)*nDots, dots, GL_DYNAMIC_DRAW);
 glEnableClientState(GL_VERTEX_ARRAY);
 glEnableClientState(GL_COLOR_ARRAY);
 glEnableVertexAttribArray(POINT_SIZE_ATTRIB);
 glVertexPointer(4, GL_FLOAT, sizeof(Dot), (void*)offsetof(Dot,pos));
 glColorPointer(3, GL_FLOAT, sizeof(Dot), (void*)offsetof(Dot,color));
 glVertexAttribPointer(POINT_SIZE_ATTRIB, 1, GL_FLOAT, GL_FALSE, sizeof(Dot), (void*)offsetof(Dot,size)); // offsetof() could also be expressed as (void*)((void*)&dots[0].size - (void*)&dots[0]), for example.
}


void draw()
{
 update_view(DEFAULT_MOVE_INTERVAL);

 if (keymap['I'] || keymap['J'] || keymap['K'] || keymap['L']) {
  mb_dots = (Dot*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
  if (!mb_dots) mb_dots = dots;

  direction_deviation_1 += 0.0005f*(!!keymap['I'] - !!keymap['K']);
  direction_deviation_2 += 0.0005f*(!!keymap['J'] - !!keymap['L']); if (direction_deviation_2 < 0.0005f) direction_deviation_2 = 0.0005f;
  generate();

  glUnmapBuffer(GL_ARRAY_BUFFER);
 }

 glDrawArrays(GL_POINTS, 0, nDots);
}


void done()
{
 // done with the VBO
 glDisableVertexAttribArray(POINT_SIZE_ATTRIB);
 glDisableClientState(GL_COLOR_ARRAY);
 glDisableClientState(GL_VERTEX_ARRAY);
 glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
 glDeleteBuffers(1, &vbo_dots);

 hypersphere_done();
}
