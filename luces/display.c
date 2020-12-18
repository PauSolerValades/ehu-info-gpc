#include "definitions.h"
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>

/** EXTERNAL VARIABLES **/

extern GLdouble _window_ratio;

extern object3d *_first_object;
extern object3d *_selected_object;

extern camera * _first_camera;
extern camera * _selected_camera;

extern int camara_interna; //0: camara no interna, 1: camara interna
extern int flat_smooth; //0: flat, 1: smooth

void dibuja_normales(object3d *aux_obj, GLint f);
void init_luces();

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

void init_luces()
{

    //TODO: pasarle el vector normalizado

    GLfloat posiSol[4] = {5,5,0,0}; 
    GLfloat rgba[4] = {0.8,0.8,0.8,1.0};
    
    //un sol
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, rgba);
    glLightfv(GL_LIGHT0, GL_SPECULAR, rgba); //Joseba dice queesta siempre con el mismo valro que la otra
    glLightfv(GL_LIGHT0, GL_POSITION, posiSol); //aquí tocar el vector para que el sol no se mueva con la cámara
    glLightfv(GL_LIGHT0, GL_DIFFUSE, rgba);



    GLfloat posiBomb[4] = {-2,7,0,1};

    //una bombilla
    glLightfv(GL_LIGHT1,GL_SPECULAR,rgba);
    glLightfv(GL_LIGHT1,GL_POSITION,posiBomb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, rgba);


    //TODO: pasarle al foco el vector del objeto para que se coloque en su lugar
    //este es el foco que apunta siempre al objeto
    GLfloat posicion2[4] ={0,-5,0,1}; //como estamos en el sistema de referencia de la camara, la posicion 0,0,0 siempre mirará la cámara.
    GLfloat vector[4] = {0,0,-1,1}; //de la misma manera, en la referencia de la camara el 0,0,-1 mira siempre dónde la camara

    glLightfv(GL_LIGHT2, GL_POSITION, posicion2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, rgba);
    glLightfv(GL_LIGHT2, GL_SPECULAR, rgba);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, vector);

    glLightfv(GL_LIGHT3, GL_SPECULAR, rgba);
    glLightfv(GL_LIGHT3, GL_POSITION, posiSol);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, rgba);


    glLightfv(GL_LIGHT4, GL_SPECULAR, rgba);
    glLightfv(GL_LIGHT4, GL_POSITION, posiSol);
    glLightfv(GL_LIGHT4, GL_DIFFUSE, rgba);


    glLightfv(GL_LIGHT5, GL_SPECULAR, rgba);
    glLightfv(GL_LIGHT5, GL_POSITION, posiSol);
    glLightfv(GL_LIGHT5, GL_DIFFUSE, rgba);


    glLightfv(GL_LIGHT6, GL_SPECULAR, rgba);
    glLightfv(GL_LIGHT6, GL_POSITION, posiSol);
    glLightfv(GL_LIGHT6, GL_DIFFUSE, rgba);


    glLightfv(GL_LIGHT7, GL_SPECULAR, rgba);
    glLightfv(GL_LIGHT7, GL_POSITION, posiSol);
    glLightfv(GL_LIGHT7, GL_DIFFUSE, rgba);


}

void init_camera(){
    
    camera *new_camera;
    elem_matrix *matrix_camera;

    new_camera = (camera *)malloc(sizeof(camera));
    new_camera->nextptr = NULL;//apuntem el seguent punter a 0

    matrix_camera = (elem_matrix *)malloc(sizeof(elem_matrix));
    matrix_camera->nextptr = NULL;

    glGetDoublev(GL_PROJECTION_MATRIX, matrix_camera->M);
    glGetDoublev(GL_PROJECTION_MATRIX, matrix_camera->inv_M);

    matrix_camera->M[14] = INIT_CAMERA;
    matrix_camera->inv_M[14] = -INIT_CAMERA;

    new_camera-> r = 0.1;
    new_camera-> l = -0.1;
    new_camera-> t = 0.1;
    new_camera-> b = -0.1;
    new_camera-> n = 0.1;
    new_camera-> f = 1000.0;

    new_camera->first = matrix_camera;
    new_camera->actual = matrix_camera;

    //asignamos la matriz a la id.
    _selected_camera = new_camera;
    _first_camera = new_camera;
    
    _selected_camera->type = 0; //modo vuelo
    _selected_camera->pers = 1; //modo paralelo.
}

