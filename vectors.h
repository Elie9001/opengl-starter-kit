/***
 Basic vector operations
 This is not meant to be full-featured. Just featured enough.

 Author: Elie Goldman Smith
 This code is uncopyrightable because it's essentially just standard math.
 Therefore it is in the public domain.

 Note: If you need something more heavy duty, I'd recommend using the GLM library instead (requires C++)
***/
#pragma once


typedef struct { float x; float y; } vec2;
typedef struct { float x; float y; float z; } vec3;
typedef struct { float x; float y; float z; float w; } vec4;


static inline vec2 new_vec2(float x, float y) {
 vec2 r;
 r.x = x;
 r.y = y;
 return r;
}

static inline vec3 new_vec3(float x, float y, float z) {
 vec3 r;
 r.x = x;
 r.y = y;
 r.z = z;
 return r;
}

static inline vec4 new_vec4(float x, float y, float z, float w) {
 vec4 r;
 r.x = x;
 r.y = y;
 r.z = z;
 r.w = w;
 return r;
}



static inline vec2 add_vec2(vec2 a, vec2 b) {
 vec2 r;
 r.x = a.x + b.x;
 r.y = a.y + b.y;
 return r;
}

static inline vec3 add_vec3(vec3 a, vec3 b) {
 vec3 r;
 r.x = a.x + b.x;
 r.y = a.y + b.y;
 r.z = a.z + b.z;
 return r;
}

static inline vec4 add_vec4(vec4 a, vec4 b) {
 vec4 r;
 r.x = a.x + b.x;
 r.y = a.y + b.y;
 r.z = a.z + b.z;
 r.w = a.w + b.w;
 return r;
}



static inline vec2 sub_vec2(vec2 a, vec2 b) {
 vec2 r;
 r.x = a.x - b.x;
 r.y = a.y - b.y;
 return r;
}

static inline vec3 sub_vec3(vec3 a, vec3 b) {
 vec3 r;
 r.x = a.x - b.x;
 r.y = a.y - b.y;
 r.z = a.z - b.z;
 return r;
}

static inline vec4 sub_vec4(vec4 a, vec4 b) {
 vec4 r;
 r.x = a.x - b.x;
 r.y = a.y - b.y;
 r.z = a.z - b.z;
 r.w = a.w - b.w;
 return r;
}



static inline vec2 mul_vec2(vec2 a, float b) {
 vec2 r;
 r.x = a.x * b;
 r.y = a.y * b;
 return r;
}

static inline vec3 mul_vec3(vec3 a, float b) {
 vec3 r;
 r.x = a.x * b;
 r.y = a.y * b;
 r.z = a.z * b;
 return r;
}

static inline vec4 mul_vec4(vec4 a, float b) {
 vec4 r;
 r.x = a.x * b;
 r.y = a.y * b;
 r.z = a.z * b;
 r.w = a.w * b;
 return r;
}


