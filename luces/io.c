#include "definitions.h"
#include "load_obj.h"
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

extern object3d *_first_object;
extern object3d *_selected_object;
extern camera *_first_camera;
extern camera *_selected_camera;

extern GLdouble _ortho_x_min, _ortho_x_max;
extern GLdouble _ortho_y_min, _ortho_y_max;
extern GLdouble _ortho_z_min, _ortho_z_max;

extern int mode; //0 objeto, 1: camara
extern int transformacion; 
//0: translación, 1: rotación, 2: escalado cuando mode = 0
//0: translacion, 2: rotación, 3: volumen de visión
extern int referencia; //00: objeto, 01: mundo;
extern int camara_interna; //0: Desactivada, 1: Activada

/* all the functions declared to improve the order of aperance */
void print_help();
void print_enonmode();
void print_eworld();
void print_enonobject();
void destructor_objeto(object3d *object);
void destructor_camara(camera *camera);
void new_transformation();
void new_camera_transformation();
void inverse();
void apuntar_objeto();
void calcular_normales();
void special(int k, int x, int y);
void keyboard(unsigned char key, int x, int y);
double euclidean_norm(double x, double y, double z);
void cross_product(double *u, double *v, double *w);
void keyboard_object(unsigned char key, int x, int y);
void keyboard_camera(unsigned char key, int x, int y);
void switch_transformaciones_analisis(int k, int *isAKey);
void switch_transformaciones(int k, int *isAKey);

/**
 * @brief Callback function to control the basic keys
 * @param key Key that has been pressed
 * @param x X coordinate of the mouse pointer when the key was pressed
 * @param y Y coordinate of the mouse pointer when the key was pressed
 */
