/***
 Elie's OpenGL Starter Kit - flatground.h

 This is a "world viewer" header.
 It controls the OpenGL model/view/projection matrices, by listening to user input from the keyboard and mouse.
 You walk on a flat ground defined by z=0. Your view height is z=1 when you aren't jumping or ducking. All your vertices should have z >= 0.
 Version: 1.0

 For an example of how to use this header, see: tutorials/10-house.c

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
void flatground_init();

// Call this function in your draw() code:
void update_view(float move_interval);  // This takes care of keyboard & mouse inputs, moves the camera/matrix accordingly, and clears the frame buffer. Note: move_interval should be a very small number. When in doubt, use DEFAULT_MOVE_INTERVAL. Basically it's your accelleration (delta delta position per frame per frame).

// Call this function in your done() code:
void flatground_done();


// Functions you can use in your draw() or init():

void points_mode_soft(); // for drawing points as orbs of light.
void points_mode_hole(); // for drawing points with a hole in middle.
void points_mode_flat(); // for drawing points as circles. Uses depth buffer.
void points_mode_ball(); // for drawing points as spheres. Uses depth buffer.
void shadow_mode();      // for drawing soft shadows.
void default_mode();     // go back to normal (for drawing triangles etc).

void shadow(float x, float y, float radius); // puts a soft shadow on the ground
void point_size(float size); // sets the "physical" size of the next point you draw


/** Notes:
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


// Field-of-vision scale (example: 1.0 is 90 degrees along x axis of screen)
#define FOV_SCALE         1.2f
#define FOV_SCALE_STRING "1.2" // same number but in a string, for shaders





#ifndef DONT_ANTIALIAS_OPAQUE_POINTS
#define ALPHA_BLEND_OPAQUE_EDGES
#endif


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
  #ifdef POINT_SIZE_LIMIT_STRING
  " gl_PointSize = clamp(ScreenSize*RealSize/gl_Position.w,  3.0, ScreenSize*"POINT_SIZE_LIMIT_STRING");"
  #else
  " gl_PointSize = max(ScreenSize*RealSize/gl_Position.w,  3.0);"
  #endif
  #ifdef ALPHA_BLEND_OPAQUE_EDGES
  " aaFactor = gl_PointSize + 6.0;"
  #endif
  " gl_FrontColor = gl_Color;"
  " spherePop = gl_Position.z / (gl_Position.w - RealSize*"FOV_SCALE_STRING")"
  "           - gl_Position.z /  gl_Position.w;"
  "}";



GLuint f_shader_pts_soft;
GLchar * f_src_pts_soft =
  "void main(void) {"
  " vec2 v = vec2(gl_TexCoord[0]) - vec2(0.5);"
  " float shade = max(0.0, 1.5 - 6.0*dot(v,v));"
  " gl_FragColor = gl_Color * shade;"
  "}";



GLuint f_shader_pts_hole;
GLchar * f_src_pts_hole =
  "void main(void) {"
  " vec2 v = vec2(gl_TexCoord[0]) - vec2(0.5);"
  " float shade = dot(v,v);"
  "       shade = max(0.0, shade*(0.25-shade)*80.0);"
  " gl_FragColor = gl_Color * shade;"
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
  " gl_FragColor.rgb = gl_Color.rgb;"
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
  " gl_FragColor.rgb = gl_Color.rgb - 0.5*(c + v.y);"
  #else
  " float z = sqrt(0.25 - d);"
  " gl_FragDepth = gl_FragCoord.z + spherePop * z;"
  " gl_FragColor.rgb = gl_Color.rgb + 0.5*(z - v.y - 0.5);"
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
GLuint program_shadows;
GLuint program_default;





void points_mode_soft()
{
 glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
 glEnable(GL_POINT_SPRITE_ARB);
 glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

 glEnable(GL_BLEND);
 glBlendFunc(GL_ONE, GL_ONE);
 glBlendEquation(GL_FUNC_ADD);

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
 glBlendFunc(GL_ONE, GL_ONE);
 glBlendEquation(GL_FUNC_ADD);

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



void shadow_mode() {
 glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
 glDisable(GL_POINT_SPRITE_ARB);
 glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_FALSE);

 glDepthMask(GL_FALSE);
 #ifdef DEPTH_TEST_SHADOWS
 glEnable(GL_DEPTH_TEST);
 #else
 glDisable(GL_DEPTH_TEST);
 #endif

 glEnable(GL_BLEND);
 glBlendFunc(GL_ONE, GL_ONE);
 glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);

 glUseProgram(program_shadows);
}



void default_mode() {
 glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
 glDisable(GL_POINT_SPRITE_ARB);
 glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_FALSE);

 //glBlendEquation(GL_FUNC_ADD);
 //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 glDisable(GL_BLEND);

 glEnable(GL_DEPTH_TEST);
 glDepthMask(GL_TRUE);

 glUseProgram(program_default);
}






#define POINT_SIZE_ATTRIB 7
void point_size(float size) { glVertexAttrib1fv(POINT_SIZE_ATTRIB, &size); }



#define DEFAULT_SHADOW_DARKNESS 0.17f
float _shadow_darkness = DEFAULT_SHADOW_DARKNESS;

void shadow(float x, float y, float radius) {
 if (radius > 0.0f) {
  glBegin(GL_QUADS);
  glColor3f(_shadow_darkness, _shadow_darkness, _shadow_darkness);
  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(x - radius, y - radius);
  glTexCoord2f(1.0f, 0.0f);
  glVertex2f(x + radius, y - radius);
  glTexCoord2f(1.0f, 1.0f);
  glVertex2f(x + radius, y + radius);
  glTexCoord2f(0.0f, 1.0f);
  glVertex2f(x - radius, y + radius);
  glEnd();
 }
}






#define SHADER_LOG_MAX_LENGTH 4096

void flatground_init()
{
 GLint isCompiled = 0;
 GLint isLinked = 0;
 GLint length = 0;
 static GLchar errorLog[SHADER_LOG_MAX_LENGTH];


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

 /************/       program_shadows = glCreateProgram();
 glAttachShader      (program_shadows, f_shader_pts_soft);
 glLinkProgram       (program_shadows);
 glGetProgramiv      (program_shadows, GL_LINK_STATUS, &isLinked);
 if(isLinked){
  glGetProgramiv(     program_shadows, GL_PROGRAM_BINARY_LENGTH, &length);
  #ifdef VERBOSE
  printf(            "program_shadows  binary size: %d bytes\n", length);
  #endif
 }else{
  glGetProgramInfoLog(program_shadows, SHADER_LOG_MAX_LENGTH, &length, &errorLog[0]);
  printf (           "program_shadows  linking failed:\n\n%s\n\n",errorLog);
  glDeleteProgram(    program_shadows  );
 }

 /************/       program_default = glCreateProgram();
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


 glDepthFunc(GL_GREATER);
 glClearDepthf(0.0f);
 glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
 default_mode();
}





