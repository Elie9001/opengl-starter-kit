/***
 A simulation of galaxies, stars, planets and moons.
 This should help demonstrate a few things:
  * How to use glMapBuffer() to alter the locations of points (vertices)
  * Building a random world with a somewhat fractal/hierarchical structure
  * Gravity-like attraction between objects, without using too much computing power. Instead of applying forces between every object and every other object [O(N^2)], we use a hierarchy to reduce the number of objects that interact with each other [O(N log N)]. For example, planets only get pulled by their local star. Overall this makes a good approximation as long as solar systems don't collide.

 To compile this:  gcc stars2.c -o stars2 -ffast-math -O -lGL -lglut -lm

 Author: Elie Goldman Smith
 This code is too unfinished to copyright, so it is in the public domain.
***/

#include "../hypersphere.h"
#include "../line.h"

#define BIG_G_CONSTANT 0.000000001
#define MASS_TO_SIZE 0.02 // inverse density

#define MAXDOTS 262144
typedef struct {
 vec4 pos;
 vec4 vel;
 vec3 color;
 float size;
 float mass;
} Dot;
Dot dots[MAXDOTS];
int nDots=0;

#define MAXRELATIONS 262144
typedef struct {
 int index1;
 int index2;
} Relation;
Relation relations[MAXRELATIONS];
int nRelations=0;

int nGalaxies=18; // XXX: #define?
GLuint vbo_dots; // vertex buffer object handle




/* This function can build a galaxy, or add planets to a star, or add moons to a planet.
 * It writes to the global arrays: dots[] and relations[]
 * Params:
    int center: the dots[] index of the center object
    int n: the number of objects in the swarm (not counting the centre object)
    float radius: the approximate radius of the swarm.................... relative to the center object's radius
    float mass:   the approximate mass of each object within the swarm... relative to the center object's mass
    vec3 (*colorFunc)() : pointer to a function that returns a color - called for every object in the swarm
 */
void create_swarm(int center, int n, float radius, float mass, vec3 (*colorFunc)()) {
 vec4 spin = normalize_vec4(make_ortho_vec4(random_unit_vec4(), dots[center].pos)); // axis
 float orbit = sqrtf(2.0f*dots[center].mass*BIG_G_CONSTANT); // coefficient to make a good orbit
 float radiuz = radius*dots[center].size; // radius, not relative
 float mazz = mass*0.5f*dots[center].mass;// mass, not relative
 int start = nDots;
 for (int i=0; i<n; i++) {
  if (nDots >= MAXDOTS || nRelations > MAXRELATIONS) return;
  float dist = radiuz*(1.0f + RND()*0.4f);
  float speed = orbit / sqrtf(dist);
  vec4 rel = normalize_vec4(make_ortho_vec4(random_unit_vec4(), spin));
  dots[nDots].pos = add_vec4(dots[center].pos, mul_vec4(rel,dist));
  dots[nDots].vel = add_vec4(dots[center].vel, mul_vec4(hypercross(rel, spin, dots[center].pos), speed));
  dots[nDots].mass = mazz/(RND01()+0.01f);
  dots[nDots].size = MASS_TO_SIZE*pow(dots[nDots].mass, 0.33f);
  dots[nDots].color = colorFunc ? colorFunc() : new_vec3(1,1,1);
  relations[nRelations].index1 = center;
  relations[nRelations].index2 = nDots;
  nRelations++;
  nDots++;
 }/*
 // gravity between objects within the swarm (requires more CPU power)
 for (int i=start;i<nDots; i++) {
  for (int j=i+1; j<nDots; j++) {
   relations[nRelations].index1 = i;
   relations[nRelations].index2 = j;
   nRelations++;
  }
 }*/
}
vec3 color_planet() {
 int channel = rand()%3;
 switch (channel) {
  case 0: return new_vec3(1.0f, RND01(), RND01());
  case 1: return new_vec3(RND01(), 1.0f, RND01());
  case 2: return new_vec3(RND01(), RND01(), 1.0f);
 }
}
vec3 color_star() {
 if (rand()%2) return new_vec3(1.0f, 1.0f, RND01()*0.5f+0.5f);
 else          return new_vec3(RND01()*0.5f+0.5f, 1.0f, 1.0f);
}



