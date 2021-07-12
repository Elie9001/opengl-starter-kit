/***
 Elie's OpenGL Starter Kit - hypersphere.h

 This is a "world viewer" header.
 It controls the OpenGL model/view/projection matrices, by listening to user input from the keyboard and mouse.
 You fly around in a universe that feels infinite but actually wraps in every direction. This is actually the surface of a 4D hypersphere.
 Version: 1.0
 
 For an example of how to use this header, see: demos/stars.c

 --
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

#include "fullscreen_main.h"
/**
 * Since fullscreen_main.h implements main(),
 * you use the following framework for your code:
             void init();
             void draw();
             void done();
 * For more details, read the comments in fullscreen_main.h
**/

// Call this function in your init() code:
void hypersphere_init();

// Call this function in your draw() code:
void update_view(float move_interval);  // This takes care of keyboard & mouse inputs, moves the camera/matrix accordingly, and clears the frame buffer. Note: move_interval should be a very small number. When in doubt, use DEFAULT_MOVE_INTERVAL. Basically it's your accelleration in radians per frame per frame.

// Call this function in your done() code:
void hypersphere_done();


// Functions you can use in your draw() or init():
void points_mode_soft(); // for drawing points as orbs of light.
void points_mode_hole(); // for drawing points with a hole in middle.
void points_mode_flat(); // for drawing points as circles. Uses depth buffer.
void points_mode_ball(); // for drawing points as spheres. Uses depth buffer.
void default_mode();     // for when you're done drawing points, and want to go back to triangles/quads etc.
void point_size(float size); // set the "physical" size of the next point you draw.  This is the same thing as glVertexAttrib1fv(POINT_SIZE_ATTRIB, &size);


/** Notes:
 *
 * All your vertices should satisfy the equation w^2 + x^2 + y^2 + z^2 = 1.  That's how this universe works!
 *
 * update_view() keyboard controls: W,A,S,D, and arrow keys to move around
 *                  mouse controls: hold left click to look around.
 *
 *
 *
 * Compiler flags needed when you use this header file:
 *      -ffast-math -O -lGL -lglut -lm
 *
 * Options that can (maybe) speed up rendering:
 *      #define FAST_SPHERE_NO_SQRT           // spheres are drawn slightly faster, but the z-fighting is worse.
 *      #define DONT_ANTIALIAS_OPAQUE_POINTS  // performance gains are small here, because antialiasing is based on alpha blending, not oversampling.
 *      #define POINT_SIZE_LIMIT_STRING "0.5" // point size won't exceed 0.5x the width of the screen. Yes you need to put the number in quotes, because it's being used in the shader's code.   Also, you can replace "0.5" with any number between "0.0" and "1.0".
**/


#define POINT_SIZE_ATTRIB 7
#define DEFAULT_POINT_SIZE 0.01f






#ifndef DONT_ANTIALIAS_OPAQUE_POINTS
#define ALPHA_BLEND_OPAQUE_EDGES
#endif



GLuint v_shader_tri;
GLchar * v_src_tri =
  "void main(void) {"
  " gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
  " gl_FrontColor = gl_Color * (gl_Position.z+1.0);"
  " gl_Position.z *= gl_Position.w; /* undoing the perspective divide, so that the z clipping will be calculated properly */"
  "}";



GLuint v_shader_pts;
GLchar * v_src_pts =
  "attribute float RealSize;"
  "uniform float ScreenSize;"
  "varying float spherePop;"
  #ifdef ALPHA_BLEND_OPAQUE_EDGES
  "varying float aaFactor;"
  #endif
  "void main(void) {"
  " gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
  " float size = RealSize*ScreenSize/gl_Position.w;"
  " gl_FrontColor.a = (gl_Position.z+1.0) * min(size*0.5, 1.0);  /* alpha actually gets applied in the fragment shader, not in blending */"
  " gl_FrontColor.rgb = gl_Color.rgb;"
  #ifdef POINT_SIZE_LIMIT_STRING
  " gl_PointSize = clamp(size, 2.0, ScreenSize*"POINT_SIZE_LIMIT_STRING");"
  #else
  " gl_PointSize = max(size, 2.0);"
  #endif
  #ifdef ALPHA_BLEND_OPAQUE_EDGES
  " aaFactor = gl_PointSize + 6.0;"
  #endif
  " spherePop = RealSize*(gl_Position.z + gl_Position.w*2.0);"
  " gl_Position.z *= gl_Position.w; /* undoing the perspective divide, so that the z clipping will be calculated properly */"
  "}";



