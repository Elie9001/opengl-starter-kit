/***
 I made this because the default OpenGL line drawing doesn't allow for perspective.
 Because I wanted something that would put more pixels of thickness wherever the line is closer to the camera.
 Version: 1.0

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


 tl;dr: Feel free to copy this file into any open-source project,
        in accordance with GNU GPL V3.
***/
#pragma once
#include "vectors.h"
vec3  _previous_line_p2;
vec4  _previous_line_p2_4;
vec3  _previous_line_color;
float _previous_line_thickness;
int   _ready_for_line_to = 0;
int   _ready_for_hypersphere_line_to = 0;
void  _tally_line_data(float length, float thickness);



// Draws a line in 3D space (by generating 6 triangles, like an antiprism)
void line(vec3 p1, vec3 p2, vec3 color, float thickness)
{
 vec3 t = sub_vec3(p1,p2);
 vec3 u; vec3 v;
 make_ortho_basis_from_normal_vec3(t, &u, &v);
 u = mul_vec3(u, thickness);
 v = mul_vec3(v, thickness);

 #ifdef TALLY_LINES
 _tally_line_data(length_vec3(t), thickness);
 #endif

 const float COS60 = 0.5;
 const float SIN60 = 0.8660254;
 glBegin(GL_TRIANGLE_STRIP);
 glColor_vec3(color);
 glVertex_vec3(add_vec3(p1, u));                                                //0deg
 glVertex_vec3(add_vec3(p2, add_vec3(mul_vec3(u, COS60), mul_vec3(v, SIN60)))); //60deg
 glVertex_vec3(add_vec3(p1, add_vec3(mul_vec3(u,-COS60), mul_vec3(v, SIN60)))); //120deg
 glVertex_vec3(sub_vec3(p2, u));                                                //180deg
 glVertex_vec3(add_vec3(p1, add_vec3(mul_vec3(u,-COS60), mul_vec3(v,-SIN60)))); //240deg
 glVertex_vec3(add_vec3(p2, add_vec3(mul_vec3(u, COS60), mul_vec3(v,-SIN60)))); //300deg
 glVertex_vec3(add_vec3(p1, u));                                                //360deg
 glVertex_vec3(add_vec3(p2, add_vec3(mul_vec3(u, COS60), mul_vec3(v, SIN60)))); //close
 glEnd();

 _previous_line_p2 = p2;
 _previous_line_color = color;
 _previous_line_thickness = thickness;
 _ready_for_line_to = 1;
}




