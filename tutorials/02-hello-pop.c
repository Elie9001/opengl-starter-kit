// Hello Pop: Same as Hello World, but:
//   you can press 'P' to make the triangle pop out, and
//   you can hold 'B' to make the triangle blue
#include "../fullscreen_main.h"


void init()
{
}


void draw()
{
 float       blue = 0.0f;
 static float pop = 0.0f;
 pop *= 0.875f;
 if (keymap['P']==KEY_FRESHLY_PRESSED) pop = 0.5f;
 if (keymap['B']) blue = 0.5f;

 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 glBegin(GL_TRIANGLES);
 glColor3f(0.5f+pop,  0.5f+pop,  0.5f+blue);
 glVertex2f( 0.0f,      0.8f+pop);
 glVertex2f(-0.5f-pop, -0.7f-pop);
 glVertex2f( 0.5f+pop, -0.7f-pop);
 glEnd();
}


void done()
{
}