GLint poligono_visible(double *M, double Av, double Bv, double Cv, double Dv)
{

    int i;
    double *N, Eo[3], eval;

    //cambio sistema referencia de la camara al objeto
    if(camara_interna)
        N = &(_selected_object->display->M[0]);
    else
        N = &(_selected_camera->actual->M[0]);
    
    Eo[0] = M[0]*N[12] + M[4]*N[13] + M[8]*N[14] + M[12];
    Eo[1] = M[1]*N[12] + M[5]*N[13] + M[9]*N[14] + M[13];
    Eo[2] = M[2]*N[12] + M[6]*N[13] + M[10]*N[14] + M[14];

    //Ax+By+Cz+D=0
    eval = Eo[0]*Av + Eo[1]*Bv + Eo[2]*Cv + Dv;

    if(eval < 0.0)
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

    GLfloat vectorMaterial[3] = {0.75164,0.60648,0.22648};
    object3d *aux_obj = _first_object; //puntero al primer elemento de la lista de objetos.
    
    /* Clear the screen */
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    
    /* Define the projection */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if(_first_camera == 0)
        init_camera(); //we load the first camera with the identity

    if(_selected_camera->pers)
        glFrustum(_selected_camera->l, _selected_camera->r,_selected_camera->b,_selected_camera->t,_selected_camera->n,_selected_camera->f);
    else
        glOrtho(_selected_camera->l, _selected_camera->r,_selected_camera->b,_selected_camera->t,_selected_camera->n,_selected_camera->f);
    

   //glOrtho(_ortho_x_min, _ortho_x_max, _ortho_y_min, _ortho_y_max, _ortho_z_min, _ortho_z_max);
    /* Now we start drawing the object */
    glMatrixMode(GL_MODELVIEW);

    /*First, we draw the axes*/
    glLoadIdentity();
    draw_axes();

    if(_selected_object != 0)
    {
        if(camara_interna) //if camera mode is activated
        {
            glRotated(180.0, 0.0, 1.0, 0.0); //multiplicando primero por la matriz de rotación, giramos el mundo 180 grados, no el objeto, permitiendo que la matriz inversa siga trasladando todo correctamente.
            glMultMatrixd(_selected_object->display->inv_M);
        }
        else
            glLoadMatrixd(_selected_camera->actual->inv_M); //Cargar la matriz de la camara actual cuando funcione.
    }


    /* Parametrizamos las luces */
    /* TODO: tenemos que inicialitzar las cosas de cada luz, hacer una función fuera. */
    init_luces();

    int poligonos = 0;

    /*Now each of the objects in the list*/
    while (aux_obj != 0) { //dibuja mientras el puntero no apunte a null.

        glPushMatrix();

        //Select the color, depending on whether the current object is the selected one or not 
        if (aux_obj == _selected_object){
            glColor3f(KG_COL_SELECTED_R,KG_COL_SELECTED_G,KG_COL_SELECTED_B);
        }else{
            glColor3f(KG_COL_NONSELECTED_R,KG_COL_NONSELECTED_G,KG_COL_NONSELECTED_B);
        }
        
        // Por revisar, asignar con vectores los valores del material  
        
        
        /* Draw the object; for each face create a new polygon with the corresponding vertices */
        glMultMatrixd(aux_obj->display->M); //debemos cambiar mptr por display, dado que display necesita el puntero que apunta a la matriz actual del objeto.
        for (f = 0; f < aux_obj->num_faces; f++) {

            dibuja = poligono_visible(&(aux_obj->display->inv_M[0]), 
                                    aux_obj->face_table[f].vn[0], 
                                    aux_obj->face_table[f].vn[1], 
                                    aux_obj->face_table[f].vn[2], 
                                    aux_obj->face_table[f].ti);
            if(dibuja)
            {   
                if(!flat_smooth) //si en flat, tenemos en cuenta los vectores del plano
                    glNormal3dv(aux_obj->face_table[f].vn);
                
                glBegin(GL_POLYGON);

                for (v = 0; v < aux_obj->face_table[f].num_vertices; v++) {
                    v_index = aux_obj->face_table[f].vertex_table[v];
                    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE, aux_obj->vectorMaterial);
                    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.4);
                    if(flat_smooth) //sino, tenemos en cuenta los de los vértices.
                        glNormal3dv(aux_obj->vertex_table[v_index].normal);
                    
                    glVertex3d(aux_obj->vertex_table[v_index].coord.x,
                            aux_obj->vertex_table[v_index].coord.y,
                            aux_obj->vertex_table[v_index].coord.z);

                }
            
                poligonos++;
                
                glEnd();
            }

            //dibuja_normales(aux_obj, f); //aquesta funció dibuixa les normals de tots els poligons..
        }

        //printf("He dibujado %d poligonos de %d totales\n", poligonos, aux_obj->num_faces);
        aux_obj = aux_obj->next;

        glPopMatrix();
    }
    /*Do the actual drawing and paint*/
    glutSwapBuffers();
}

void dibuja_normales(object3d *aux_obj, GLint f)
{
    int primervertice;
    vertex ve;

    primervertice = aux_obj->face_table[f].vertex_table[0];
    ve = aux_obj->vertex_table[primervertice];
    //printf("%f %f %f\n",aux_obj->face_table[f].vn[0], aux_obj->face_table[f].vn[1], aux_obj->face_table[f].vn[2] );
    glBegin(GL_LINES);
        glVertex3d(ve.coord.x, ve.coord.y, ve.coord.z);
        glVertex3d(ve.coord.x+aux_obj->face_table[f].vn[0], 
                    ve.coord.y+aux_obj->face_table[f].vn[1], 
                    ve.coord.z+aux_obj->face_table[f].vn[2]);
    glEnd();
                
}