static inline float dot_vec2(vec2 a, vec2 b) { return a.x*b.x + a.y*b.y; }
static inline float dot_vec3(vec3 a, vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static inline float dot_vec4(vec4 a, vec4 b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }


static inline vec3 cross(vec3 a, vec3 b) {
 vec3 r;
 r.x = a.y*b.z - a.z*b.y;
 r.y = a.z*b.x - a.x*b.z;
 r.z = a.x*b.y - a.y*b.x;
 return r;
}

static inline vec4 hypercross(vec4 a, vec4 b, vec4 c) { // also called 'ternary product': given 3 vectors, make a 4th that is orthogonal to all 3
 vec4 r;
 r.x = a.y*b.w*c.z + a.w*b.z*c.y + a.z*b.y*c.w - a.w*b.y*c.z - a.y*b.z*c.w - a.z*b.w*c.y;
 r.y = a.w*b.x*c.z + a.x*b.z*c.w + a.z*b.w*c.x - a.x*b.w*c.z - a.w*b.z*c.x - a.z*b.x*c.w;
 r.z = a.x*b.w*c.y + a.w*b.y*c.x + a.y*b.x*c.w - a.w*b.x*c.y - a.x*b.y*c.w - a.y*b.w*c.x;
 r.w = a.x*b.y*c.z + a.y*b.z*c.x + a.z*b.x*c.y - a.y*b.x*c.z - a.x*b.z*c.y - a.z*b.y*c.x;
 return r;
}



#ifndef isSubnormalF
#define isSubnormalF(f)  ((*(unsigned*)&(f) & 0x7F800000) == 0)  // returns true if 'f' is either zero or so close to zero that 'f' doesn't behave normally. Note: 'f' must be a 32-bit 'float' type.
#endif

static inline vec2 normalize_vec2(vec2 v) {
 float d = dot_vec2(v,v);
 if (isSubnormalF(d)) d = 0.0f;
 else                 d = 1.0f/sqrtf(d);
 return mul_vec2(v,d);
}

static inline vec3 normalize_vec3(vec3 v) {
 float d = dot_vec3(v,v);
 if (isSubnormalF(d)) d = 0.0f;
 else                 d = 1.0f/sqrtf(d);
 return mul_vec3(v,d);
}

static inline vec4 normalize_vec4(vec4 v) {
 float d = dot_vec4(v,v);
 if (isSubnormalF(d)) d = 0.0f;
 else                 d = 1.0f/sqrtf(d);
 return mul_vec4(v,d);
}


static inline float length_vec2(vec2 v) { return sqrtf(dot_vec2(v,v)); }
static inline float length_vec3(vec3 v) { return sqrtf(dot_vec3(v,v)); }
static inline float length_vec4(vec4 v) { return sqrtf(dot_vec4(v,v)); }





static inline vec2 projection_vec2(vec2 v, vec2 dir) {
 dir = normalize_vec2(dir);
 return mul_vec2(dir, dot_vec2(v,dir));
}
static inline vec3 projection_vec3(vec3 v, vec3 dir) {
 dir = normalize_vec3(dir);
 return mul_vec3(dir, dot_vec3(v,dir));
}
static inline vec4 projection_vec4(vec4 v, vec4 dir) {
 dir = normalize_vec4(dir);
 return mul_vec4(dir, dot_vec4(v,dir));
}

static inline vec2 make_ortho_vec2(vec2 v, vec2 ref) { return sub_vec2(v, projection_vec2(v,ref)); } //
static inline vec3 make_ortho_vec3(vec3 v, vec3 ref) { return sub_vec3(v, projection_vec3(v,ref)); } //
static inline vec4 make_ortho_vec4(vec4 v, vec4 ref) { return sub_vec4(v, projection_vec4(v,ref)); } // gram-schmidt process


// This takes in a normal vector, and spits out two vectors u and v, such that
// - both are orthogonal to the normal
// - both are orthogonal to each other
// - both are unit vectors
// - u cross v = normalize(normal)
void make_ortho_basis_from_normal_vec3(vec3 normal, vec3 *return_u, vec3 *return_v) {
 vec3 u; u.x = -normal.y;
         u.y =  normal.x;
         u.z =  0.0f;
 float d = dot_vec3(u,u); if (isSubnormalF(d)) u.x = 1.0f;   // special case
                          else u = mul_vec3(u, 1.0f/sqrtf(d)); // usual case
 *return_u = u;
 *return_v = normalize_vec3(cross(normal,u));
}



vec2 random_unit_vec2() {
 vec2 r;
 float d;
 do {
  r.x = rand()*(2.0f/RAND_MAX)-1.0f;
  r.y = rand()*(2.0f/RAND_MAX)-1.0f;
  d = dot_vec2(r,r);
 } while (d > 1.0f || isSubnormalF(d));
 d = 1.0f/sqrtf(d);
 r = mul_vec2(r,d);
 return r;
}

vec3 random_unit_vec3() {
 vec3 r;
 float d;
 do {
  r.x = rand()*(2.0f/RAND_MAX)-1.0f;
  r.y = rand()*(2.0f/RAND_MAX)-1.0f;
  r.z = rand()*(2.0f/RAND_MAX)-1.0f;
  d = dot_vec3(r,r);
 } while (d > 1.0f || isSubnormalF(d));
 d = 1.0f/sqrtf(d);
 r = mul_vec3(r,d);
 return r;
}

vec4 random_unit_vec4() {
 vec4 r;
 float d;
 do {
  r.x = rand()*(2.0f/RAND_MAX)-1.0f;
  r.y = rand()*(2.0f/RAND_MAX)-1.0f;
  r.z = rand()*(2.0f/RAND_MAX)-1.0f;
  r.w = rand()*(2.0f/RAND_MAX)-1.0f;
  d = dot_vec4(r,r);
 } while (d > 1.0f || isSubnormalF(d));
 d = 1.0f/sqrtf(d);
 r = mul_vec4(r,d);
 return r;
}




#if defined(__gl_h_) || defined(_GL_H) || defined(GL_POINTS)  // OpenGL
static inline void glVertex_vec2(vec2 v) { glVertex2f(v.x, v.y); }
static inline void glVertex_vec3(vec3 v) { glVertex3f(v.x, v.y, v.z); }
static inline void glVertex_vec4(vec4 v) { glVertex4f(v.x, v.y, v.z, v.w); }
static inline void glColor_vec4 (vec4 v) { glColor4f(v.x, v.y, v.z, v.w); }
static inline void glColor_vec3 (vec3 v) { glColor3f(v.x, v.y, v.z); }
#endif
