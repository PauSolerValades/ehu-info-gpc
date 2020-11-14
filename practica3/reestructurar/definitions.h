#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <GL/gl.h>

/** DEFINITIONS **/

#define KG_WINDOW_TITLE                     "Practica GPC Pau Soler"
#define KG_WINDOW_WIDTH                     600
#define KG_WINDOW_HEIGHT                    400
#define KG_WINDOW_X                         50
#define KG_WINDOW_Y                         50

#define KG_MSSG_SELECT_FILE                 "Idatz ezazu fitxategiaren path-a: "
#define KG_MSSG_FILENOTFOUND                "Fitxategi hori ez da existitzen!!"
#define KG_MSSG_INVALIDFILE                 "Arazoren bat egon da fitxategiarekin ..."
#define KG_MSSG_EMPTYFILE                   "Fitxategia hutsik dago"
#define KG_MSSG_FILEREAD                    "Fitxategiaren irakurketa buruta"

#define KG_STEP_MOVE                        5.0f
#define KG_STEP_ROTATE                      10.0f
#define KG_STEP_ZOOM                        0.75
#define KG_STEP_CAMERA_ANGLE                5.0f

#define KG_ORTHO_X_MIN_INIT                -5
#define KG_ORTHO_X_MAX_INIT                 5
#define KG_ORTHO_Y_MIN_INIT                -5
#define KG_ORTHO_Y_MAX_INIT                 5
#define KG_ORTHO_Z_MIN_INIT                -100
#define KG_ORTHO_Z_MAX_INIT                 10000

#define KG_COL_BACK_R                       0.30f
#define KG_COL_BACK_G                       0.30f
#define KG_COL_BACK_B                       0.30f
#define KG_COL_BACK_A                       1.00f

#define KG_COL_SELECTED_R                   1.00f
#define KG_COL_SELECTED_G                   0.75f
#define KG_COL_SELECTED_B                   0.00f

#define KG_COL_NONSELECTED_R                1.00f
#define KG_COL_NONSELECTED_G                1.00f
#define KG_COL_NONSELECTED_B                1.00f

#define KG_COL_X_AXIS_R                     1.0f
#define KG_COL_X_AXIS_G                     0.0f
#define KG_COL_X_AXIS_B                     0.0f

#define KG_COL_Y_AXIS_R                     0.0f
#define KG_COL_Y_AXIS_G                     1.0f
#define KG_COL_Y_AXIS_B                     0.0f

#define KG_COL_Z_AXIS_R                     0.0f
#define KG_COL_Z_AXIS_G                     0.0f
#define KG_COL_Z_AXIS_B                     1.0f

#define KG_MAX_DOUBLE                       10E25

#define T					0.5
#define A					10.0
#define US					1.1
#define DS					0.9
#define INIT_CAMERA         5.0

/** STRUCTURES **/

/****************************
 * Structure to store the   *
 * coordinates of 3D points *
 ****************************/
typedef struct {
    GLdouble x, y, z;
} point3;

/*****************************
 * Structure to store the    *
 * coordinates of 3D vectors *
 *****************************/
typedef struct {
    GLdouble x, y, z;
} vector3;

/****************************
 * Structure to store the   *
 * colors in RGB mode       *
 ****************************/
typedef struct {
    GLfloat r, g, b;
} color3;

/****************************
 * Structure to store       *
 * objects' vertices         *
 ****************************/
typedef struct {
    point3 coord;                       /* coordinates,x, y, z */
    GLint num_faces;                    /* number of faces that share this vertex */
} vertex;

/****************************
 * Structure to store       *
 * objects' faces or        *
 * polygons                 *
 ****************************/
typedef struct {
    GLint num_vertices;                 /* number of vertices in the face */
    GLint *vertex_table;                /* table with the index of each vertex */
} face;

/****************************
 * Structure to store       *
 * transformation           *
 * matrixes                 *
 ****************************/

typedef struct elem_matrix
{
    double M[16]; 			/* Tabla M con 16 elementos */
    double inv_M[16];       /* Tabla con 16 elementos, es la matriz inversa de M */
    struct elem_matrix *nextptr;	/* puntero a la siguiente matriz*/	
} elem_matrix;				/* así el compliador reconoce elem_matrix como struct y no se tiene que poner struct elem_matrix cada vex*/

typedef struct camera
{
    double M[16];
    double M_inv[16];
    int type;  // 0 = vuelo 1 = analisis  
    struct camera *nextptr;
    //guardar el point3 del objeto al que apuntas?
} camera;

/****************************
 * Structure to store a     *
 * pile of 3D objects       *
 ****************************/
struct object3d{
    GLint num_vertices;		    /* number of vertices in the object*/
    vertex *vertex_table;		/* table of vertices */
    GLint num_faces;			/* number of faces in the object */
    face *face_table;			/* table of faces */
    point3 min;			        /* coordinates' lower bounds */
    point3 max;                 /* coordinates' bigger bounds */
    struct elem_matrix *display;
    struct elem_matrix *mptr;	/* next element in the pile of matrixes */
    struct object3d *next;		/* next element in the pile of objects */
};

typedef struct object3d object3d;

#endif //DEFINITIONS_H