GLuint f_shader_pts_soft;
GLchar * f_src_pts_soft =
  "void main(void) {"
  " vec2 v = vec2(gl_TexCoord[0]) - vec2(0.5);"
  " float shade = max(0.0, 1.5 - 6.0*dot(v,v));"
  " gl_FragColor.rgb = gl_Color.rgb;"
  " gl_FragColor.a = gl_Color.a * shade;"
  "}";



GLuint f_shader_pts_hole;
GLchar * f_src_pts_hole =
  "void main(void) {"
  " vec2 v = vec2(gl_TexCoord[0]) - vec2(0.5);"
  " float shade = dot(v,v);"
  "       shade = max(0.0, shade*(0.25-shade)*80.0);"
  " gl_FragColor.rgb = gl_Color.rgb;"
  " gl_FragColor.a = gl_Color.a * shade;"
  "}";



GLuint f_shader_pts_flat;
GLchar * f_src_pts_flat =
  #ifdef ALPHA_BLEND_OPAQUE_EDGES
  "varying float aaFactor;"
  #endif
  "void main(void) {"
  " vec2 v = vec2(gl_TexCoord[0]) - vec2(0.5);"
  " float d = dot(v,v);"
  " if (d > 0.25) discard;"
  #ifdef WHITE_BACKGROUND
  " gl_FragColor.rgb = mix(vec3(1.0), gl_Color.rgb, gl_Color.a);"
  #else // black background
  " gl_FragColor.rgb =                gl_Color.rgb * gl_Color.a;"
  #endif
  #ifdef ALPHA_BLEND_OPAQUE_EDGES
  " gl_FragColor.a = min(1.0, (0.25-d) * aaFactor);"
  " gl_FragDepth = gl_FragCoord.z * gl_FragColor.a;"
  #endif
  "}";



GLuint f_shader_pts_ball;
GLchar * f_src_pts_ball =
  #ifdef ALPHA_BLEND_OPAQUE_EDGES
  "varying float aaFactor;"
  #endif
  "varying float spherePop;"
  "void main(void) {"
  " vec2 v = vec2(gl_TexCoord[0]) - vec2(0.5);"
  " float d = dot(v,v);"
  " if (d > 0.25) discard;"
  #ifdef FAST_SPHERE_NO_SQRT
    " float c = d * 2.0;"
    " gl_FragDepth = gl_FragCoord.z + spherePop * (0.5 - c);"
    #ifdef WHITE_BACKGROUND
    " gl_FragColor.rgb = mix(vec3(1.0),  gl_Color.rgb - c,   gl_Color.a);"
    #else
    " gl_FragColor.rgb =                (gl_Color.rgb + c) * gl_Color.a;"
    #endif
  #else
    " float z = sqrt(0.25 - d);"
    " gl_FragDepth = gl_FragCoord.z + spherePop * z;"
    #ifdef WHITE_BACKGROUND
    " gl_FragColor.rgb = mix(vec3(1.0),  gl_Color.rgb-0.5+z,   gl_Color.a);"
    #else
    " gl_FragColor.rgb =                (gl_Color.rgb+0.5-z) * gl_Color.a;"
    #endif
  #endif
  #ifdef ALPHA_BLEND_OPAQUE_EDGES
  " gl_FragColor.a = min(1.0, (0.25-d) * aaFactor);"
  " gl_FragDepth *= gl_FragColor.a;"
  #endif
  "}";



GLuint program_pts_soft;
GLuint program_pts_hole;
GLuint program_pts_flat;
GLuint program_pts_ball;
GLuint program_default;





void points_mode_soft()
{
 glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
 glEnable(GL_POINT_SPRITE_ARB);
 glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

 glEnable(GL_BLEND);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE);
 #ifdef WHITE_BACKGROUND
 glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
 #else
 glBlendEquation(GL_FUNC_ADD);
 #endif

 glDepthMask(GL_FALSE);
 #ifdef DEPTH_TEST_ALL_POINTS
 glEnable(GL_DEPTH_TEST);
 #else
 glDisable(GL_DEPTH_TEST);
 #endif

 glUseProgram(program_pts_soft);
 float screenSize = _screen_size;  glUniform1fv(glGetUniformLocation(
              program_pts_soft,  "ScreenSize"), 1, &screenSize);
}



