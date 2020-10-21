#include "definitions.h"
#include "load_obj.h"
#include <GL/glut.h>
#include <stdio.h>

extern object3d *_first_object;
extern object3d *_selected_object;

extern GLdouble _ortho_x_min, _ortho_x_max;
extern GLdouble _ortho_y_min, _ortho_y_max;
extern GLdouble _ortho_z_min, _ortho_z_max;

extern int mode;	   //0: translación, 1: rotación, 2: escalado.
extern int referencia; //0: objeto, 1: mundo

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
	printf("\n\n");
}

void print_enonmode()
{
	printf("No hay ninguna transformación selecionada. Por favor, seleccione una con:\n- M -> Translación\n- B -> Rotación\n- T -> Escalado\n");
}
void print_eworld()
{
	printf("No hay ninguna referencia seleccionada. Por favor, selecciona una con:\n- L -> Referencia Objeto\n- G -> Referencia Mundo\n");
}

void print_enonobject()
{
	printf("No se puede atender esta petición: no hay ningún objeto cargado.\nCargue uno con la tecla f, por favor.\n");
}

/*
free the memory of a 3dobject.
has to be freed:
- vertex_table: table filled with struct vertex. Check if openGL objects have to be freed
- face_table: table filled with struct face. Again, check if openGL objects need to be freed
- elem_matrix: linked list of structs elem_matrix.

We have a face_table, which contains as many vertex_tables as num_vertex in face_table. We have to run over the face_table freeing every vertex table, then free the face_table. Then we repeat for each face.
Freeing the linked list as normaly is done.
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

void borrar_lista_objetos()
{
	/* PREGUNTAR SI ES NECESARIO */

	object3d *del;

	while (_first_object->next != 0)
	{
		del = _first_object;
		_first_object = _first_object->next;
		destructor(del);
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

	elem_matrix *mptr;
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

			/* DONE: cargar matriz */
			mptr = (elem_matrix *)malloc(sizeof(elem_matrix)); //guardem espai per la matriu identitat
			mptr->nextptr = 0;								   //apuntem el seguent punter a 0

			//llenamos la tabla M
			for (i = 1; i < 15; i++)
			{
				mptr->M[i] = 0.0;
			}
			//cutríssim pero va
			mptr->M[0] = 1.0;
			mptr->M[5] = 1.0;
			mptr->M[10] = 1.0;
			mptr->M[15] = 1.0;

			//asignamos la matriz a la id.
			_selected_object->mptr = mptr;

			//asignamos la variable global
			_selected_object->display = mptr;

			//declaramos el num_undos
			_selected_object->num_undos = 0;

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
		}
		break;

	case 127: /* <SUPR> */ //borrar objeto

		/* si no hay objeto que borrar tiene que no borrar nada*/
		/*Erasing an object depends on whether it is the first one or not*/
		if (_first_object != 0)
		{
			if (_selected_object == _first_object)
			{
				/*DONE el free de elem_matrix*/
				/*To remove the first object we just set the first as the current's next*/
				_first_object = _first_object->next;
				/*Once updated the pointer to the first object it is save to free the memory*/
				/*TODO: free the memory properly*/
				destructor(_selected_object);
				/*
                //codi d'abans
                free(_selected_object->next_matrix);
                free(_selected_object);
                */
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
				/*TODO: free the memory*/
				destructor(_selected_object);
				/*
                //codi d'abans
                free(_selected_object->next_matrix);
                free(_selected_object);
                */
				/*and update the selection*/
				_selected_object = auxiliar_object;
			}
		}
		break;

	case '+': //hace que todo se vea mas pequeño

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
		break;

	case '-': //hace que todo se vea más grande.

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
		break;

	case 'm':
	case 'M': /* Activar Translación */
		mode = 0;
		printf("Translaciones ACTIVADAS\n");
		break;

	case 'b':
	case 'B': /* Rotación */
		mode = 1;
		printf("Rotaciones ACTIVADAS\n");
		break;

	case 't':
	case 'T': /* Escalado */
		mode = 2;
		printf("Escalado ACTIVADO\n");
		break;

	case 'g':
	case 'G': /* Transformaciones ref mundo */
		referencia = 1;
		printf("Referencia del MUNDO\n");
		break;

	case 'l':
	case 'L': /* Transformaciones ref objetos */
		referencia = 0;
		printf("Referencia del OBJETO\n");
		break;

	case 'o':
	case 'O': /* Sistema referencia objeto */
		printf("Objeto\n");
		break;

	case 'k':
	case 'K': /* Transformaciones camara actual */
		printf("Funcionalidad no implementada\n");
		break;

	case 'a':
	case 'A': /* Transformaciones luz selecionada */
		printf("Funcionalidad no implementada\n");

	case '?':
		print_help();
		break;

	case 25:
		printf("Rehacer\n");

		//_selected_object->display = _selected_object->mptr + ((_selected_object->num_undos) - 1) * (sizeof(elem_matrix));

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
		_selected_object->num_undos -= 1;
    	

		printf("%d\n", _selected_object->num_undos);

		glutPostRedisplay();

		break;

	case 26: /* CONTROL+Z */
		printf("Deshacer\n");

		if (_selected_object->display->nextptr != 0)
		{
			_selected_object->display = _selected_object->display->nextptr;
			_selected_object->num_undos++;
		}
		else
		{
			printf("No más undos\n");
		}
		//glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(_selected_object->display->M);

		glGetDoublev(GL_MODELVIEW_MATRIX, _selected_object->display->M);
		glutPostRedisplay();

		break;

	case 27: /* <ESC> */
		/* TODO: eliminar los objetos que no se han suprimido manualmente */

		borrar_lista_objetos();

		exit(0);
		break;

	default:
		/*In the default case we just print the code of the key. This is usefull to define new cases*/
		printf("%d %c\n", key, key);
	}
	/*In case we have do any modification affecting the displaying of the object, we redraw them*/
	glutPostRedisplay();
}