void flatground_done() {
 glDeleteShader(v_shader_pts);
 glDeleteShader(f_shader_pts_soft);
 glDeleteShader(f_shader_pts_hole);
 glDeleteShader(f_shader_pts_flat);
 glDeleteShader(f_shader_pts_ball);
 glDeleteProgram(program_pts_soft);
 glDeleteProgram(program_pts_hole);
 glDeleteProgram(program_pts_flat);
 glDeleteProgram(program_pts_ball);
 glDeleteProgram(program_shadows);
 glDeleteProgram(program_default);
}













#define DEFAULT_MOVE_INTERVAL 0.25

// the coordinate system uses
// X and Y for North,South,East,West
// and Z for altitude
float _view_Xpos=0, //
      _view_Ypos=0, // location of viewpoint
      _view_Zpos=1; //
float _view_dx = 0, //
      _view_dy = 0, // viewpoint velocity
      _view_dz = 0; //
float _view_direction=0; // horizontal looking angle
float _view_eyelevel=90; // vertical looking angle
float _view_mouse_sensitivity = 0.25;

void update_view(float move_interval)
{
 if (move_interval < 0) move_interval = DEFAULT_MOVE_INTERVAL;
 const float JUMP_POWER = 0.5; //TODO: make these work proportionally to move_interval
 const float GRAVITY = 0.04;   //  ... but the problem is, i like the current effect of randomly jumping higher on some bounces, which is caused by a glitch. the glitch goes away when i make things proportional to move_interval. so i gotta think of how to get the same nice bounces within a more reliable physics.

 // used for rotations and movement
 float cosD = cosf(_view_direction*M_PI/180.0f);
 float sinD = sinf(_view_direction*M_PI/180.0f);
 float cosE = cosf(_view_eyelevel*M_PI/180.0f);
 float sinE = sinf(_view_eyelevel*M_PI/180.0f);

 // calculate input from keyboard
 int direct = keyboard_dz();
 int strafe = keyboard_dx();
 int jump   = keyboard_dy();

 // update velocities based on input
 _view_dx += move_interval * (direct*sinD + strafe*cosD);
 _view_dy += move_interval * (direct*cosD - strafe*sinD);
 if (_view_Zpos <= 1.0) _view_dz = JUMP_POWER*jump + (1.0-_view_Zpos)*0.7;

 // update view based on velocities    TODO: add some up-and-down motion during walking
 _view_Xpos += _view_dx;
 _view_Ypos += _view_dy;
 _view_Zpos += _view_dz;
 _view_direction -= _mouse_dx*_view_mouse_sensitivity*360;
 _view_eyelevel  += _mouse_dy*_view_mouse_sensitivity*360;

 // fix it if it's out of range
 while (_view_direction >  360) _view_direction -= 360;
 while (_view_direction < -360) _view_direction += 360;
 if    (_view_eyelevel  >  170) _view_eyelevel = 170;
 if    (_view_eyelevel  <  -10) _view_eyelevel = -10;

 // update the velocities for smooth start/stop
 _view_dx *= 0.5;
 _view_dy *= 0.5;
 _view_dz -= GRAVITY;


 // push this view into OpenGL
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 glRotatef(_view_eyelevel,  1.0f, 0.0f, 0.0f);
 glRotatef(_view_direction, 0.0f, 0.0f, 1.0f);
 glTranslatef(-_view_Xpos, -_view_Ypos, -_view_Zpos);

 // projection
 float a = -(float)_screen_x / (float)_screen_y;
 float b = FOV_SCALE;
 float c = move_interval;
 float projection_matrix[16] = { 1,  0,  0,  0,
                                 0,  a,  0,  0,
                                 0,  0,  0,  b,
                                 0,  0,  c,  0 };
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