void keyboard(unsigned char key, int x, int y)
{

	elem_matrix *new_mptr;
	int i;

	char *fname = malloc(sizeof(char) * 128); /* Note that scanf adds a null character at the end of the vector*/
	int read = 0;
	object3d *auxiliar_object = 0;
	camera *auxiliar_camera = 0;

	switch (key)
	{
	case 'f':
	case 'F': //carga un fichero.
		/*Ask for file*/
		printf("%s", KG_MSSG_SELECT_FILE);
		scanf("%s", fname);
		/*Allocate memory for the structure and read the file*/
		auxiliar_object = (object3d *)malloc(sizeof(object3d));
		read = read_wavefront(fname, auxiliar_object);

		switch (read)
		{
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

			/* Loading the firss matrix */
			new_mptr = (elem_matrix *)malloc(sizeof(elem_matrix)); //guardem espai per la matriu identitat
			new_mptr->nextptr = NULL; //apuntem el seguent punter a 0

			//llenamos la tabla M
			for (i = 1; i < 15; i++)
			{
				new_mptr->M[i] = 0.0;
				new_mptr->inv_M[i] = 0.0;
			}
			//cutríssim pero va
			new_mptr->M[0] = 1.0;
			new_mptr->M[5] = 1.0;
			new_mptr->M[10] = 1.0;
			new_mptr->M[15] = 1.0;

			new_mptr->inv_M[0] = 1.0;
			new_mptr->inv_M[5] = 1.0;
			new_mptr->inv_M[10] = 1.0;
			new_mptr->inv_M[15] = 1.0;

			//asignamos la matriz a la id.
			_selected_object->mptr = new_mptr;

			//asignamos la variable global
			_selected_object->display = new_mptr;

			calcular_normales();
			printf("%s\n", KG_MSSG_FILEREAD);
			break;
		}
		break;

	case GLUT_KEY_F1:
		printf("JON PUTO ADICTO AL MOVIL COLLONS\n");
		break;

	case 9: /* <TAB> */ //cambiar de objeto

		/* Controlar la lista, si és diferente de 0 que apunte al primero.*/
		if (_first_object != 0)
		{
			_selected_object = _selected_object->next;
			/*The selection is circular, thus if we move out of the list we go back to the first element*/
			if (_selected_object == 0)
				_selected_object = _first_object;

			if(mode && transformacion == 0) //TODO: NO RECORDO QUÈ FA AIXÒ.
				apuntar_objeto();
			
		}
		break;

	case 8: /* <BACKSPACE> */

		if(_selected_camera != _first_camera)
		{
			auxiliar_camera = _first_camera;
			while (auxiliar_camera->nextptr != _selected_camera)
				auxiliar_camera = auxiliar_camera->nextptr;
			/*Now we bypass the element to erase*/
			auxiliar_camera->nextptr = _selected_camera->nextptr;
			/* Free all the memory object*/
			destructor_camara(_selected_camera);

			/*and update the selection*/
			_selected_camera = auxiliar_camera;
		}
		else
		{
			printf("No puedes borrar la primera cámara\n");
		}
		break;

	case 127: /* <SUPR> */ //borrar objeto

		/*Erasing an object depends on whether it is the first one or not*/
		if (_first_object != 0)
		{
			if (_selected_object == _first_object)
			{
				/*DONE el free de elem_matrix*/
				/*To remove the first object we just set the first as the current's next*/
				_first_object = _first_object->next;
				/*Once updated the pointer to the first object it is save to free the memory*/
				/* Free all the memory object */
				destructor_objeto(_selected_object);			

				/*Finally, set the selected to the new first one*/
				_selected_object = _first_object;
			}
			else
			{
				/*In this case we need to get the previous element to the one we want to erase*/
				auxiliar_object = _first_object;
				while (auxiliar_object->next != _selected_object)
					auxiliar_object = auxiliar_object->next;
				/*Now we bypass the element to erase*/
				auxiliar_object->next = _selected_object->next;
				/* Free all the memory object*/
				destructor_objeto(_selected_object);

				/*and update the selection*/
				_selected_object = auxiliar_object;
			}
		}
		break;

	case 'o':
	case 'O': /* Sistema referencia objeto */
		printf("Objeto\n");
		break;

	case 'c': /* cambia a la siguiente camara */
		if (_first_camera != 0)
		{
			_selected_camera = _selected_camera->nextptr;
			/*The selection is circular, thus if we move out of the list we go back to the first element*/
			if (_selected_camera == 0)
				_selected_camera = _first_camera;
			if(_selected_camera->type)
				apuntar_objeto();
			printf("Siguiente camara\n");
		}

		break;

	case 'C': /* Activa/desactiva la camara interna del objeto */
		printf("CAMBIO A MODO OBJETO\n");

		if(camara_interna)
		{
			printf("Camara No Interna\n");	
			camara_interna = 0;
		}
		else
		{
			printf("Camara Interna\n");
			camara_interna = 1;
			mode = 0; //volvemos en modo transformación.
			referencia = 0; /* No tiene sntido transformar la cámara cuando estás desde la perspectiva del objeto. */
		}
		break;

	case 'k': /* ACTIVA EL MODO CÁMARA */
	case 'K': /* Transformaciones camara actual */
		if(mode){
			mode = 0;
			printf("TRANSFORMACIONES DE OBJETOS\n");
		}
		else
		{
			mode = 1;
			printf("TRANSFORMACIONES DE CAMARAS\n");
		}
		
		break;

	case 'a':
	case 'A': /* Transformaciones luz selecionada */
		printf("Funcionalidad no implementada\n");
		break;
	case '?':
		print_help();
		break;

	case 27: /* <ESC> */
		exit(0);
		break;

	default:
		/* In default we call the other two functions. If the program has reached this line a key with two functionalities has been pressed */
		
		if(mode)
			keyboard_camera(key, x, y);
		else
			keyboard_object(key, x, y);
		
        break;
	}

	free(fname); /* We have to free the memory used in the scanf */
	glutPostRedisplay();
}



