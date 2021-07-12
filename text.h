/***
 Some useful functions for putting text on the screen.
 When using these, it's recommended to disable lighting, shading, depth buffer etc.

 Author: Elie Goldman Smith
 This code is too short/trivial to copyright, so it's in the public domain.

 Feel free to copy this file into any project.
***/
#include <string.h>

#define FONT_WIDTH   (9.0f * 2.0f/_screen_size)
#define FONT_HEIGHT (15.0f * 2.0f/_screen_size)

void draw_text(const char *str, float x, float y) {
 glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
 glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();

 #ifdef VIEWPORT_SCALE
 glRasterPos2f(x/VIEWPORT_SCALE, y/VIEWPORT_SCALE);
 #else
 glRasterPos2f(x, y);
 #endif
 while (*str) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *(str++)); // XXX: glutBitmapCharacter() is VERY slow on some computers. I've seen it take frame rates down from 100FPS to 10FPS! Maybe we should find some other way to render text?

 glPopMatrix(); glMatrixMode(GL_MODELVIEW); glPopMatrix();
}


void text_top_left(const char *str, int line_number) {
 draw_text(str, -1.0f,
                 1.0f - FONT_HEIGHT*(line_number+1));
}
void text_top_center(const char *str, int line_number) {
 draw_text(str,       - FONT_WIDTH*strlen(str)*0.5f,
                 1.0f - FONT_HEIGHT*(line_number+1));
}
void text_top_right(const char *str, int line_number) {
 draw_text(str,  1.0f - FONT_WIDTH*strlen(str),
                 1.0f - FONT_HEIGHT*(line_number+1));
}
void text_center(const char *str, int line_number) {
 draw_text(str,       - FONT_WIDTH*strlen(str)*0.5f,
                      - FONT_HEIGHT*(line_number+0.5f));
}
void text_bottom_left(const char *str, int line_number) {
 draw_text(str, -1.0f,
                -1.0f + FONT_HEIGHT*line_number);
}
void text_bottom_center(const char *str, int line_number) {
 draw_text(str,       - FONT_WIDTH*strlen(str)*0.5f,
                -1.0f + FONT_HEIGHT*line_number);
}
void text_bottom_right(const char *str, int line_number) {
 draw_text(str,  1.0f - FONT_WIDTH*strlen(str),
                -1.0f + FONT_HEIGHT*line_number);
}
