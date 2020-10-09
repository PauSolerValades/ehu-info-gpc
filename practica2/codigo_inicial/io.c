#include "definitions.h"
#include "load_obj.h"
#include <GL/glut.h>
#include <stdio.h>

extern object3d * _first_object;
extern object3d * _selected_object;

extern GLdouble _ortho_x_min,_ortho_x_max;
extern GLdouble _ortho_y_min,_ortho_y_max;
extern GLdouble _ortho_z_min,_ortho_z_max;

/**
 * @brief This function just prints information about the use
 * of the keys
 */
void print_help(){
    printf("KbG Irakasgaiaren Praktika. Programa honek 3D objektuak \n");
    printf("aldatzen eta bistaratzen ditu.  \n\n");
    printf("\n\n");
    printf("FUNTZIO NAGUSIAK \n");
    printf("<?>\t\t Laguntza hau bistaratu \n");
    printf("<ESC>\t\t Programatik irten \n");
    printf("<F>\t\t Objektua bat kargatu\n");
    printf("<TAB>\t\t Kargaturiko objektuen artean bat hautatu\n");
    printf("<DEL>\t\t Hautatutako objektua ezabatu\n");
    printf("<CTRL + ->\t Bistaratze-eremua handitu\n");
    printf("<CTRL + +>\t Bistaratze-eremua txikitu\n");
    printf("\n\n");
}

/**
 * @brief Callback function to control the basic keys
 * @param key Key that has been pressed
 * @param x X coordinate of the mouse pointer when the key was pressed
 * @param y Y coordinate of the mouse pointer when the key was pressed
 */