void new_transformation()
{
	elem_matrix *new_mptr;

	new_mptr = (elem_matrix *)malloc(sizeof(elem_matrix));

	_selected_object->mptr = new_mptr;
	new_mptr->nextptr = _selected_object->display;
	_selected_object->display = _selected_object->mptr; //new_mptr

	/* TODO: BORRAR LES MATRIUS QUE ES PERDEN AQUÍ */
}

void translation(double x, double y, double z)
{

	//glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(_selected_object->display->M);
	glTranslated(x, y, z);

	new_transformation(); //crea el nou elem_matrix buit i el posa a la llista

	glGetDoublev(GL_MODELVIEW_MATRIX, _selected_object->mptr->M);

	glutPostRedisplay();
}

void rotation(double a, double x, double y, double z)
{
	//glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(_selected_object->display->M);
	glRotated(a, x, y, z);

	new_transformation(); //crea el nou elem_matrix buit i el posa a la llista

	glGetDoublev(GL_MODELVIEW_MATRIX, _selected_object->display->M);

	glutPostRedisplay();
}

void scale(double lx, double ly, double lz)
{
	//glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(_selected_object->display->M);
	glScaled(lx, ly, lz);

	new_transformation(); //crea el nou elem_matrix buit i el posa a la llista

	glGetDoublev(GL_MODELVIEW_MATRIX, _selected_object->display->M);

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
	if (_selected_object != 0)
	{
		switch (key)
		{
		case 100: /* LEFT ARROW */
			printf("TECLA ESQUERRA\n");
			switch (referencia)
			{
			case 0: //objeto
				switch (mode)
				{
				case 0:
					translation(-T, 0.0, 0.0);
					//translation(-1.0,0.0,0.0);
					break;
				case 1:
					//angle is in degrees
					rotation(A, 0.0, -1.0, 0.0);
					break;
				case 2:
					scale(DS, 1.0, 1.0);
					break;
				default:
					print_enonmode();
				}
				break;
			case 1: //mundo
				switch (mode)
				{
				case 0:
					translation(-T, 0.0, 0.0);
					break;
				case 1:
					rotation(A, 0.0, 1.0, 0.0);
					break;
				case 2:
					//scale(2.0, 2.0, 2.0);
					break;
				default:
					print_enonmode();
				}
				break;
			default:
				print_enonmode();
			}
			break;

		case 101: /* UP ARROW */
			printf("TECLA AMUNT\n");
			switch (referencia)
			{
			case 0: //objeto
				switch (mode)
				{
				case 0:
					translation(0.0, T, 0.0);
					break;
				case 1:
					rotation(A, -1.0, 0.0, 0.0);
					break;
				case 2:
					scale(1.0, US, 1.0);
					break;
				default:
					print_enonmode();
				}
				break;
			case 1: //mundo
				switch (mode)
				{
				case 0:
					translation(0.0, T, 0.0);
					break;
				case 1:
					//rotation(0.0);
					break;
				case 2:
					//scale(1.0);
					break;
				default:
					print_enonmode();
				}
				break;
			default:
				print_eworld();
			}
			break;

		case 102: /* RIGHT ARROW */
			printf("TECLA DRETA\n");
			switch (referencia)
			{
			case 0: //objeto
				switch (mode)
				{
				case 0:
					translation(T, 0.0, 0.0);
					break;
				case 1:
					rotation(A, 0.0, 1.0, 0.0);
					break;
				case 2:
					scale(US, 1.0, 1.0);
					break;
				default:
					print_enonmode();
				}
				break;
			case 1: //mundo
				switch (mode)
				{
				case 0:
					translation(T, 0.0, 0.0);
					break;
				case 1:
					//rotation(0.0);
					break;
				case 2:
					//scale(1.0);
					break;
				default:
					print_enonmode();
				}
				break;
			default:
				print_eworld();
			}
			break;

		case 103: /* DOWN ARROW */
			printf("TECLA AVALL\n");
			switch (referencia)
			{
			case 0: //objeto
				switch (mode)
				{
				case 0:
					translation(0.0, -T, 0.0);
					break;
				case 1:
					rotation(A, 1.0, 0.0, 0.0);
					break;
				case 2:
					scale(1.0, DS, 1.0);
					break;
				default:
					print_enonmode();
				}
				break;
			case 1: //mundo
				switch (mode)
				{
				case 0:
					translation(0.0, -T, 0.0);
					break;
				case 1:
					//rotation(0.0);
					break;
				case 2:
					//scale(1.0);
					break;
				default:
					print_enonmode();
				}
				break;
			default:
				print_enonmode();
			}
			break;

		case 104: /* REPAG */
			printf("TECLA REPAG\n");
			switch (referencia)
			{
			case 0: //objeto
				switch (mode)
				{
				case 0:
					translation(0.0, 0.0, T);
					break;
				case 1:
					rotation(A, 0.0, 0.0, 1.0);
					break;
				case 2:
					scale(1.0, 1.0, DS);
					break;
				default:
					print_enonmode();
				}
				break;
			case 1: //mundo
				switch (mode)
				{
				case 0:
					translation(0.0, T, 0.0);
					break;
				case 1:
					//rotation(0.0);
					break;
				case 2:
					//scale(1.0);
					break;
				default:
					print_enonmode();
				}
				break;
			default:
				print_eworld();
			}
			break;

		case 105: /* AVPAG */
			printf("TECLA AVPAG\n");
			switch (referencia)
			{
			case 0: //objeto
				switch (mode)
				{
				case 0:
					translation(0.0, 0.0, T);
					break;
				case 1:
					rotation(A, 0.0, 0.0, -1.0);
					break;
				case 2:
					scale(1.0, 1.0, US);
					break;
				default:
					print_enonmode();
				}
				break;
			case 1: //mundo
				switch (mode)
				{
				case 0:
					translation(0.0, T, 0.0);
					break;
				case 1:
					//rotation(0.0);
					break;
				case 2:
					//scale(1.0);
					break;
				default:
					print_enonmode();
				}
				break;
			default:
				print_eworld();
			}
			break;

		default:
			printf("%d \n", key);
		}
	}
	else
	{
		print_enonobject();
	}

	glutPostRedisplay();
}
