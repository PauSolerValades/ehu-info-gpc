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
extern int referencia; //00: objeto, 01: mundo; 10: análisis, 11: vuelo.
extern int camara_interna; //0: Desactivada, 1: Activada

/* all the functions declared to improve the order of aperance */
void print_help();
void print_enonmode();
void print_eworld();
void print_enonobject();
void destructor(object3d *object);
void new_transformation();
void inverse();
void centrar_camara();
void special(int k, int x, int y);
void keyboard(unsigned char key, int x, int y);
double euclidean_norm(double x, double y, double z);
void keyboard_object(unsigned char key, int x, int y);
void keyboard_camera(unsigned char key, int x, int y);


void keyboard_camera(unsigned char key, int x, int y)
{
	printf("ENTRO A KEYBOARD CAMERA\n");
	switch (key)
	{
		case 'n':
		case 'N':
			printf("Nueva camara\n");
			
			//la primera camara siempre está inicializada => el puntero nunca será null
			if(mode)
			{

				int i = 0;
				camera *new_camera;

				new_camera = (camera *)malloc(sizeof(camera));
				new_camera->nextptr = NULL;//apuntem el seguent punter a 0

				for (i = 1; i < 15; i++)
				{
					new_camera->M[i] = 0.0;
				}

				new_camera->M[0] = 1.0;
				new_camera->M[5] = 1.0;
				new_camera->M[10] = 1.0;
				new_camera->M[15] = 1.0;

				_selected_camera->nextptr = new_camera;
				_selected_camera = _selected_camera->nextptr;

			}else
			{
				printf("No está en el modo camera, para entrar en el pulse k\n");
			}
			
			break;

		case 'b':
		case 'B': /* Rotación */
			transformacion = 1;
			printf("Rotaciones CAMARA ACTIVADAS\n");
			break;
		
		case 'm':
		case 'M': /* Translación */
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
			referencia = 0; //cambiamos a modo objeto porque no tiene sentido el global.
			transformacion = 0;
			centrar_camara();

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
		if(mode == 0)
		{

			glMatrixMode(GL_MODELVIEW);
			
			if(!referencia)
				glLoadMatrixd(_selected_object->display->M);
			else
				glLoadIdentity();


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
						isAKey = 1;
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
						isAKey = 1;
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
						isAKey = 1;
						break;
					}
				break;

			default:
				print_enonmode();
				break;
			}

			if(referencia)
				glMultMatrixd(_selected_object->display->M);

			if(!isAKey)
				new_transformation(); //crea el nou elem_matrix buit i el posa a la llista
			
		}
		else if(mode == 1)
		{
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixd(_selected_camera->M);

			switch (transformacion)
			{
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
					isAKey = 1;
					break;
				}
				break;
			}

			glGetDoublev(GL_MODELVIEW_MATRIX, _selected_camera->M);
		}

		glutPostRedisplay();
	}
}

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
	GLdouble wd, he, midx, midy;

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
			new_mptr->nextptr = NULL;//apuntem el seguent punter a 0

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

			printf("%s\n", KG_MSSG_FILEREAD);
			break;
		}
		break;

	case 9: /* <TAB> */ //cambiar de objeto

		/* Controlar la lista, si és diferente de 0 que apunte al primero.*/
		if (_first_object != 0)
		{
			_selected_object = _selected_object->next;
			/*The selection is circular, thus if we move out of the list we go back to the first element*/
			if (_selected_object == 0)
				_selected_object = _first_object;

			if(mode && transformacion == 0)
			{
				centrar_camara();double euclidean_norm(double x, double y, double z) { return sqrt(x*x + y*y + z*z); }

			}
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
				destructor(_selected_object);			

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
				destructor(_selected_object);

				/*and update the selection*/
				_selected_object = auxiliar_object;
			}
		}
		break;

	case '-': //hace que todo se vea mas pequeño

		if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
		{
			/*Increase the projection plane; compute the new dimensions*/
			wd = (_ortho_x_max - _ortho_x_min) / KG_STEP_ZOOM;
			he = (_ortho_y_max - _ortho_y_min) / KG_STEP_ZOOM;
			/*In order to avoid moving the center of the plane, we get its coordinates*/
			midx = (_ortho_x_max + _ortho_x_min) / 2;
			midy = (_ortho_y_max + _ortho_y_min) / 2;
			/*The the new limits are set, keeping the center of the plane*/
			_ortho_x_max = midx + wd / 2;
			_ortho_x_min = midx - wd / 2;
			_ortho_y_max = midy + he / 2;
			_ortho_y_min = midy - he / 2;
		}
		else /* If control wasn't pressed, we have to call the special keys to attend the petition. This maybe isn't the most correct way, but it's very clear, sort and easy to undersand. */
			special(45, x, y);
			
		break;

	case '+': //hace que todo se vea más grande.

		if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
		{
			/*Increase the projection plane; compute the new dimensions*/
			wd = (_ortho_x_max - _ortho_x_min) * KG_STEP_ZOOM;
			he = (_ortho_y_max - _ortho_y_min) * KG_STEP_ZOOM;
			/*In order to avoid moving the center of the plane, we get its coordinates*/
			midx = (_ortho_x_max + _ortho_x_min) / 2;
			midy = (_ortho_y_max + _ortho_y_min) / 2;
			/*The the new limits are set, keeping the center of the plane*/
			_ortho_x_max = midx + wd / 2;
			_ortho_x_min = midx - wd / 2;
			_ortho_y_max = midy + he / 2;
			_ortho_y_min = midy - he / 2;
		}
		else /* Analogous of case '-': */
			special(43, x, y);
			
		break;

	case 'l':
	case 'L': /* Transformaciones ref objetos */
		referencia = 0;
		printf("Referencia OBJETO\n");
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
			
			printf("Siguiente camara\n");
		}

		break;

	case 'C': /* Activa/desactiva la camara interna del objeto */
		printf("CAMBIO A MODO OBJETO\n");
		mode = 0; //volvemos en modo transformación.
		referencia = 0; /* No tiene sentido transformar la cámara cuando estás desde la perspectiva del objeto. */
		
		if(camara_interna)
		{
			printf("Camara No Interna\n");	
			camara_interna = 0;
		}
		else
		{
			printf("Camara Interna\n");
			camara_interna = 1;
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

	case '?':
		print_help();
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
            printf("Cargue un objecto, por favor\n");            
        }
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

	/*In case we have do any modification affecting the displaying of the object, we redraw them*/
	free(fname); /* We have to free the memory used in the scanf */
	glutPostRedisplay();
}