void init()
{
 hypersphere_init();
 points_mode_soft();

 // supermass at the centre of each galaxy
 for (int i=0; i<nGalaxies; i++) {
  dots[nDots].pos = i ? random_unit_vec4() : new_vec4(0, 0, M_SQRT1_2, M_SQRT1_2);
  dots[nDots].vel = new_vec4(0,0,0,0);
  dots[nDots].mass = 1.0/(RND01()+0.01);
  dots[nDots].size = 0.4*MASS_TO_SIZE*pow(dots[nDots].mass, 0.33);
  dots[nDots].color = new_vec3(1,1,1);
  nDots++;
 }/*
 // gravity between supermasses
 for (int i=0;    i<nDots; i++) {
  for (int j=i+1; j<nDots; j++) {
   relations[nRelations].index1 = i;
   relations[nRelations].index2 = j;
   nRelations++;
  }
 }*/
 int start_galaxies=0;
 for (int i=0; i<nGalaxies; i++) {
  int nStars = 16.0/(RND01()+0.1);
  int start_stars = nDots;
  create_swarm(i+start_galaxies, nStars, 16.0, 0.001, color_star);
  for (int j=0; j<nStars; j++) {
   int nPlanets = rand()%12;
   int start_planets = nDots;
   create_swarm(j+start_stars, nPlanets, 4.0, 0.01, color_planet);
   for (int k=0; k<nPlanets; k++) {
    relations[nRelations].index1 = i+start_galaxies;
    relations[nRelations].index2 = k+start_planets;
    nRelations++;
    int start_moons = nDots;
    int nMoons = rand()%4;
    create_swarm(k+start_planets, nMoons, 4.0, 0.1, NULL);
    for (int l=0; l<nMoons; l++) {
     relations[nRelations].index1 = i+start_galaxies;
     relations[nRelations].index2 = l+start_moons;
     nRelations++;
     relations[nRelations].index1 = j+start_stars;
     relations[nRelations].index2 = l+start_moons;
     nRelations++;
    }
   }
  }
 }
 printf("%d dots, %d relations\n", nDots, nRelations);


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
 update_view(0.0005);
 Dot* mb = (Dot*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE); // mapped buffer
 if (!mb) mb = dots;

 // apply a force of gravity between any dots that have a 'relation'
 for (int i=0; i<nRelations; i++) {
  int i1 = relations[i].index1;
  int i2 = relations[i].index2;
  vec4 diff = sub_vec4(mb[i1].pos, mb[i2].pos);
  float invDist = 1.0f / sqrtf(dot_vec4(diff,diff) + 1e-18); // inverse distance; technically faster to calculate than distance, because of the floating-point 'inverse square root' operation. note: the 1e-18 is there to avoid division by zero
  float mult = invDist * invDist * invDist * BIG_G_CONSTANT; // invDist^2 to make the inverse-square law; another invDist to normalize the diff vector
  if (mult > 0.02f/(mb[i1].mass+mb[i2].mass)) continue; // extra clause to avoid gravitational singularities. It basically removes any gravitational forces between bodies that are too close to each other. As a side effect, some bodies get stuck in this 'no gravity zone' and form a sort of bond with each other.
  mb[i1].vel = sub_vec4(mb[i1].vel, mul_vec4(diff, mult*mb[i2].mass)); // apply the accelleration
  mb[i2].vel = add_vec4(mb[i2].vel, mul_vec4(diff, mult*mb[i1].mass)); // apply the accelleration
 }
 // update positions
 for (int i=0; i<nDots; i++) {
  // motion (inertia) (and normalize to maintain hypersphere rules)
  mb[i].pos = normalize_vec4(add_vec4(mb[i].pos, mb[i].vel));
  // orthogonalize velocity (to maintain hypersphere rules)
  mb[i].vel = sub_vec4(mb[i].vel, mul_vec4(mb[i].pos, dot_vec4(mb[i].vel, mb[i].pos)));
 }

 glUnmapBuffer(GL_ARRAY_BUFFER);
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
