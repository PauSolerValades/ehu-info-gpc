#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "display.h"
#include "io.h"
#include "definitions.h"

/** GLOBAL VARIABLES **/

GLdouble _window_ratio;                     /*Control of window's proportions */
GLdouble _ortho_x_min,_ortho_x_max;         /*Variables for the control of the orthographic projection*/
GLdouble _ortho_y_min ,_ortho_y_max;        /*Variables for the control of the orthographic projection*/
GLdouble _ortho_z_min,_ortho_z_max;         /*Variables for the control of the orthographic projection*/

object3d * _first_object= 0;                /*List of objects*/
object3d * _selected_object = 0;            /*Object currently selected*/

camera * _first_camera = 0;
camera * _selected_camera = 0;

elem_matrix * _actual_matrix = 0;

int mode; //0: objeto, 1: camara, 2: luz

int transformacion;
int referencia;

int camara_interna;

int fill_polygons; //controla si se pintan los polígonos o no

int iluminacion[8];
int flat_smooth; //0: flat, 1: smooth

/** GENERAL INITIALIZATION **/
void initialization (){

    int i = 0;

    fill_polygons = 1;
    mode = 0; //por defecto a objeto.
    transformacion = 0; //translación por defecto
    referencia = 0; //por defecto a objeto.
    camara_interna = 0;
    flat_smooth = 1; //0: flat, 1: smooth

    for(i=0; i<9; i++)
    {
        iluminacion[i] = 0;
    }

    iluminacion[0] = 1;

    /*Initialization of all the variables with the default values*/
    //definimos puntos maximos y minimos de lo que vamos a poder visualizar.
    //zona tridimensional donde se van a tener que ver en pantalla.
    

    _window_ratio = (GLdouble) KG_WINDOW_WIDTH / (GLdouble) KG_WINDOW_HEIGHT;

    /*Definition of the background color*/
    glClearColor(KG_COL_BACK_R, KG_COL_BACK_G, KG_COL_BACK_B, KG_COL_BACK_A);

    /*Definition of the method to draw the objects*/
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


/** MAIN FUNCTION **/
int main(int argc, char** argv) {

    /*First of all, print the help information*/
    print_help();

    /* glut initializations */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);
    glutInitWindowSize(KG_WINDOW_WIDTH, KG_WINDOW_HEIGHT);
    glutInitWindowPosition(KG_WINDOW_X, KG_WINDOW_Y);
    glutCreateWindow(KG_WINDOW_TITLE);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST); //activa el zbuffer.
    glEnable(GL_LIGHTING);

    //deja las luces activadas
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);
    glEnable(GL_LIGHT4);
    glEnable(GL_LIGHT5);
    glEnable(GL_LIGHT6);
    glEnable(GL_LIGHT7);


    if(!flat_smooth)
        glShadeModel(GL_SMOOTH);
    else
        glShadeModel(GL_FLAT);

    /* set the callback functions */
    glutDisplayFunc(display); //donde se hace "la foto". el observador que ve
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard); //evento de llamada por teclado.
    glutSpecialFunc(special); //gestor teclas especiales

    /* this initialization has to be AFTER the creation of the window */
    initialization();

    /* start the main loop */
    glutMainLoop();
    return 0;
}