void keyboard_object(unsigned char key, int x, int y)
{

	GLdouble wd, he, midx, midy;

	switch (key)
	{
		case 'm':
		case 'M': /* Translación */
			transformacion = 0;
			printf("Translaciones ACTIVADAS\n");
			break;
		
		case 'b':
		case 'B': /* Rotación */
			transformacion = 1;
			printf("Rotaciones OBJECTO ACTIVADAS\n");
			break;

		case 't':
		case 'T': /* Escalado */
			transformacion = 2;
			printf("Escalado OBJETO ACTIVADO\n");
			break;

		case 'l':
		case 'L': /* Transformaciones ref objetos */
			referencia = 0;
			printf("Referencia OBJETO\n");
			break;

		case 'g':
		case 'G': /* Transformaciones ref mundo */
			if(!camara_interna)
			{
				
				referencia = 1;
				printf("Referencia MUNDO\n");
			}
			else
			{
				printf("No puedes activar la referencia de MUNDO cuando la cámara interna de un objeto está activada");
			}
			break;
		
		//cambia los límites de la camara
		case '-': //hace que todo se vea mas pequeño

			if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
			{
				/*Increase the projection plane; compute the new dimensions*/
				wd = (_selected_camera->r - _selected_camera->l) / KG_STEP_ZOOM;
				he = (_selected_camera->t - _selected_camera->b) / KG_STEP_ZOOM;
				/*In order to avoid moving the center of the plane, we get its coordinates*/
				midx = (_selected_camera->r + _selected_camera->l) / 2;
				midy = (_selected_camera->t + _selected_camera->b) / 2;
				/*The the new limits are set, keeping the center of the plane*/
				_selected_camera->r = midx + wd / 2;
				_selected_camera->l = midx - wd / 2;
				_selected_camera->t = midy + he / 2;
				_selected_camera->b = midy - he / 2;
			}
			else /* If control wasn't pressed, we have to call the special keys to attend the petition. This maybe isn't the most correct way, but it's very clear, sort and easy to undersand. */
				special(45, x, y);
			break;

		case '+': //hace que todo se vea más grande.

			if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
			{
				/*Increase the projection plane; compute the new dimensions*/
				wd = (_selected_camera->r - _selected_camera->l) * KG_STEP_ZOOM;
				he = (_selected_camera->t - _selected_camera->b) * KG_STEP_ZOOM;
				/*In order to avoid moving the center of the plane, we get its coordinates*/
				midx = (_selected_camera->r + _selected_camera->l) / 2;
				midy = (_selected_camera->t + _selected_camera->b) / 2;
				/*The the new limits are set, keeping the center of the plane*/
				_selected_camera->r = midx + wd / 2;
				_selected_camera->l = midx - wd / 2;
				_selected_camera->t = midy + he / 2;
				_selected_camera->b = midy - he / 2;
			}
			else /* Analogous of case '-': */
				special(43, x, y);
			break;

		case 25:
        
			if(_selected_object != 0)
			{
				if(_selected_object->display != _selected_object->mptr)
				{
					printf("Rehacer\n");
					
					elem_matrix *iter;
					elem_matrix *ant;

					iter = _selected_object->mptr->nextptr;
					ant = _selected_object->mptr;
					
					
					while(1)
					{

						if(_selected_object->display==iter)
							break;

						ant = iter;
						iter = iter->nextptr;
						
					}
					
					_selected_object->display = ant;
					}
				else
				{
					printf("No más redo\n");
				}
				glutPostRedisplay();

			}else{
				printf("Cargue un objecto, por favor\n");        
			}

			break;

		case 26: /* CONTROL+Z */

			if(_selected_object != 0)
			{
				if (_selected_object->display->nextptr != 0)
				{
					printf("Deshacer\n");
						
					_selected_object->display = _selected_object->display->nextptr;
				
				}
				else
				{
					printf("No más undo\n");
				}

				glutPostRedisplay();
			}
			else
			{
				printf("Cargue un objeto, por favor\n");            
			}
			break;

		default:
			printf("%d %c\n", key, x);
			break;
	}
}




