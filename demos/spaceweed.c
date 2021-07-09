/***
 Generates an organic-looking thing in space.
 To compile this: gcc spaceweed.c -o spaceweed -ffast-math -O -lGL -lglut -lm

 Author: Elie Goldman Smith
 This code is too simple to copyright, so it is in the public domain.
***/
#define VIEWPORT_SCALE 1.25
#define POINT_SIZE_LIMIT_STRING "0.5"
#include "../hypersphere.h"
#include "../vectors.h"

#define MAXDOTS 65536
#define INITIAL_DOT_SIZE 0.02
#define SIZE_INCREMENT 0.0001
#define N_BRANCHES 200
#define DIRECTION_DEVIATION 0.2
#define LIGHTNESS_DEVIATION 0.1
#define N_STARS 5000

typedef struct {
 vec4 pos;
 vec3 color;
 float size;
} Dot;

Dot dots[MAXDOTS];
int nDots=0;

GLuint dl_dots; // display-list

void init()
{
 dots[0].pos = new_vec4(0, sqrt(0.01), sqrt(0.99), 0);
 dots[0].size = INITIAL_DOT_SIZE;
 dots[0].color = new_vec3(1,1,1);
 nDots=1;

 for (int i=0; i<N_BRANCHES; i++) {
  int index = rand()%nDots;
  vec4 p = dots[index].pos;      // position
  vec4 d = random_unit_vec4();   // direction
  float l = dots[index].color.x; // lightness
  for (float s=dots[index].size;  s>0 && nDots<MAXDOTS;  s-=SIZE_INCREMENT) {
   // move p in direction d (distance s)
   p = add_vec4(mul_vec4(d,s), p);
   // deviate d randomly
   d = add_vec4(mul_vec4(random_unit_vec4(), DIRECTION_DEVIATION), d);
   // make d perpendicular to p again
   d = sub_vec4(d, mul_vec4(p, dot_vec4(p,d)));
   // normalize both d and p
   d = normalize_vec4(d);
   p = normalize_vec4(p);
   // update lightness
   l += RND()*LIGHTNESS_DEVIATION; if (l < 0.0f) l = 0.0f;
                              else if (l > 1.0f) l = 1.0f;
   // save the dot
   dots[nDots].pos = p;
   dots[nDots].size = s;
   dots[nDots].color = new_vec3(l,0.8,l);
   nDots++;
  }
 }
 dots[0].size *= 2.0f;
 printf("%d dots\n", nDots);

 hypersphere_init();
 points_mode_ball();

 dl_dots = glGenLists(1);
 glNewList(dl_dots, GL_COMPILE);
 glBegin(GL_POINTS);
 for (int i=0; i<nDots; i++) {
  point_size(dots[i].size);
  glColor_vec3(dots[i].color);
  glVertex_vec4(dots[i].pos);
 }
 for (int i=0; i<N_STARS; i++) {
  point_size(0.004*pow(RND01()+0.001, -0.33));
  glColor3f(1.0+RND(), 1.0, 1.0+RND());
  glVertex_vec4(random_unit_vec4());
 }
 glEnd();
 glEndList();
}

void draw()
{
 update_view(DEFAULT_MOVE_INTERVAL);
 glCallList(dl_dots);
}

void done()
{
 glDeleteLists(dl_dots, 1);
 hypersphere_done();
}
