// Hello World: Just draws a boring triangle in the middle of the screen.
// Press ESC to quit.
#include "../fullscreen_main.h"


void init()
{
}


void draw()
{
 // Clear the frame buffer
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 // Draw the triangle
 glBegin(GL_TRIANGLES);
 glColor3f(1.0f, 1.0f, 1.0f); // RGB values (range: 0 to 1)
 glVertex2f( 0.0f,  0.8f); //
 glVertex2f(-0.5f, -0.7f); // Screen coordinates (range: -1 to 1)
 glVertex2f( 0.5f, -0.7f); //
 glEnd();
}


void done()
{
}
