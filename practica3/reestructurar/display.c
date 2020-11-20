#include "definitions.h"
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdio.h>

/** EXTERNAL VARIABLES **/

extern GLdouble _window_ratio;
extern GLdouble _ortho_x_min,_ortho_x_max;
extern GLdouble _ortho_y_min,_ortho_y_max;
extern GLdouble _ortho_z_min,_ortho_z_max;

extern object3d *_first_object;
extern object3d *_selected_object;

extern camera * _first_camera;
extern camera * _selected_camera;

extern int camara_interna; //0: camara no interna, 1: camara interna

/**
 * @brief Function to draw the axes
 */
void draw_axes()
{
    /*Draw X axis*/
    glColor3f(KG_COL_X_AXIS_R,KG_COL_X_AXIS_G,KG_COL_X_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(KG_MAX_DOUBLE,0,0);
    glVertex3d(-1*KG_MAX_DOUBLE,0,0);
    glEnd();
    /*Draw Y axis*/
    glColor3f(KG_COL_Y_AXIS_R,KG_COL_Y_AXIS_G,KG_COL_Y_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(0,KG_MAX_DOUBLE,0);
    glVertex3d(0,-1*KG_MAX_DOUBLE,0);
    glEnd();
    /*Draw Z axis*/
    glColor3f(KG_COL_Z_AXIS_R,KG_COL_Z_AXIS_G,KG_COL_Z_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(0,0,KG_MAX_DOUBLE);
    glVertex3d(0,0,-1*KG_MAX_DOUBLE);
    glEnd();
}


/**
 * @brief Callback reshape function. We just store the new proportions of the window
 * @param width New width of the window
 * @param height New height of the window
 */
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    /*  Take care, the width and height are integer numbers, but the ratio is a GLdouble so, in order to avoid
     *  rounding the ratio to integer values we need to cast width and height before computing the ratio */
    _window_ratio = (GLdouble) width / (GLdouble) height;
}

void init_camera(){
    
    camera *new_camera;

    new_camera = (camera *)malloc(sizeof(camera));
    new_camera->nextptr = NULL;//apuntem el seguent punter a 0

    glGetDoublev(GL_PROJECTION_MATRIX, new_camera->M);
    glGetDoublev(GL_PROJECTION_MATRIX, new_camera->M_inv);

    new_camera->M[14] = INIT_CAMERA;
    new_camera->M_inv[14] = -INIT_CAMERA;
    new_camera-> r = 0.1;
    new_camera-> l = -0.1;
    new_camera-> t = 0.1;
    new_camera-> b = -0.1;
    new_camera-> n = 0.1;
    new_camera-> f = 1000.0;

    //asignamos la matriz a la id.
    _selected_camera = new_camera;
    _first_camera = new_camera;
    
    _selected_camera->type = 0; //modo vuelo
    _selected_camera->pers = 1; //modo paralelo.
}

GLint poligono_visible(int f)
{
    /*
    //pel que es veu fer-ho amb Gl toques coses que estaven bé ja i clar, no ho sé fer bé xd
    elem_matrix *aux;
    aux = (elem_matrix*)malloc(sizeof(elem_matrix));
    aux->nextptr = NULL;

    
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    glGetDoublev(GL_MODELVIEW_MATRIX, aux->M);
    glGetDoublev(GL_MODELVIEW_MATRIX, aux->inv_M);
    
    glLoadMatrixd(_selected_object->display->inv_M);
    glTranslated(_selected_camera->M[12],_selected_camera->M[13], _selected_camera->M[14]);
    glGetDoublev(GL_MODELVIEW_MATRIX, aux->M);
    */

    int i;
    double Eo[3], eval;

    for(i=0; i<3; i++)
    {
        Eo[i] = _selected_object->display->inv_M[12+i]*_selected_camera->M[12+i];
    }

    //Ax+By+Cz+D=0
    eval = _selected_object->face_table[f].vn[0]*Eo[0] +
           _selected_object->face_table[f].vn[1]*Eo[1] +
           _selected_object->face_table[f].vn[2]*Eo[2] +
           _selected_object->face_table[f].ti;

    //printf("Eval: %f\n", eval);
    
    //free(aux);

    if(eval < 0)
        return 0;
    else
        return 1;
}

/**
 * @brief Callback display function
 */
//_first_objectfunción que SOLO DIBUJA. No modifica nada. Este es el observador.
void display(void) {
    GLint v_index, v, f, dibuja;
    object3d *aux_obj = _first_object; //puntero al primer elemento de la lista de objetos.
    /* Clear the screen */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Define the projection */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if(_first_camera == 0)
        init_camera(); //we load the first camera with the identity


    /* TODO: inicializar si ortho o frustrum*/

    if(_selected_camera->pers){
        //proyección
        glFrustum(_selected_camera->l, _selected_camera->r,_selected_camera->b,_selected_camera->t,_selected_camera->n,_selected_camera->f);
    }
    else
    {
        glOrtho(_ortho_x_min, _ortho_x_max, _ortho_y_min, _ortho_y_max, _ortho_z_min, _ortho_z_max);
    }
    

   //glOrtho(_ortho_x_min, _ortho_x_max, _ortho_y_min, _ortho_y_max, _ortho_z_min, _ortho_z_max);
    /* Now we start drawing the object */
    glMatrixMode(GL_MODELVIEW);

    /*First, we draw the axes*/
    glLoadIdentity();
    draw_axes();

    if(_selected_object != 0)
    {
        if(camara_interna) //if camera mode is activated
            glLoadMatrixd(_selected_object->display->inv_M);
        else
            glLoadMatrixd(_selected_camera->M_inv); //Cargar la matriz de la camara actual cuando funcione.
    }

    /*Now each of the objects in the list*/
    while (aux_obj != 0) { //dibuja mientras el puntero no apunte a null.

        glPushMatrix();

        /* Select the color, depending on whether the current object is the selected one or not */
        if (aux_obj == _selected_object){
            glColor3f(KG_COL_SELECTED_R,KG_COL_SELECTED_G,KG_COL_SELECTED_B);
        }else{
            glColor3f(KG_COL_NONSELECTED_R,KG_COL_NONSELECTED_G,KG_COL_NONSELECTED_B);
        }

        /* Draw the object; for each face create a new polygon with the corresponding vertices */
        glMultMatrixd(aux_obj->display->M); //debemos cambiar mptr por display, dado que display necesita el puntero que apunta a la matriz actual del objeto.
        for (f = 0; f < aux_obj->num_faces; f++) {
            glBegin(GL_POLYGON);

            dibuja = poligono_visible(f);
            
            if(dibuja)
            {
                for (v = 0; v < aux_obj->face_table[f].num_vertices; v++) {
                    v_index = aux_obj->face_table[f].vertex_table[v];
                    glVertex3d(aux_obj->vertex_table[v_index].coord.x,
                            aux_obj->vertex_table[v_index].coord.y,
                            aux_obj->vertex_table[v_index].coord.z);

                }
            }
            glEnd();
        }
        aux_obj = aux_obj->next;

        glPopMatrix();
    }
    /*Do the actual drawing*/
    glFlush();
}