void keyboard(unsigned char key, int x, int y) {

    elem_matrix *mptr;
    int i;

    char* fname = malloc(sizeof (char)*128); /* Note that scanf adds a null character at the end of the vector*/
    int read = 0;
    object3d *auxiliar_object = 0;
    GLdouble wd,he,midx,midy;

    switch (key) {
    case 'f':
    case 'F': //carga un fichero.
        /*Ask for file*/
        printf("%s", KG_MSSG_SELECT_FILE);
        scanf("%s", fname);
        /*Allocate memory for the structure and read the file*/
        auxiliar_object = (object3d *) malloc(sizeof (object3d));
        read = read_wavefront(fname, auxiliar_object);
        switch (read) {
        /*Errors in the reading*/
        case 1:
            printf("%s: %s\n", fname, KG_MSSG_FILENOTFOUND);
            break;
        case 2:
            printf("%s: %s\n", fname, KG_MSSG_INVALIDFILE);
            break;
        case 3:
            printf("%s: %s\n", fname, KG_MSSG_EMPTYFILE);
            break;
        /*Read OK*/
        case 0:
            /*Insert the new object in the list*/
            auxiliar_object->next = _first_object;
            _first_object = auxiliar_object;
            _selected_object = _first_object;
 
            /* DONE: cargar matriz */
            mptr = (elem_matrix *) malloc(sizeof(elem_matrix)); //guardem espai per la matriu identitat
            mptr->nextptr = 0; //apuntem el seguent punter a 0

            //llenamos la tabla M
            for(i=1; i<15; i++)
            {
                mptr->M[i] = 0.0;
            }
            //cutríssim pero va
            mptr->M[0] = 1.0;
            mptr->M[5] = 1.0;
            mptr->M[10] = 1.0;
            mptr->M[15] = 1.0;

            //assignamos la matriz a la id.
            _selected_object->next_matrix = mptr;

            printf("%s\n",KG_MSSG_FILEREAD);
            break;
        }
        break;

    case 9: /* <TAB> */ //cambiar de objeto

        /* Controlar la lista, si és diferente de 0 que apunte al primero.*/
        if(_first_object !=0)
        {
            _selected_object = _selected_object->next;
            /*The selection is circular, thus if we move out of the list we go back to the first element*/
            if (_selected_object == 0) _selected_object = _first_object;
        }
        break;

    case 127: /* <SUPR> */ //borrar objeto

        /* si no hay objeto que borrar tiene que no borrar nada*/
        /*Erasing an object depends on whether it is the first one or not*/
        if(_first_object != 0){
            if (_selected_object == _first_object)
            {
                /*DONE el free de elem_matrix*/
                /*To remove the first object we just set the first as the current's next*/
                _first_object = _first_object->next;
                /*Once updated the pointer to the first object it is save to free the memory*/
                free(_selected_object->next_matrix);
                free(_selected_object);
                /*Finally, set the selected to the new first one*/
                _selected_object = _first_object;
            } else {
                /*In this case we need to get the previous element to the one we want to erase*/
                auxiliar_object = _first_object;
                while (auxiliar_object->next != _selected_object)
                    auxiliar_object = auxiliar_object->next;
                /*Now we bypass the element to erase*/
                auxiliar_object->next = _selected_object->next;
                /*free the memory*/
                free(_selected_object->next_matrix);
                free(_selected_object);
                /*and update the selection*/
                _selected_object = auxiliar_object;
            }
        }
        break;

    case '+': //hace que todo se vea mas pequeño

        if (glutGetModifiers() == GLUT_ACTIVE_CTRL){
            /*Increase the projection plane; compute the new dimensions*/
            wd=(_ortho_x_max-_ortho_x_min)/KG_STEP_ZOOM;
            he=(_ortho_y_max-_ortho_y_min)/KG_STEP_ZOOM;
            /*In order to avoid moving the center of the plane, we get its coordinates*/
            midx = (_ortho_x_max+_ortho_x_min)/2;
            midy = (_ortho_y_max+_ortho_y_min)/2;
            /*The the new limits are set, keeping the center of the plane*/
            _ortho_x_max = midx + wd/2;
            _ortho_x_min = midx - wd/2;
            _ortho_y_max = midy + he/2;
            _ortho_y_min = midy - he/2;
        }
        break;

    case '-': //hace que todo se vea más grande.

        if (glutGetModifiers() == GLUT_ACTIVE_CTRL){
            /*Increase the projection plane; compute the new dimensions*/
            wd=(_ortho_x_max-_ortho_x_min)*KG_STEP_ZOOM;
            he=(_ortho_y_max-_ortho_y_min)*KG_STEP_ZOOM;
            /*In order to avoid moving the center of the plane, we get its coordinates*/
            midx = (_ortho_x_max+_ortho_x_min)/2;
            midy = (_ortho_y_max+_ortho_y_min)/2;
            /*The the new limits are set, keeping the center of the plane*/
            _ortho_x_max = midx + wd/2;
            _ortho_x_min = midx - wd/2;
            _ortho_y_max = midy + he/2;
            _ortho_y_min = midy - he/2;
        }
        break;

    case 'm':
    case 'M': /* Activar Translación */
    	printf("Translación\n");
    	break;
    
    case 'b':
    case 'B': /* Rotación */
    	printf("Rotación\n");
    	break;
    
    case 't':
    case 'T': /* Escalado */
    	printf("Escalado\n");
    	break;
    
    case 'g':
    case 'G': /* Transformaciones ref mundo */
    	printf("Mundo\n");
    	break;
    	
    case 'l':
    case 'L': /* Transformaciones ref objetos */
    	printf("Objetos");
    	break;
    	
    case 'o':
    case 'O': /* Sistema referencia objeto */
    	printf("Objeto\n");
    	break;
    	
    case 'k':
    case 'K': /* Transformaciones camara actual */
    	printf("Transformaciones Camara Actual\n");
    	break;
    	
    case 'a':
    case 'A': /* Transformaciones luz selecionada */
    	printf("Transformaciones luz actual\n");
    	
    case '?':
        print_help();
        break;
        
    case 26: /* CONTROL+Z */
    	printf("Deshacer\n");
    	break;
    	
    case 27: /* <ESC> */
        exit(0);
        break;

    default:
        /*In the default case we just print the code of the key. This is usefull to define new cases*/
        printf("%d %c\n", key, key);
    }
    /*In case we have do any modification affecting the displaying of the object, we redraw them*/
    glutPostRedisplay();
}

/**
 * @brief Callback function to control the special keys
 * @param key Key that has been pressed
 * @param x X coordinate of the mouse pointer when the key was pressed
 * @param y Y coordinate of the mouse pointer when the key was pressed
 */
void special(int key, int x, int y)
{

    /* Suposo que això s'implementarà amb booleans globals. Si esta activat un, els altres no i podrem posar un if o un altre switch amb un char o una variable més complexa. De moment no toca */
    switch (key)
    {
    	case 100: /* LEFT ARROW */
    		printf("TECLA ESQUERRA\n");
    		break;
    		
    	case 101: /* UP ARROW */
    		printf("TECLA AMUNT\n");
    		break;
    		
    	case 102: /* RIGHT ARROW */
    		printf("TECLA DRETA\n");
    		break;
    		
    	case 103: /* DOWN ARROW */
    		printf("TECLA AVALL\n");
    		break;
    		
    	case 104: /* REPAG */
    		printf("TECLA REPAG\n");
    		break;
    		
    	case 105: /* AVPAG */
    		printf("TECLA AVPAG\n");
    		break;
    		
   	default:
   		printf("%d \n", key);
    }
}