void points_mode_hole()
{
 glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
 glEnable(GL_POINT_SPRITE_ARB);
 glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

 glEnable(GL_BLEND);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE);
 #ifdef WHITE_BACKGROUND
 glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
 #else
 glBlendEquation(GL_FUNC_ADD);
 #endif

 glDepthMask(GL_FALSE);
 #ifdef DEPTH_TEST_ALL_POINTS
 glEnable(GL_DEPTH_TEST);
 #else
 glDisable(GL_DEPTH_TEST);
 #endif

 glUseProgram(program_pts_hole);
 float screenSize = _screen_size;  glUniform1fv(glGetUniformLocation(
              program_pts_hole,  "ScreenSize"), 1, &screenSize);
}



void points_mode_flat()
{
 glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
 glEnable(GL_POINT_SPRITE_ARB);
 glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

 #ifdef ALPHA_BLEND_OPAQUE_EDGES
 glEnable(GL_BLEND);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 glBlendEquation(GL_FUNC_ADD);
 #else
 glDisable(GL_BLEND);
 #endif

 glEnable(GL_DEPTH_TEST);
 glDepthMask(GL_TRUE);

 glUseProgram(program_pts_flat);
 float screenSize = _screen_size;  glUniform1fv(glGetUniformLocation(
              program_pts_flat,  "ScreenSize"), 1, &screenSize);
}



void points_mode_ball()
{
 glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
 glEnable(GL_POINT_SPRITE_ARB);
 glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

 #ifdef ALPHA_BLEND_OPAQUE_EDGES
 glEnable(GL_BLEND);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 glBlendEquation(GL_FUNC_ADD);
 #else
 glDisable(GL_BLEND);
 #endif

 glEnable(GL_DEPTH_TEST);
 glDepthMask(GL_TRUE);

 glUseProgram(program_pts_ball);
 float screenSize = _screen_size;  glUniform1fv(glGetUniformLocation(
              program_pts_ball,  "ScreenSize"), 1, &screenSize);
}



void default_mode() {
 glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
 glDisable(GL_POINT_SPRITE_ARB);
 glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_FALSE);

 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 glBlendEquation(GL_FUNC_ADD);
 glDisable(GL_BLEND);

 glEnable(GL_DEPTH_TEST);
 glDepthMask(GL_TRUE);

 glUseProgram(program_default);
}



void point_size(float size) {
 glVertexAttrib1fv(POINT_SIZE_ATTRIB, &size);
}





#define SHADER_LOG_MAX_LENGTH 4096