void keyboard_camera(unsigned char key, int x, int y)
{
	GLdouble wd, he, midx, midy;

	switch (key)
	{
		case 'n': //crea una nova camara mode projecció
			printf("Nueva Camara Proyeccion\n");
			
			//la primera camara siempre está inicializada => el puntero nunca será null
			if(mode)
			{

				glMatrixMode(GL_PROJECTION);
    			glLoadIdentity();
				
				elem_matrix *matrix_camera;
				camera *new_camera;

				new_camera = (camera *)malloc(sizeof(camera));
				new_camera->nextptr = NULL;//apuntem el seguent punter a 0

				matrix_camera = (elem_matrix *)malloc(sizeof(elem_matrix));
				matrix_camera->nextptr = NULL;

				glGetDoublev(GL_PROJECTION_MATRIX, matrix_camera->M);
    			glGetDoublev(GL_PROJECTION_MATRIX, matrix_camera->inv_M);

    			matrix_camera->M[14] = INIT_CAMERA;
    			matrix_camera->inv_M[14] = -INIT_CAMERA;

				new_camera->type = 0;
				new_camera->pers = 1;

				new_camera-> r = 0.1;
				new_camera-> l = -0.1;
				new_camera-> t = 0.1;
				new_camera-> b = -0.1;
				new_camera-> n = 0.1;
				new_camera-> f = 1000.0;

				new_camera->first = matrix_camera;
				new_camera->actual = matrix_camera;

				_selected_camera->nextptr = new_camera;
				_selected_camera = new_camera;

			}else
				printf("No está en el modo camera, para entrar en el pulse k\n");
			
			break;
		case 'N':
			printf("Nueva Camara Paralela\n");
			
			//la primera camara siempre está inicializada => el puntero nunca será null
			if(mode)
			{

				glMatrixMode(GL_PROJECTION);
    			glLoadIdentity();

				elem_matrix *matrix_camera;
				camera *new_camera;

				new_camera = (camera *)malloc(sizeof(camera));
				new_camera->nextptr = NULL;//apuntem el seguent punter a 0

				matrix_camera = (elem_matrix *)malloc(sizeof(elem_matrix));
				matrix_camera->nextptr = NULL;

				glGetDoublev(GL_PROJECTION_MATRIX, matrix_camera->M);
    			glGetDoublev(GL_PROJECTION_MATRIX, matrix_camera->inv_M);

    			matrix_camera->M[14] = INIT_CAMERA;
    			matrix_camera->inv_M[14] = -INIT_CAMERA;

				new_camera->type = 0;
				new_camera->pers = 0;

				new_camera-> r = 5.0;
				new_camera-> l = -5.0;
				new_camera-> t = 5.0;
				new_camera-> b = -5.0;
				new_camera-> n = 0.0;
				new_camera-> f = 1000.0;

				new_camera->first = matrix_camera;
				new_camera->actual = matrix_camera;

				_selected_camera->nextptr = new_camera;
				_selected_camera = new_camera;

			}else
				printf("No está en el modo camera, para entrar en el pulse k\n");
			
			break;

		case 'b':
		case 'B': /* Rotación */
			transformacion = 1;
			printf("Rotaciones CAMARA ACTIVADAS\n");
			break;
		
		case 'm':
		case 'M': /* Translaciprint_maón */
			transformacion = 0;
			printf("Translaciones CAMARA ACTIVADAS\n");
			break;
		
		case 't':
		case 'T': /* Escalado */
			transformacion = 2;
			printf("Escalado CAMARA ACTIVADO\n");
			break;

		case 'g':
		case 'G':	
			printf("Camara Análisi\n");
			referencia = 0; //cambiamos a modo objeto porque no tiene sentido el global.
			transformacion = 0;
			_selected_camera->type = 1;
			apuntar_objeto();
			
			//print_matrix(_selected_camera->M);
			break;

		case 'l':
		case 'L':
			_selected_camera->type = 0;
			printf("Camara Modo Vuelo\n");
			break;

		case '+':
			/*Increase the projection plane; compute the new dimensions*/
			wd = (_selected_camera->r - _selected_camera->l) * KG_STEP_ZOOM;
			he = (_selected_camera->t - _selected_camera->b) * KG_STEP_ZOOM;
			/*In order to avoid moving the center of the plane, we get its coordinates*/
			midx = (_selected_camera->r + _selected_camera->l) / 2;
			midy = (_selected_camera->t + _selected_camera->b) / 2;
			/*The the new limits are set, keeping the center of the plane*/
			_selected_camera->r = midx + wd / 2;
			_selected_camera->l = midx - wd / 2;
			_selected_camera->t = midy + he / 2;
			_selected_camera->b = midy - he / 2;
			break;

		case '-':
			wd = (_selected_camera->r - _selected_camera->l) / KG_STEP_ZOOM;
			he = (_selected_camera->t - _selected_camera->b) / KG_STEP_ZOOM;
			/*In order to avoid moving the center of the plane, we get its coordinates*/
			midx = (_selected_camera->r + _selected_camera->l) / 2;
			midy = (_selected_camera->t + _selected_camera->b) / 2;
			/*The the new limits are set, keeping the center of the plane*/
			_selected_camera->r = midx + wd / 2;
			_selected_camera->l = midx - wd / 2;
			_selected_camera->t = midy + he / 2;
			_selected_camera->b = midy - he / 2;
			break;

		case 25:
        
			if(_selected_camera != 0)
			{
				if(_selected_camera->actual != _selected_camera->first)
				{
					printf("Rehacer\n");
					
					elem_matrix *iter;
					elem_matrix *ant;

					iter = _selected_camera->first->nextptr;
					ant = _selected_camera->first;
					
					
					while(1)
					{

						if(_selected_camera->actual==iter)
							break;

						ant = iter;
						iter = iter->nextptr;
						
					}
					
					_selected_camera->actual = ant;
					}
				else
				{
					printf("No más redo\n");
				}
				glutPostRedisplay();

			}else{
				printf("Camara, por favor\n");        
			}

			break;

		case 26: /* CONTROL+Z */

			if(_selected_camera != 0)
			{
				if (_selected_camera->actual->nextptr != 0)
				{
					printf("Deshacer\n");
						
					_selected_camera->actual = _selected_camera->actual->nextptr;
				
				}
				else
				{
					printf("No más undo\n");
				}

				glutPostRedisplay();
			}
			else
			{
				printf("Camara, por favor\n");            
			}
			break;

		default:
			printf("%d %c\n", key, x);
			break;

	}	
}