void keyboard_object(unsigned char key, int x, int y)
{
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

		case 'g':
		case 'G': /* Transformaciones ref mundo */
			if(!camara_interna)
			{
				referencia = 1;
				printf("Referencia MUNDO\n");
			}
			else
			{
				printf("No puedes activar el modo MUNDO cuando la cámara interna de un objeto está activada");
			}
			break;
		
		default:
			printf("%d %c\n", key, x);
			break;
	}
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
	
	//printf("NEW OBJECT ADDED\n");
	
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
	_selected_object->display = new_mptr; //new_mptr

    glGetDoublev(GL_MODELVIEW_MATRIX, _selected_object->display->M);

	inverse(); //calcula i carrega la matriu inversa (transposada vamos.)
}

void inverse()
{
	int i;
	double dot_productX;
	double dot_productY;
	double dot_productZ;

	double *a;
	double *inv_A;

	a =_selected_object->display->M;
	inv_A =_selected_object->display->inv_M;

	dot_productX = 0.0;
	dot_productY = 0.0;
	dot_productZ = 0.0;

	for(i=0; i<=2; i++)
	{
		//producto escalar de X y assignación del vector x
		dot_productX += a[i] * a[i+12];
		inv_A[i] = a[4*i];

		//producto escalar de X y assignación del vector y
		dot_productY += a[i+4] * a[i+12];
		inv_A[i+4] = a[(4*i)+1];

		//producto escalar de X y assignación del vector z
		dot_productZ += a[i+8] * a[i+12];
		inv_A[i+8] = a[(4*i)+2];

		//printf("%d %d %d %d %d %d\n", i, i+4, i+8, (4*i), (4*i)+1, (4*i)+2);
	}

	/* posem el producte escalar a la matriu (i inicialitzem les altres per seguretat...) */
	inv_A[3] = 0.0;
	inv_A[7] = 0.0;
	inv_A[11] = 0.0;
	inv_A[12] = -dot_productX;
	inv_A[13] = -dot_productY;
	inv_A[14] = -dot_productZ;
	inv_A[15] = 1.0;

}

double euclidean_norm(double x, double y, double z) { return sqrt(x*x + y*y + z*z); }

/* Centers the camera in the object owo. */
void centrar_camara()
{
	int i, centrado_mismo_punto;
	double module_z, module_x;

	double E[3], P[3], U[3], x_c[3], y_c[3], z_c[3];

	centrado_mismo_punto = 0;

	for(i = 0; i<3; i++){
		E[i] = _selected_camera->M[12+i];
		P[i] = _selected_object->display->M[12+i];
		U[i] = _selected_camera->M[i+4];

		if(E[i]==P[i]){
			centrado_mismo_punto++;
		}
	}

	if(centrado_mismo_punto != 3)
	{
		z_c[0] = (-E[0]+P[0]);
		z_c[1] = (-E[1]+P[1]);
		z_c[2] = (-E[2]+P[2]);

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
			_selected_camera->M[i] = x_c[i];
			_selected_camera->M[i+4] = y_c[i];
			_selected_camera->M[i+8] = z_c[i];
			_selected_camera->M[i+12] = P[i];
		}
	}
}



/**
Called in case supr, this function frees al the dynamic memory used in a object3d:
	- vertex_table
	- face_tables and all the vertex_table inside it
	- linked list of elem_matrix
*/
void destructor(object3d *object)
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