// Same as above, but in hypersphere space (p1 and p2 should be normalized 4D vectors)
// Caution: long lines will cause depth buffer mistakes
void hypersphere_line(vec4 p1, vec4 p2, vec3 color, float thickness)
{
 // generate a set of vectors (s,t,u,v)
 // each perpendicular to each other
 // s is double the midpoint of the line
 // t is the orientation of the line
 // u and v will be of length 'thickness'
 vec4 s = add_vec4(p1,p2);
 vec4 t = sub_vec4(p1,p2);                   //if(dot_vec4(t,t)>0.01f)return;

 #ifdef TALLY_LINES
 _tally_line_data(length_vec4(t), thickness);
 #endif

 // what we know mathematically: for any 2 vectors 's' and 't', there will be at least 1 vector, orthogonal to both, in the xyz brane (w=0)
 // usually we can find it by projecting s and t onto 3D (xyz) and then crossing them
 // but we have to be prepared for cases when this doesn't work: if 's' has no xyz component, or 't' has no xyz component. note: there's also a third case where the cross wouldn't work, but we don't have to worry about it, because we know 's' and 't' are always orthogonal to each other, assuming p1 and p2 are normalized 4D vectors.
 vec3 s3; s3.x=s.x; s3.y=s.y; s3.z=s.z;
 vec3 t3; t3.x=t.x; t3.y=t.y; t3.z=t.z;
 if      (!(s3.x || s3.y || s3.z)) {
  s3.x = -t3.y;
  s3.y =  t3.x;
  if     (!(s3.x || s3.y)) s3.x = 1;
 }
 else if (!(t3.x || t3.y || t3.z)) {
  t3.x =  s3.y;
  t3.y = -s3.x;
  if     (!(t3.x || t3.y)) t3.x = 1;
 }
 vec3 u3 = cross(s3,t3);
 u3 = mul_vec3(normalize_vec3(u3), thickness);
 vec4 u; u.x = u3.x;
         u.y = u3.y;
         u.z = u3.z;
         u.w = 0;
 vec4 v = hypercross(s,t,u);
 v = mul_vec4(normalize_vec4(v), thickness);


 // generate the vertices
 const float COS60 = 0.5;
 const float SIN60 = 0.8660254;
 glBegin(GL_TRIANGLE_STRIP);
 glColor_vec3(color);
 glVertex_vec4(add_vec4(p1, u));                                                //0deg
 glVertex_vec4(add_vec4(p2, add_vec4(mul_vec4(u, COS60), mul_vec4(v, SIN60)))); //60deg
 glVertex_vec4(add_vec4(p1, add_vec4(mul_vec4(u,-COS60), mul_vec4(v, SIN60)))); //120deg
 glVertex_vec4(sub_vec4(p2, u));                                                //180deg
 glVertex_vec4(add_vec4(p1, add_vec4(mul_vec4(u,-COS60), mul_vec4(v,-SIN60)))); //240deg
 glVertex_vec4(add_vec4(p2, add_vec4(mul_vec4(u, COS60), mul_vec4(v,-SIN60)))); //300deg
 glVertex_vec4(add_vec4(p1, u));                                                //360deg
 glVertex_vec4(add_vec4(p2, add_vec4(mul_vec4(u, COS60), mul_vec4(v, SIN60)))); //close
 glEnd();

 _previous_line_p2_4 = p2;
 _previous_line_color = color;
 _previous_line_thickness = thickness;
 _ready_for_hypersphere_line_to = 1;
}





void             line_to(vec3 p) { if (_ready_for_line_to)                         line(_previous_line_p2,   p, _previous_line_color, _previous_line_thickness); }
void hypersphere_line_to(vec4 p) { if (_ready_for_hypersphere_line_to) hypersphere_line(_previous_line_p2_4, p, _previous_line_color, _previous_line_thickness); }


void             line_mv(vec3 p) { _previous_line_p2   = p; }
void hypersphere_line_mv(vec4 p) { _previous_line_p2_4 = p; }





// The TALLY_LINES feature is useful if you're using line() for CAD designs. You get a running total of the amount of material in the design.
#ifdef TALLY_LINES
 double _total_line_length = 0;
 double _total_line_volume = 0;

 #ifdef LINES_INVENTORY_MAX_SIZE
  vec2 _lines_inventory[LINES_INVENTORY_MAX_SIZE]; // vec2.x is the length; vec2.y is the thickness.
  int  _lines_inventory_size = 0;
 #endif

 void _tally_line_data(float length, float thickness) {
  _total_line_length += length;
  _total_line_volume += M_PI*thickness*thickness*length; // volume assuming the line is supposed to represent a solid cylinder, with 'thickness' as the radius

  #ifdef LINES_INVENTORY_MAX_SIZE
   if (_lines_inventory_size < LINES_INVENTORY_MAX_SIZE) {
    _lines_inventory[_lines_inventory_size].x = length;
    _lines_inventory[_lines_inventory_size].y = thickness;
    _lines_inventory_size++;
   }
  #endif
 }
#endif // TALLY_LINES




/*
Future plans for this 'line' header:
- make a version that uses VBOs instead of triangle strips
  - the kind that involves both glDrawArrays and glDrawElements
  - first gotta set a standard for vertex & index arrays/structs?
- make a version that works with GLM
  - or instead, keep this file the same and modify vectors.h:
    - vectors.h: put an #ifdef: if GLM is already included:
      - don't define structs (vec2, vec3, vec4)
      - put "using namespace glm;"
      - functions become macros; for example: #define add_vec3(a,b) (a+b)
All of this is assuming I ever get around to it haha
*/