/**
 * @brief Callback function to control the special keys
 * @param key Key that has been pressed
 * @param x X coordinate of the mouse pointer when the key was pressed
 * @param y Y coordinate of the mouse pointer when the key was pressed
 * 
 * Heart of the application, calls gl to apply the scaling, rotating and translating transformations. Multiplies the matrix on the rigth if referencia = 0 and on left otherwise.
 */
void special(int k, int x, int y)
{
    int isAKey;

    isAKey = 0;
    
    if(_selected_object != NULL)
    {	

		glMatrixMode(GL_MODELVIEW);

		if(mode) //todo modo camara
		{
			if(_selected_camera->type == 0)
			{
				glLoadMatrixd(_selected_camera->actual->M);
				switch_transformaciones(k, &isAKey);
			}
			else
				switch_transformaciones_analisis(k, &isAKey);

			if(!isAKey)
				new_camera_transformation();
			
		}
		else //todo modo objeto
		{
			if(!referencia)
				glLoadMatrixd(_selected_object->display->M);
			else
				glLoadIdentity();

			switch_transformaciones(k, &isAKey); //aquí hi ha el switch on es multiplica tot.

			if(referencia)
				glMultMatrixd(_selected_object->display->M);
			
			if(!isAKey)
				new_transformation(); //crea el nou elem_matrix buit i el posa a la llista
		}

		glutPostRedisplay();
	}
}



void switch_transformaciones_analisis(int k, int *isAKey)
{
	int i;
	double z[3], P[3], x[3], y[3];

	glLoadIdentity();

	for(i=0; i<3; i++)
	{
		P[i] = _selected_object->display->M[i+12];
		x[i] = _selected_camera->actual->M[i];
		y[i] = _selected_camera->actual->M[i+4];
		z[i] = _selected_camera->actual->M[i+8];
	}

	glTranslated(P[0], P[1], P[2]);
	
	switch (k)
	{
	case GLUT_KEY_UP:
		glRotated(-A, x[0], x[1], x[2]);
		break;
	case GLUT_KEY_DOWN:
		glRotated(A, x[0], x[1], x[2]);
		break;
	case GLUT_KEY_RIGHT:
		glRotated(A, y[0], y[1], y[2]);
		break;
	case GLUT_KEY_LEFT:
		glRotated(-A, y[0], y[1], y[2]);
		break;
	case GLUT_KEY_PAGE_UP:
		glRotated(A, 0.0, 0.0, 1.0);
		break;
	case GLUT_KEY_PAGE_DOWN:
		glRotated(-A, 0.0, 0.0, 1.0);
		break;
	case 43:
		glTranslated(-z[0]*T, -z[1]*T, -z[2]*T);
		break;
	case 45:
		glTranslated(z[0]*T, z[1]*T, z[2]*T);
		break;
	default:
		*isAKey = 1;
		break;
	}

	glTranslated(-P[0], -P[1], -P[2]);

	glMultMatrixd(_selected_camera->actual->M);
}


