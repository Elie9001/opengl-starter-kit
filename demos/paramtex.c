/***
 Just a little experiment with parametrically-generated textures.
 I made a shader that produces a texture from a set of frequencies/waves.
 In theory, you can get fine details with perfect anisotropic filtering.

 To compile this:  gcc paramtex.c -o paramtex -ffast-math -O -lGL -lglut -lm

 Author: Elie Goldman Smith
 This code is too unfinished to copyright, so it is in the public domain.
***/

#define USE_MULTISAMPLING // This should supersample the geometry edges, not the shading. My shader already has built-in filtering; doesn't need additional antialiasing.
#include "../flatground.h"
#include "../line.h"

#define BOUNDARY 32.0f
#define SIZE 10.0f
#define FRAME_THICKNESS 0.05f
float aspect=1;
GLuint dl_geometry;


GLuint program_tex;
GLuint f_shader_tex;
GLchar * f_src_tex =
"float shade=0.0;"
"vec2 dx_vtc = dFdx(vec2(gl_TexCoord[0]));"
"vec2 dy_vtc = dFdy(vec2(gl_TexCoord[0]));"
"void addWave(vec2 freq) {"
" float d = max( abs(dot(freq,dx_vtc)), abs(dot(freq,dy_vtc)) );"
" if (d < 0.33) shade += (0.33-d) * clamp(abs(fract(dot(freq,vec2(gl_TexCoord[0])))-0.5)-0.25, -0.125, 0.125);" // Even though 'if' statements are stereotypically slow in shaders, this one isn't. The test-condition will usually be false for an entire block of pixels, so no GPU core is holding back the others.
"}"
"void main(void) {"
" addWave(vec2(   5.0,    0.0)); shade *= 0.2;"
" addWave(vec2(  64.0,    0.0));"
" addWave(vec2(   0.0,   64.0));"
" addWave(vec2(1600.0,    0.0));"
" addWave(vec2(   0.0, 1600.0));"
" addWave(vec2(1984.0,    0.0));"
" addWave(vec2(   0.0, 1984.0));"
" gl_FragColor = gl_Color+shade*24.0;"
"}"; // TODO: make these into uniform params instead of hardcoded in the shader. Perhaps name them: freqs[], nfreqs, amplitude. Is it worth having a separate amplitude for each wave? idk yet. and how about phase? maybe just use some texcoord trickery instead? dont wanna add too many shader clock cycles. Another trick: maybe group the freqs into sets (clustered in vector space)... Or actually, instead of groups of single-frequency waves, maybe use some other wavy functions such as chirps, could be more efficient, needing less of them for the same effect, idk; cool thing about a chirp is you could cull it differently in different areas, as the local frequency interacts with pixel derivatives.




void init() {
 flatground_init();
 _view_Ypos = -SIZE;
 glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

 // set up the shader
 int success=0;
 int length=0;
 char errorLog[SHADER_LOG_MAX_LENGTH];
 /************/      f_shader_tex = glCreateShader(GL_FRAGMENT_SHADER);
 glShaderSource  (   f_shader_tex, 1, &f_src_tex, NULL);
 glCompileShader (   f_shader_tex  );
 glGetShaderiv   (   f_shader_tex, GL_COMPILE_STATUS, &success);
 if(!success)
 {
  glGetShaderInfoLog(f_shader_tex, SHADER_LOG_MAX_LENGTH, &length, &errorLog[0]);
  printf         (  "f_shader_tex  compilation failed:\n\n%s\n\n", errorLog);
  glDeleteShader (   f_shader_tex  );
 }
 /************/       program_tex = glCreateProgram();
 glAttachShader      (program_tex, f_shader_tex);
 glLinkProgram       (program_tex);
 glGetProgramiv      (program_tex, GL_LINK_STATUS, &success);
 if(success){
  glGetProgramiv(     program_tex, GL_PROGRAM_BINARY_LENGTH, &length);
  printf(            "program_tex  binary size: %d bytes\n", length);
 }else{
  glGetProgramInfoLog(program_tex, SHADER_LOG_MAX_LENGTH, &length, &errorLog[0]);
  printf (           "program_tex  linking failed:\n\n%s\n\n",errorLog);
  glDeleteProgram(    program_tex  );
 }

 // set up a display list for untextured geometry
 dl_geometry = glGenLists(1);
 glNewList(dl_geometry, GL_COMPILE);
 line(   new_vec3(-SIZE*0.5f, SIZE*aspect, 0),
         new_vec3(-SIZE*0.5f, FRAME_THICKNESS, 0),   new_vec3(0,0,1), FRAME_THICKNESS);
 line_to(new_vec3(-SIZE*0.5f, FRAME_THICKNESS, SIZE*aspect));
 line_to(new_vec3(-SIZE*0.5f, SIZE*aspect, 0));
 line(   new_vec3( SIZE*0.5f, SIZE*aspect, 0),
         new_vec3( SIZE*0.5f, FRAME_THICKNESS, 0),   new_vec3(0,0,1), FRAME_THICKNESS);
 line_to(new_vec3( SIZE*0.5f, FRAME_THICKNESS, SIZE*aspect));
 line_to(new_vec3( SIZE*0.5f, SIZE*aspect, 0));
 line(   new_vec3( BOUNDARY, -BOUNDARY, 0.0f),
         new_vec3( BOUNDARY, -BOUNDARY, SIZE),  new_vec3(0,0,0), FRAME_THICKNESS*2);
 line(   new_vec3( BOUNDARY,  BOUNDARY, 0.0f),
         new_vec3( BOUNDARY,  BOUNDARY, SIZE),  new_vec3(0,0,0), FRAME_THICKNESS*2);
 line(   new_vec3(-BOUNDARY,  BOUNDARY, 0.0f),
         new_vec3(-BOUNDARY,  BOUNDARY, SIZE),  new_vec3(0,0,0), FRAME_THICKNESS*2);
 line(   new_vec3(-BOUNDARY, -BOUNDARY, 0.0f),
         new_vec3(-BOUNDARY, -BOUNDARY, SIZE),  new_vec3(0,0,0), FRAME_THICKNESS*2);
 line_mv(new_vec3(-BOUNDARY, -BOUNDARY, SIZE-0.7f));
 line_to(new_vec3( BOUNDARY, -BOUNDARY, SIZE-0.7f));
 line_to(new_vec3( BOUNDARY,  BOUNDARY, SIZE-0.7f));
 line_to(new_vec3(-BOUNDARY,  BOUNDARY, SIZE-0.7f));
 line_to(new_vec3(-BOUNDARY, -BOUNDARY, SIZE-0.7f));
 line_mv(new_vec3(-BOUNDARY, -BOUNDARY, SIZE-1.4f));
 line_to(new_vec3( BOUNDARY, -BOUNDARY, SIZE-1.4f));
 line_to(new_vec3( BOUNDARY,  BOUNDARY, SIZE-1.4f));
 line_to(new_vec3(-BOUNDARY,  BOUNDARY, SIZE-1.4f));
 line_to(new_vec3(-BOUNDARY, -BOUNDARY, SIZE-1.4f));
 glEnd();
 glEndList();
}




