/***
 In the previous tutorials, we were drawing using screen coordinates.
 That's cool for 2D programs, but what about 3D?

 Well it works almost the same way. We still draw triangles etc by specifying
 the coordinates of the vertices. But this time, it's "world coordinates".
 OpenGL converts those into "screen coordinates" using a matrix (math),
 which depends on your camera angle etc.

 To keep it simple, we include flatground.h, which takes care of
 - the math
 - keyboard & mouse inputs
 So the user can move the camera around like a video game.

 All we have to do is make the 3D world.
 In this case, it's a simple house.
***/

#include "../flatground.h"  // flatground.h already includes fullscreen_main.h


void init()
{
 // This next line of code is always needed:
 flatground_init();
}


void draw()
{
 // This next line of code is always needed.
 // It checks keyboard & mouse inputs, and moves the camera accordingly,
 // and updates the view matrix.
 update_view(DEFAULT_MOVE_INTERVAL); // This is always needed when you use flatground.h


 // We draw the shadow first.
 // Soft blurry shadows are a feature of flatground.h
 shadow_mode();   // this sets up the proper blending mode and shader.
 shadow(0,15,10); // this draws a shadow under the house: position=[0,15], radius=10
 
 // back to normal triangle-drawing mode
 default_mode();
 
 
 // Next we draw the house.
 // We use flatground's coordinate system:
 //  user walks along the xy plane
 //  z=0 is the ground
 //  z=1 is the view height (unless you jump or duck) 

 
 // front face of the house
 glBegin(GL_POLYGON);
 glColor3f(1.f, 1.f, 1.f); 
 glVertex3f(-4.f, 10.f, 0.f);
 glVertex3f( 4.f, 10.f, 0.f);
 glVertex3f( 4.f, 10.f, 5.f);
 glVertex3f( 0.f, 10.f, 8.f);
 glVertex3f(-4.f, 10.f, 5.f);
 glEnd();

 // back face of the house
 glBegin(GL_POLYGON);
 glVertex3f( 4.f, 20.f, 0.f);
 glVertex3f(-4.f, 20.f, 0.f);
 glVertex3f(-4.f, 20.f, 5.f);
 glVertex3f( 0.f, 20.f, 8.f);
 glVertex3f( 4.f, 20.f, 5.f);
 glEnd();

 // sides
 glBegin(GL_QUAD_STRIP);
 glColor3f(0.f, 0.f, 0.f);
 glVertex3f(-4.f, 20.f, 0.f);
 glVertex3f(-4.f, 10.f, 0.f);
 glVertex3f(-4.f, 20.f, 5.f);
 glVertex3f(-4.f, 10.f, 5.f);
 glColor3f(0.2f, 0.5f, 0.2f);
 glVertex3f( 0.f, 20.f, 8.f);
 glVertex3f( 0.f, 10.f, 8.f);
 glColor3f(0.f, 0.f, 0.f);
 glVertex3f( 4.f, 20.f, 5.f);
 glVertex3f( 4.f, 10.f, 5.f);
 glVertex3f( 4.f, 20.f, 0.f);
 glVertex3f( 4.f, 10.f, 0.f);
 glEnd();
 
 // front door
 glBegin(GL_QUADS);
 glVertex3f(-0.8f, 9.9f, 0.0f);
 glVertex3f( 0.8f, 9.9f, 0.0f);
 glVertex3f( 0.8f, 9.9f, 2.4f);
 glVertex3f(-0.8f, 9.9f, 2.4f);
 glEnd();
}


void done()
{
 // This next line of code is always needed:
 flatground_done();
}