void switch_transformaciones(int k, int *isAKey)
{
	switch (transformacion)
	{
	case 0:
		switch (k)
		{
			case GLUT_KEY_UP:
				glTranslated(0.0, T, 0.0);
				break;
			case GLUT_KEY_DOWN:
				glTranslated(0.0, -T, 0.0);
				break;
			case GLUT_KEY_RIGHT:
				glTranslated(T, 0.0, 0.0);
				break;
			case GLUT_KEY_LEFT:
				glTranslated(-T, 0.0, 0.0);
				break;
			case GLUT_KEY_PAGE_UP:
				glTranslated(0.0, 0.0, T);
				break;
			case GLUT_KEY_PAGE_DOWN:
				glTranslated(0.0, 0.0, -T);
				break;
			default:
				*isAKey = 1;
				break;
			}
		break;
	case 1:
		switch (k)
		{
			case GLUT_KEY_UP :
				glRotated(A, -1.0, 0.0, 0.0);
				break;
			case GLUT_KEY_DOWN:
				glRotated(A, 1.0, 0.0, 0.0);
				break;
			case GLUT_KEY_RIGHT :
				glRotated(A, 0.0, 1.0, 0.0);
				break;
			case GLUT_KEY_LEFT :
				glRotated(A, 0.0, -1.0, 0.0);
				break;
			case GLUT_KEY_PAGE_UP:
				glRotated(A, 0.0, 0.0, 1.0);
				break;
			case GLUT_KEY_PAGE_DOWN:
				glRotated(A, 0.0, 0.0, -1.0);
				break;
			default:
				*isAKey = 1;
				break;
		}
		break;

	case 2:
		switch (k)
		{
			case GLUT_KEY_UP :
				glScaled(1.0, US, 1.0);
				break;
			case GLUT_KEY_DOWN:
				glScaled(1.0, DS, 1.0);
				break;
			case GLUT_KEY_RIGHT :
				glScaled(US, 1.0, 1.0);
				break;
			case GLUT_KEY_LEFT :
				glScaled(DS, 1.0, 1.0);
				break;
			case GLUT_KEY_PAGE_UP:
				glScaled(1.0, 1.0, US);
				break;
			case GLUT_KEY_PAGE_DOWN:
				glScaled(1.0, 1.0, DS);
				break;
			case 43:
				glScaled(DS, DS, DS);
				break;
			case 45:
				glScaled(US, US, US);
				break;
			default:
				*isAKey = 1;
				break;
			}
		break;

	default:
		print_enonmode();
		break;
	}
}



void new_camera_transformation()
{
	elem_matrix *new_mptr, *aux;
	
	new_mptr = (elem_matrix *)malloc(sizeof(elem_matrix));

	while(_selected_camera->first != _selected_camera->actual)
	{
		aux = _selected_camera->first;
		_selected_camera->first = _selected_camera->first->nextptr;
		free(aux);
	}

	_selected_camera->first = new_mptr;
	new_mptr->nextptr = _selected_camera->actual;
	_selected_camera->actual = new_mptr;

	glGetDoublev(GL_MODELVIEW_MATRIX, _selected_camera->actual->M);
	inverse(_selected_camera->actual->M, _selected_camera->actual->inv_M);

}


/**
new_transformation() allocates memory for a new element matrix, adds it to the first element in the linked list matrixes of object3d and deletes all the matrixes that cannot be used again due to redo and undo functions.
1
*/
void new_transformation()
{
	int control;
	elem_matrix *new_mptr, *aux;

	new_mptr = (elem_matrix *)malloc(sizeof(elem_matrix));
	
	control = 1;
	
	while(_selected_object->mptr != _selected_object->display)
	{
		aux = _selected_object->mptr;
		_selected_object->mptr = _selected_object->mptr->nextptr;
		free(aux);
		
		//printf("Borro %d\n", control);
		control++;
	}	
	

	_selected_object->mptr = new_mptr;
	new_mptr->nextptr = _selected_object->display;
	_selected_object->display = new_mptr;

    glGetDoublev(GL_MODELVIEW_MATRIX, _selected_object->display->M);

	inverse(_selected_object->display->M, _selected_object->display->inv_M); //calcula i carrega la matriu inversa (transposada vamos.)

}