void draw() {
 update_view(0.1f);

 // soft horizon effect
 shadow_mode();
 _shadow_darkness = DEFAULT_SHADOW_DARKNESS;
 shadow(0,0, BOUNDARY*3.0f);

 // bright sky soft glow
 glBlendEquation(GL_FUNC_ADD);
 glBegin(GL_QUADS);
 glColor3f(0.14f, 0.14f, 0.14f);
 glTexCoord2f(0.0f, 1.0f);
 glVertex3f(-8000.f,  8000.f, 1000.f);
 glTexCoord2f(1.0f, 1.0f);
 glVertex3f( 8000.f,  8000.f, 1000.f);
 glTexCoord2f(1.0f, 0.0f);
 glVertex3f( 8000.f, -8000.f, 1000.f);
 glTexCoord2f(0.0f, 0.0f);
 glVertex3f(-8000.f, -8000.f, 1000.f);
 glEnd();

 // green floor
 default_mode();
 glUseProgram(program_tex);
 glBegin(GL_QUADS);
 glColor3f(0.3f, 0.7f, 0.3f);
 glTexCoord2f(0.0f, 1.0f);
 glVertex3f(-BOUNDARY-0.5f, -BOUNDARY-0.5f, 0.0f);
 glTexCoord2f(1.0f, 1.0f);
 glVertex3f( BOUNDARY+0.5f, -BOUNDARY-0.5f, 0.0f);
 glTexCoord2f(1.0f, 0.0f);
 glVertex3f( BOUNDARY+0.5f,  BOUNDARY+0.5f, 0.0f);
 glTexCoord2f(0.0f, 0.0f);
 glVertex3f(-BOUNDARY-0.5f,  BOUNDARY+0.5f, 0.0f);
 glEnd();

 // shadow under canvas
 shadow_mode();
 _shadow_darkness = 0.4f;
 shadow(0,0, SIZE*0.7f);

 // misc
 default_mode();
 glCallList(dl_geometry);

 // canvas
 glUseProgram(program_tex);
 glBegin(GL_QUADS);
 glColor3f(0.5f, 0.5f, 0.5f);
 glTexCoord2f(0.0f, 1.0f);
 glVertex3f(-SIZE*0.5f, 0, 0);
 glTexCoord2f(1.0f, 1.0f);
 glVertex3f( SIZE*0.5f, 0, 0);
 glColor3f(1.0f, 1.0f, 1.0f);
 glTexCoord2f(1.0f, 0.0f);
 glVertex3f( SIZE*0.5f, 0, SIZE*aspect);
 glTexCoord2f(0.0f, 0.0f);
 glVertex3f(-SIZE*0.5f, 0, SIZE*aspect);
 glEnd();
}


void done() {
 glDeleteLists(dl_geometry, 1);
 glDeleteProgram(program_tex);
 glDeleteShader(f_shader_tex);
 flatground_done();
}
