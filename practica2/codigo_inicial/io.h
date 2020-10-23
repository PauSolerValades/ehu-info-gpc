#ifndef IO_H
#define IO_H
#include "definitions.h"

void destructor(object3d *object);
void keyboard(unsigned char key, int x, int y);
void special(int k, int x, int y);
void print_help();
void print_enonmode();
void print_eworld();
void print_enonobject();
void borrar_matrices(elem_matrix *first_prt, elem_matrix *last_ptr);

#endif // IO_H