void hypersphere_init()
{
 GLint isCompiled = 0;
 GLint isLinked = 0;
 GLint length = 0;
 static GLchar errorLog[SHADER_LOG_MAX_LENGTH];


 v_shader_tri = glCreateShader(GL_VERTEX_SHADER);
 glShaderSource  (   v_shader_tri, 1, &v_src_tri, NULL);
 glCompileShader (   v_shader_tri  );
 glGetShaderiv   (   v_shader_tri, GL_COMPILE_STATUS, &isCompiled);
 if(!isCompiled)
 {
  glGetShaderInfoLog(v_shader_tri, SHADER_LOG_MAX_LENGTH, &length, &errorLog[0]);
  printf         (  "v_shader_tri  compilation failed:\n\n%s\n\n", errorLog);
  glDeleteShader (   v_shader_tri  );
 }


 v_shader_pts = glCreateShader(GL_VERTEX_SHADER);
 glShaderSource  (   v_shader_pts, 1, &v_src_pts, NULL);
 glCompileShader (   v_shader_pts  );
 glGetShaderiv   (   v_shader_pts, GL_COMPILE_STATUS, &isCompiled);
 if(!isCompiled)
 {
  glGetShaderInfoLog(v_shader_pts, SHADER_LOG_MAX_LENGTH, &length, &errorLog[0]);
  printf         (  "v_shader_pts  compilation failed:\n\n%s\n\n", errorLog);
  glDeleteShader (   v_shader_pts  );
 }


 f_shader_pts_soft = glCreateShader(GL_FRAGMENT_SHADER);
 glShaderSource  (   f_shader_pts_soft, 1, &f_src_pts_soft, NULL);
 glCompileShader (   f_shader_pts_soft  );
 glGetShaderiv   (   f_shader_pts_soft, GL_COMPILE_STATUS, &isCompiled);
 if(!isCompiled)
 {
  glGetShaderInfoLog(f_shader_pts_soft, SHADER_LOG_MAX_LENGTH, &length, &errorLog[0]);
  printf         (  "f_shader_pts_soft  compilation failed:\n\n%s\n\n", errorLog);
  glDeleteShader (   f_shader_pts_soft  );
 }


 f_shader_pts_hole = glCreateShader(GL_FRAGMENT_SHADER);
 glShaderSource  (   f_shader_pts_hole, 1, &f_src_pts_hole, NULL);
 glCompileShader (   f_shader_pts_hole  );
 glGetShaderiv   (   f_shader_pts_hole, GL_COMPILE_STATUS, &isCompiled);
 if(!isCompiled)
 {
  glGetShaderInfoLog(f_shader_pts_hole, SHADER_LOG_MAX_LENGTH, &length, &errorLog[0]);
  printf         (  "f_shader_pts_hole  compilation failed:\n\n%s\n\n", errorLog);
  glDeleteShader (   f_shader_pts_hole  );
 }

 f_shader_pts_flat = glCreateShader(GL_FRAGMENT_SHADER);
 glShaderSource  (   f_shader_pts_flat, 1, &f_src_pts_flat, NULL);
 glCompileShader (   f_shader_pts_flat  );
 glGetShaderiv   (   f_shader_pts_flat, GL_COMPILE_STATUS, &isCompiled);
 if(!isCompiled)
 {
  glGetShaderInfoLog(f_shader_pts_flat, SHADER_LOG_MAX_LENGTH, &length, &errorLog[0]);
  printf         (  "f_shader_pts_flat  compilation failed:\n\n%s\n\n", errorLog);
  glDeleteShader (   f_shader_pts_flat  );
 }


 f_shader_pts_ball = glCreateShader(GL_FRAGMENT_SHADER);
 glShaderSource  (   f_shader_pts_ball, 1, &f_src_pts_ball, NULL);
 glCompileShader (   f_shader_pts_ball  );
 glGetShaderiv   (   f_shader_pts_ball, GL_COMPILE_STATUS, &isCompiled);
 if(!isCompiled)
 {
  glGetShaderInfoLog(f_shader_pts_ball, SHADER_LOG_MAX_LENGTH, &length, &errorLog[0]);
  printf         (  "f_shader_pts_ball  compilation failed:\n\n%s\n\n", errorLog);
  glDeleteShader (   f_shader_pts_ball  );
 }

 /************/       program_pts_soft = glCreateProgram();
 glAttachShader      (program_pts_soft, f_shader_pts_soft);
 glAttachShader      (program_pts_soft, v_shader_pts);
 glBindAttribLocation(program_pts_soft, POINT_SIZE_ATTRIB, "RealSize");
 glLinkProgram       (program_pts_soft);
 glGetProgramiv      (program_pts_soft, GL_LINK_STATUS, &isLinked);
 if(isLinked){
  glGetProgramiv(     program_pts_soft, GL_PROGRAM_BINARY_LENGTH, &length);
  #ifdef VERBOSE
  printf(            "program_pts_soft  binary size: %d bytes\n", length);
  #endif
 }else{
  glGetProgramInfoLog(program_pts_soft, SHADER_LOG_MAX_LENGTH, &length, &errorLog[0]);
  printf (           "program_pts_soft  linking failed:\n\n%s\n\n",errorLog);
  glDeleteProgram(    program_pts_soft  );
 }

 /************/       program_pts_hole = glCreateProgram();
 glAttachShader      (program_pts_hole, f_shader_pts_hole);
 glAttachShader      (program_pts_hole, v_shader_pts);
 glBindAttribLocation(program_pts_hole, POINT_SIZE_ATTRIB, "RealSize");
 glLinkProgram       (program_pts_hole);
 glGetProgramiv      (program_pts_hole, GL_LINK_STATUS, &isLinked);
 if(isLinked){
  glGetProgramiv(     program_pts_hole, GL_PROGRAM_BINARY_LENGTH, &length);
  #ifdef VERBOSE
  printf(            "program_pts_hole  binary size: %d bytes\n", length);
  #endif
 }else{
  glGetProgramInfoLog(program_pts_hole, SHADER_LOG_MAX_LENGTH, &length, &errorLog[0]);
  printf (           "program_pts_hole  linking failed:\n\n%s\n\n",errorLog);
  glDeleteProgram(    program_pts_hole  );
 }

 /************/       program_pts_flat = glCreateProgram();
 glAttachShader      (program_pts_flat, f_shader_pts_flat);
 glAttachShader      (program_pts_flat, v_shader_pts);
 glBindAttribLocation(program_pts_flat, POINT_SIZE_ATTRIB, "RealSize");
 glLinkProgram       (program_pts_flat);
 glGetProgramiv      (program_pts_flat, GL_LINK_STATUS, &isLinked);
 if(isLinked){
  glGetProgramiv(     program_pts_flat, GL_PROGRAM_BINARY_LENGTH, &length);
  #ifdef VERBOSE
  printf(            "program_pts_flat  binary size: %d bytes\n", length);
  #endif
 }else{
  glGetProgramInfoLog(program_pts_flat, SHADER_LOG_MAX_LENGTH, &length, &errorLog[0]);
  printf (           "program_pts_flat  linking failed:\n\n%s\n\n",errorLog);
  glDeleteProgram(    program_pts_flat  );
 }

 /************/       program_pts_ball = glCreateProgram();
 glAttachShader      (program_pts_ball, f_shader_pts_ball);
 glAttachShader      (program_pts_ball, v_shader_pts);
 glBindAttribLocation(program_pts_ball, POINT_SIZE_ATTRIB, "RealSize");
 glLinkProgram       (program_pts_ball);
 glGetProgramiv      (program_pts_ball, GL_LINK_STATUS, &isLinked);
 if(isLinked){
  glGetProgramiv(     program_pts_ball, GL_PROGRAM_BINARY_LENGTH, &length);
  #ifdef VERBOSE
  printf(            "program_pts_ball  binary size: %d bytes\n", length);
  #endif
 }else{
  glGetProgramInfoLog(program_pts_ball, SHADER_LOG_MAX_LENGTH, &length, &errorLog[0]);
  printf (           "program_pts_ball  linking failed:\n\n%s\n\n",errorLog);
  glDeleteProgram(    program_pts_ball  );
 }

 /************/       program_default = glCreateProgram();
 glAttachShader      (program_default, v_shader_tri);
 glLinkProgram       (program_default);
 glGetProgramiv      (program_default, GL_LINK_STATUS, &isLinked);
 if(isLinked){
  glGetProgramiv(     program_default, GL_PROGRAM_BINARY_LENGTH, &length);
  #ifdef VERBOSE
  printf(            "program_default  binary size: %d bytes\n", length);
  #endif
 }else{
  glGetProgramInfoLog(program_default, SHADER_LOG_MAX_LENGTH, &length, &errorLog[0]);
  printf (           "program_default  linking failed:\n\n%s\n\n",errorLog);
  glDeleteProgram(    program_default  );
 }

 #ifdef WHITE_BACKGROUND
 glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
 #endif

 glDepthFunc(GL_GREATER);
 glClearDepthf(0.0f);
 default_mode();
}