void calcular_normales()
{
	vertex a, b, c;
	int i, j, ia, ib, ic, indice;
	double v1[3], v2[3], vn[3], module_vn;

	/* Encontrando la ecuación del plano */
	for(i = 0; i<_selected_object->num_faces; i++)
	{
		//esto son los indices de los vertices del i-essimo poligono en object3d vertex_table
		ia = _selected_object->face_table[i].vertex_table[0];
		ib = _selected_object->face_table[i].vertex_table[1];
		ic = _selected_object->face_table[i].vertex_table[2];

		//accedemos a los puntos del i-essimo poligono
		a = _selected_object->vertex_table[ia];
		b = _selected_object->vertex_table[ib];
		c = _selected_object->vertex_table[ic];

		//printf("%f %f %f\n", a.coord.x, a.coord.y, a.coord.z);

		//calculamos los dos vectores
		v1[0] = b.coord.x - a.coord.x;
		v1[1] = b.coord.y - a.coord.y;
		v1[2] = b.coord.z - a.coord.z;

		v2[0] = c.coord.x - a.coord.x;
		v2[1] = c.coord.y - a.coord.y;
		v2[2] = c.coord.z - a.coord.z;

		//producto vectorial entre v1 i
		vn[0] = v1[1] * v2[2] - v2[1] * v1[2];
		vn[1] = -(v1[0] * v2[2]) + (v2[0] * v1[2]);
		vn[2] = v1[0] * v2[1] - v2[0] * v1[1];

		module_vn = euclidean_norm(vn[0], vn[1], vn[2]);

		vn[0] = vn[0]/module_vn;
		vn[1] = vn[1]/module_vn;
		vn[2] = vn[2]/module_vn;

		_selected_object->face_table[i].vn[0] = vn[0];
		_selected_object->face_table[i].vn[1] = vn[1];
		_selected_object->face_table[i].vn[2] = vn[2];

		//Ax+By+Cz+D=0 => D=-(Ax+By+Cz) 
		_selected_object->face_table[i].ti = -(vn[0]*a.coord.x + vn[1]*a.coord.y + vn[2]*a.coord.z);

		//printf("%fx + %fy + %fz + %f\n",vn[0], vn[1], vn[2], _selected_object->face_table[i].ti);

		for(j = 0; j<_selected_object->face_table->num_vertices; j++)
		{
			indice = _selected_object->face_table[i].vertex_table[j];
			_selected_object->vertex_table[indice].normal.x += vn[0];
			_selected_object->vertex_table[indice].normal.y += vn[1];
			_selected_object->vertex_table[indice].normal.z += vn[2];
		}
	}

	
	for(i = 0; i<_selected_object->num_vertices; i++)
	{
		double v[3], module;

		v[0] = _selected_object->vertex_table[i].normal.x;
		v[1] = _selected_object->vertex_table[i].normal.y;
		v[2] = _selected_object->vertex_table[i].normal.z;

		module = euclidean_norm(v[0], v[1], v[2]);

		_selected_object->vertex_table[i].normal.x = v[0]/module;
		_selected_object->vertex_table[i].normal.y = v[1]/module;
		_selected_object->vertex_table[i].normal.z = v[2]/module;

	}
}

void inverse(double *b, double *a)
{
	GLdouble x,y,z;

    a[0] = b[0];
    a[5] = b[5];
    a[10] = b[10];
    x = b[1]; a[1] = b[4]; a[4] = x;
    x = b[2]; a[2] = b[8]; a[8] = x;
    x = b[6]; a[6] = b[9]; a[9] = x;

    a[3] = b[3];
    a[7] = b[7];
    a[11] = b[11];
    a[15] = b[15];
    
    x = (a[0]*b[12])+(a[4]*b[13])+(a[8]*b[14]);
    y = (a[1]*b[12])+(a[5]*b[13])+(a[9]*b[14]);
    z = (a[2]*b[12])+(a[6]*b[13])+(a[10]*b[14]);
    a[12] = -x;
    a[13] = -y;
    a[14] = -z;

}

double euclidean_norm(double x, double y, double z) { return sqrt(x*x + y*y + z*z); }

