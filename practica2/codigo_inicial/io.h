#ifndef IO_H
#define IO_H
#include "definitions.h"

void destructor(object3d *object);
void keyboard(unsigned char key, int x, int y);
void special(int a, int x, int y);
void print_help();
void print_enonmode();
void print_eworld();
void print_enonobject();

#endif // IO_H