void hypersphere_done() {
 glDeleteShader(v_shader_pts);
 glDeleteShader(f_shader_pts_soft);
 glDeleteShader(f_shader_pts_hole);
 glDeleteShader(f_shader_pts_flat);
 glDeleteShader(f_shader_pts_ball);
 glDeleteProgram(program_pts_soft);
 glDeleteProgram(program_pts_hole);
 glDeleteProgram(program_pts_flat);
 glDeleteProgram(program_pts_ball);
 glDeleteProgram(program_default);
}












#pragma pack(push,1)
typedef union {
 //             relative  relative  relative  viewpoint
 //              x axis    y axis    z axis   position
 struct { float  relx_x,   rely_x,   relz_x,   Xpos,
                 relx_y,   rely_y,   relz_y,   Ypos,
                 relx_z,   rely_z,   relz_z,   Zpos,
                 relx_w,   rely_w,   relz_w,   Wpos; };
 float matrix[16];
} HypersphereView;
HypersphereView  _view_data;
#pragma pack(pop)



#define DEFAULT_MOVE_INTERVAL 0.0018
float _view_dw=0, //
      _view_dx=0, //
      _view_dy=0, // viewpoint velocity/motion
      _view_dz=0; //
int   _view_anchor=0; // boolean: whether the view is anchored or floating
float _view_roll_speed=0.02; // for the keys '<' and '>'
float _view_mouse_sensitivity=1;