/* Centers the camera in the object owo. */
void apuntar_objeto()
{
	int i, centrado_mismo_punto;
	double module_z, module_x;

	double E[3], P[3], U[3], x_c[3], y_c[3], z_c[3];

	centrado_mismo_punto = 0;

	for(i = 0; i<3; i++){
		E[i] = _selected_camera->actual->M[12+i];
		P[i] = _selected_object->display->M[12+i];
		U[i] = _selected_camera->actual->M[i+4];

		if(E[i]==P[i]){
			centrado_mismo_punto++;
		}
	}

	if(centrado_mismo_punto != 3)
	{
		z_c[0] = (E[0]-P[0]);
		z_c[1] = (E[1]-P[1]);
		z_c[2] = (E[2]-P[2]);

		module_z = euclidean_norm(z_c[0], z_c[1], z_c[2]);
		
		//printf("Modulo z: %f\n", module_z);

		z_c[0] = z_c[0]/module_z;
		z_c[1] = z_c[1]/module_z;
		z_c[2] = z_c[2]/module_z;
	

		//cross product with z_c with u-> camera vector. (y from de camera.)
		
		x_c[0] = U[1] * z_c[2] - z_c[1] * U[2];
		x_c[1] = -(U[0] * z_c[2] - z_c[0] * U[2]);
		x_c[2] = U[0] * z_c[1] - z_c[0] * U[1];
		
		module_x = euclidean_norm(x_c[0], x_c[1], x_c[2]);

		x_c[0] = x_c[0]/module_x;
		x_c[1] = x_c[1]/module_x;
		x_c[2] = x_c[2]/module_x;

		y_c[0] = z_c[1] * x_c[2] - x_c[1] * z_c[2];
		y_c[1] = -(z_c[0] * x_c[2] - x_c[0] * z_c[2]);
		y_c[2] = z_c[0] * x_c[1] - x_c[0] * z_c[1];

		for(i = 0; i<3; i++)
		{
			_selected_camera->actual->M[i] = x_c[i];
			_selected_camera->actual->M[4+i] = y_c[i];
			_selected_camera->actual->M[i+8] = z_c[i];
			_selected_camera->actual->M[i+12] = E[i];
		}

		inverse(_selected_camera->actual->M, _selected_camera->actual->inv_M);
	}
}

void destructor_camara(camera *camera)
{
	elem_matrix *aux;

	while(camera->first != 0)
	{
		aux = camera->first;
		camera->first = camera->first->nextptr;
		free(aux);
	}

	free(camera);
}


/**
Called in case supr, this function frees al the dynamic memory used in a object3d:
	- vertex_table
	- face_tables and all the vertex_table inside it
	- linked list of elem_matrix
*/
void destructor_objeto(object3d *object)
{
	int i;
	elem_matrix *aux;

	/*we free the vertex table*/
	free(object->vertex_table);

	/* we free every vertex table of all the positions in face_table */
	for (i = 0; i < object->num_faces; i++)
	{
		free(object->face_table[i].vertex_table);
	}
	/* we free the face_table */
	free(object->face_table);

	/* we free the linked list */
	while (object->mptr != 0)
	{
		aux = object->mptr;
		object->mptr = object->mptr->nextptr;
		free(aux);
	}
	
	/* finally, we delete the object itself */
	free(object);
}

/**
 * @brief This function just prints information about the use
 * of the keys
 */
void print_help()
{
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
	printf("<M> \t\t Mode translation\n");
	printf("<B> \t\t Mode rotation\n");
	printf("<T> \t\t Mode scaling\n");
	printf("<L> \t\t Object Reference\n");
	printf("<G> \t\t Global Reference\n");
	printf("<+/- in T> \t Scale up/down all the object axis\n\n");
	
	printf("Use the UP, DOWN, LEFT, RIGHT, AV_PAG, RE_PAG keys to apply the transformations M, B, T to the object selected respect himself (object reference) or respect to the axis (global reference)\n");
	printf("\n\n");
}

/* Called if a transformation has not been selected */
void print_enonmode()
{
	printf("No hay ninguna transformación selecionada. Por favor, seleccione una con:\n- M -> Translación\n- B -> Rotación\n- T -> Escalado\n");
}

/* Called if an object has not been loaded and the user tries to make a transformation */
void print_enonobject()
{
	printf("No se puede atender esta petición: no hay ningún objeto cargado.\nCargue uno con la tecla f, por favor.\n");
}

void print_matrix(double * mptr)
{
	int i,j;
	printf("\n");
	for(i=0; i<4; i++){
		for(j=0;j<4;j++){
			printf("%.3lf\t", mptr[i+j*4]);
		}
		printf("\n");
	}
	printf("\n");
}