void update_view(float move_interval)
{
 if (move_interval < 0) move_interval = DEFAULT_MOVE_INTERVAL;
 float t;
 static int firstTime = 1; if (firstTime) { firstTime = 0;
  // init view matrix
  float vm[16] = { 1, 0, 0, 0,
                   0, 1, 0, 0,
                   0, 0, 1, 0,
                   0, 0, 0, 1 };
  int i; for(i=0;i<16;i++) _view_data.matrix[i] = vm[i];
  // add a bit of motion
  if (!_view_anchor) _view_dz = move_interval * 0.25;
 }
 HypersphereView v = _view_data;

 // calculate input from keyboard
 float direct = keyboard_dz();
 float strafe = keyboard_dx();
 float drop   = keyboard_dy();
 static float roll=0; roll -= !!keymap['<']-!!keymap['>']+!!keymap[',']-!!keymap['.'];
 if (keymap[8]==KEY_FRESHLY_PRESSED) _view_anchor = !_view_anchor; // backspace to toggle 'anchored' vs 'floating' view
 float anchorf = _view_anchor ? 1.0 : 0.125;

 // update velocity based on input
 _view_dw += move_interval * (direct*v.relz_w + strafe*v.relx_w + drop*v.rely_w) * anchorf;
 _view_dx += move_interval * (direct*v.relz_x + strafe*v.relx_x + drop*v.rely_x) * anchorf;
 _view_dy += move_interval * (direct*v.relz_y + strafe*v.relx_y + drop*v.rely_y) * anchorf;
 _view_dz += move_interval * (direct*v.relz_z + strafe*v.relx_z + drop*v.rely_z) * anchorf;

 // update position
 v.Wpos += _view_dw;
 v.Xpos += _view_dx;
 v.Ypos += _view_dy;
 v.Zpos += _view_dz;

 // normalize position vector
 t = v.Wpos*v.Wpos+v.Xpos*v.Xpos+v.Ypos*v.Ypos+v.Zpos*v.Zpos;
 if (t > 0) {
  t = 1.0 / sqrt(t);
  v.Wpos *= t;
  v.Xpos *= t;
  v.Ypos *= t;
  v.Zpos *= t;
 }
 else v.Wpos = 1;

 // update orientation --
 // mouse movement: warp relative z axis
 v.relz_w -= v.relx_w * _mouse_dx*_view_mouse_sensitivity;
 v.relz_x -= v.relx_x * _mouse_dx*_view_mouse_sensitivity;
 v.relz_y -= v.relx_y * _mouse_dx*_view_mouse_sensitivity;
 v.relz_z -= v.relx_z * _mouse_dx*_view_mouse_sensitivity;

 v.relz_w -= v.rely_w * _mouse_dy*_view_mouse_sensitivity;
 v.relz_x -= v.rely_x * _mouse_dy*_view_mouse_sensitivity;
 v.relz_y -= v.rely_y * _mouse_dy*_view_mouse_sensitivity;
 v.relz_z -= v.rely_z * _mouse_dy*_view_mouse_sensitivity;

 // make relative z perpendicular to viewpoint position vector
 float dot = v.relz_w*v.Wpos + v.relz_x*v.Xpos + v.relz_y*v.Ypos + v.relz_z*v.Zpos;
 v.relz_w -= v.Wpos*dot;
 v.relz_x -= v.Xpos*dot;
 v.relz_y -= v.Ypos*dot;
 v.relz_z -= v.Zpos*dot;

 // normalize relative z
 t = v.relz_w*v.relz_w+v.relz_x*v.relz_x+v.relz_y*v.relz_y+v.relz_z*v.relz_z;
 if (t > 0) {
  t = 1.0 / sqrt(t);
  v.relz_w *= t;
  v.relz_x *= t;
  v.relz_y *= t;
  v.relz_z *= t;
 }
 else v.relz_z = 1;

 // roll control: warp relative y axis
 v.rely_w += v.relx_w*roll*_view_roll_speed*anchorf;
 v.rely_x += v.relx_x*roll*_view_roll_speed*anchorf;
 v.rely_y += v.relx_y*roll*_view_roll_speed*anchorf;
 v.rely_z += v.relx_z*roll*_view_roll_speed*anchorf;

 // make relative y perpendicular to relative z
 dot = v.rely_w*v.relz_w + v.rely_x*v.relz_x + v.rely_y*v.relz_y + v.rely_z*v.relz_z;
 v.rely_w -= v.relz_w*dot;
 v.rely_x -= v.relz_x*dot;
 v.rely_y -= v.relz_y*dot;
 v.rely_z -= v.relz_z*dot;

 // make relative y perpendicular to viewpoint position vector
 dot = v.rely_w*v.Wpos + v.rely_x*v.Xpos + v.rely_y*v.Ypos + v.rely_z*v.Zpos;
 v.rely_w -= v.Wpos*dot;
 v.rely_x -= v.Xpos*dot;
 v.rely_y -= v.Ypos*dot;
 v.rely_z -= v.Zpos*dot;

 // normalize relative y
 t = v.rely_w*v.rely_w+v.rely_x*v.rely_x+v.rely_y*v.rely_y+v.rely_z*v.rely_z;
 if (t > 0) {
  t = 1.0 / sqrt(t);
  v.rely_w *= t;
  v.rely_x *= t;
  v.rely_y *= t;
  v.rely_z *= t;
 }
 else v.rely_y = v.relz_z = 1;

 // make relative x perpendicular to relative y
 dot = v.relx_w*v.rely_w + v.relx_x*v.rely_x + v.relx_y*v.rely_y + v.relx_z*v.rely_z;
 v.relx_w -= v.rely_w*dot;
 v.relx_x -= v.rely_x*dot;
 v.relx_y -= v.rely_y*dot;
 v.relx_z -= v.rely_z*dot;

 // make relative x perpendicular to relative z
 dot = v.relx_w*v.relz_w + v.relx_x*v.relz_x + v.relx_y*v.relz_y + v.relx_z*v.relz_z;
 v.relx_w -= v.relz_w*dot;
 v.relx_x -= v.relz_x*dot;
 v.relx_y -= v.relz_y*dot;
 v.relx_z -= v.relz_z*dot;

 // make relative x perpendicular to viewpoint position vector
 dot = v.relx_w*v.Wpos + v.relx_x*v.Xpos + v.relx_y*v.Ypos + v.relx_z*v.Zpos;
 v.relx_w -= v.Wpos*dot;
 v.relx_x -= v.Xpos*dot;
 v.relx_y -= v.Ypos*dot;
 v.relx_z -= v.Zpos*dot;

 // normalize relative x
 t = v.relx_w*v.relx_w+v.relx_x*v.relx_x+v.relx_y*v.relx_y+v.relx_z*v.relx_z;
 if (t > 0) {
  t = 1.0 / sqrt(t);
  v.relx_w *= t;
  v.relx_x *= t;
  v.relx_y *= t;
  v.relx_z *= t;
 }
 else v.relx_x = v.rely_y = v.relz_z = 1;

 // make velocities perpendicular to position vector
 dot = _view_dw*v.Wpos + _view_dx*v.Xpos + _view_dy*v.Ypos + _view_dz*v.Zpos;
 _view_dw -= v.Wpos*dot;
 _view_dx -= v.Xpos*dot;
 _view_dy -= v.Ypos*dot;
 _view_dz -= v.Zpos*dot;

 // update the control-velocities for smooth start/stop
 if (_view_anchor) {
  _view_dw *= 0.5;
  _view_dx *= 0.5;
  _view_dy *= 0.5;
  _view_dz *= 0.5;
  roll *= 0.5;
 }
 else roll *= 0.95;

 // done
 _view_data = v;

 // push this into OpenGL
 glMatrixMode(GL_MODELVIEW);
 glLoadMatrixf(v.matrix);

 float projection_matrix[16] = { 1, 0, 0, 0,
                                 0, 1, 0, 0,
                                 0, 0,-1, 1,
                                 0, 0, 1, 0 };
 projection_matrix[5] = (GLfloat)_screen_x / (GLfloat)_screen_y;
 #ifdef VIEWPORT_SCALE
 projection_matrix[0] /= VIEWPORT_SCALE;
 projection_matrix[5] /= VIEWPORT_SCALE;
 #endif
 glMatrixMode(GL_PROJECTION);
 glLoadMatrixf(projection_matrix);

 // make point shaders work with current screen size
 GLfloat screenSize = _screen_size;
 GLuint current_program;  glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
 GLint uniform_location = glGetUniformLocation(current_program,"ScreenSize");
 glUniform1fv(uniform_location, 1, &screenSize);

 // clear the frame buffer (color + depth) because we just moved the camera
 glPushAttrib(GL_ENABLE_BIT);
 glDepthMask(GL_TRUE);
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 glPopAttrib();
}
