#include "definitions.h"
#include "load_obj.h"
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

extern object3d *_first_object;
extern object3d *_selected_object;
extern camera *_first_camera;
extern camera *_selected_camera;

extern GLdouble _ortho_x_min, _ortho_x_max;
extern GLdouble _ortho_y_min, _ortho_y_max;
extern GLdouble _ortho_z_min, _ortho_z_max;

extern int mode; //0 objeto, 1: camara, 2: luz
extern int transformacion; 
//0: translación, 1: rotación, 2: escalado cuando mode = 0
//0: translacion, 2: rotación, 3: volumen de visión
extern int referencia; //00: objeto, 01: mundo;
extern int camara_interna; //0: Desactivada, 1: Activada
extern int flat_smooth; //0: flat, 1: smooth

extern int fill_polygons;

extern int selected_light; //uno para cada luz
extern light* luces[8];

/* all the functions declared to improve the order of aperance */
void print_help();
void print_enonmode();
void print_eworld();
void print_enonobject();
void print_matrix();
void print_lista_materiales();
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
void keyboard_luces(unsigned char key, int x, int y);
void switch_transformaciones_analisis(int k, int *isAKey);
void switch_transformaciones(int k, int *isAKey);
void funcion_transformacion(int k);
void new_light_transformation();
void apuntar_punto(elem_matrix *object, double P[4]);

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
	int mname = 0;
	int typeLight;

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

			//cargamos el oro por defecto porque pau es daltónico
			auxiliar_object->vectorMaterial[0] = 0.75164;
			auxiliar_object->vectorMaterial[1] = 0.60648;
			auxiliar_object->vectorMaterial[2] = 0.22648;
			auxiliar_object->shine = 0,4;

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


	case 9: /* <TAB> */ //cambiar de objeto

		/* Controlar la lista, si és diferente de 0 que apunte al primero.*/
		if (_first_object != 0)
		{
			_selected_object = _selected_object->next;
			/*The selection is circular, thus if we move out of the list we go back to the first element*/
			if (_selected_object == 0)
				_selected_object = _first_object;

			if(mode && transformacion == 0){ //TODO: NO RECORDO QUÈ FA AIXÒ.
				double puntoObjeto[4] = {_selected_object->display->M[12], _selected_object->display->M[13], _selected_object->display->M[14], _selected_object->display->M[15]};
				apuntar_punto(_selected_camera->actual, puntoObjeto);
			}
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
		if(mode != 0)
		{
			printf("TRANSFORMACIONES DE OBJETOS\n");
			mode = 0;
		}
		else
		{
			printf("Ya estás en modo objeto...\n");
		}
		break;

	case 'c': /* cambia a la siguiente camara */
		if (_first_camera != 0)
		{
			_selected_camera = _selected_camera->nextptr;
			/*The selection is circular, thus if we move out of the list we go back to the first element*/
			if (_selected_camera == 0)
				_selected_camera = _first_camera;
			if(_selected_camera->type)
			{
				double puntoObjeto[4] = {_selected_object->display->M[12], _selected_object->display->M[13], _selected_object->display->M[14], _selected_object->display->M[15]};
				apuntar_punto(_selected_camera->actual, puntoObjeto);
			}
			printf("Siguiente camara\n");
		}

		break;

	case 'C': /* Activa/desactiva la camara interna del objeto */
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
		if(mode != 1)
		{
			mode = 1;
			printf("TRANSFORMACIONES DE CAMARAS\n");
		}
		else
		{
			printf("Ya estas en modo camara...\n");
		}
		
		
		break;

	case 'a':
	case 'A': /* Transformaciones luz selecionada */
		if(fill_polygons)
		{
			if(mode != 2)
			{
				printf("TRANSFORMACIONES DE LUCES\n");
				mode = 2;

			}
			else
			{
				printf("Ya estás en modo luces...\n");
			}
		}
		else
		{
			printf("No hay luces en modo GL_LINE. Cambia a GL_FILL con F9\n");
		}
		
		break;

	case '?':
		print_help();
		break;

	case 27: /* <ESC> */
		exit(0);
		break;

	default:
		/* In default we call the other two functions. If the program has reached this line a key with two functionalities has been pressed */
		
		if(mode == 0)
			keyboard_object(key, x, y);
		else if(mode == 1)
			keyboard_camera(key, x, y);
		else if(mode == 2)
			keyboard_luces(key, x, y);
		else
			printf("polla\n");
		
        break;
	}

	free(fname); /* We have to free the memory used in the scanf */
	glutPostRedisplay();
}


void keyboard_luces(unsigned char key, int x,int y)
{
	float newangulo;
	int luz_actual, counter; 
	char *yesno;

	switch (key)
	{
		case 'm':
		case 'M': /* Translación */
			transformacion = 0;
			printf("Translaciones LUCES ACTIVADAS\n");
			break;
		
		case 'b':
		case 'B': /* Rotación */
			transformacion = 1;
			printf("Rotaciones LUCES ACTIVADAS\n");
			break;

		case '+':
			if(luces[(selected_light-1)]->type == 2)
			{
				newangulo = 0;
				newangulo = luces[selected_light-1]->angulo + A/2;
				if(newangulo <= 90)
					luces[selected_light-1]->angulo = newangulo;
				else
					printf("Los focos tienen un rango de [0,90]\n");	
			}else{
				printf("Sólo se pueden modificar los focos\n");
			}

			break;
		
		case '-':
			if(luces[(selected_light-1)]->type == 2)
			{
				newangulo = 0;
				newangulo = luces[selected_light-1]->angulo - A/2;
				if(newangulo >= 0)
					luces[selected_light-1]->angulo = newangulo;
				else
					printf("Los focos tienen un rango de [0,90]\n");
			}else{
				printf("Sólo se pueden modificar los focos\n");
			}

			break;

	case '0':

		if(selected_light <= 3)
		{
			printf("Las tres primeras luces son inmodificables\n");
		}
		else
		{
			yesno = malloc(sizeof(char));
			counter = 0;
			
			luz_actual = selected_light-1;
			while(_selected_object != 0)
			{
				luces[luz_actual]->type = (luces[luz_actual]->type + 1) % 3;
				counter++;

				if(luces[luz_actual]->type == 1){		
					printf("Modo bombilla? [y/n]\n");
				}else if(luces[luz_actual]->type == 2){					
					printf("Modo foco? [y/n]\n");
				}else{
					printf("Modo sol? [y/n]\n");
				}	

				scanf("%s",yesno);


				if(!strcmp(yesno,"y"))
				{	
					switch (luces[luz_actual]->type)
					{
					case 0: //sol
						printf("Introduce las coordenadas x y z separados por espacios:\n");
						scanf(" %f %f %f", &luces[luz_actual]->position[0], &luces[luz_actual]->position[1], &luces[luz_actual]->position[2]);
						printf("Introduce los valores entre [0,1] red green blue alpha separados por espacios: \n");
						scanf(" %f %f %f %f", &luces[luz_actual]->RGBA[0], &luces[luz_actual]->RGBA[1], &luces[luz_actual]->RGBA[2], &luces[luz_actual]->RGBA[3]);
						luces[luz_actual]->position[4] = 0.0;
						printf("Valores cambiados correctamente\n");
						break;
					
					case 1: //bombilla
						printf("Introduce las coordenadas x y z separados por espacios:\n");
						scanf(" %f %f %f", &luces[luz_actual]->position[0], &luces[luz_actual]->position[1], &luces[luz_actual]->position[2]);
						printf("Introduce las coordenadas del punto al que mirará la bombilla:\n");
						scanf(" %f %f %f", &luces[luz_actual]->direction[0], &luces[luz_actual]->direction[1], &luces[luz_actual]->direction[2]);
						printf("Introduce los valores red green blue alpha separados por espacios: \n");
						scanf(" %f %f %f %f", &luces[luz_actual]->RGBA[0], &luces[luz_actual]->RGBA[1], &luces[luz_actual]->RGBA[2], &luces[luz_actual]->RGBA[3]);
						luces[luz_actual]->position[4] = 1.0;
						printf("Valores cambiados correctamente\n");
						break;

					case 2: //foco
						printf("Introduce las coordenadas x y z separados por espacios:\n");
						scanf(" %f %f %f", &luces[luz_actual]->position[0], &luces[luz_actual]->position[1], &luces[luz_actual]->position[2]);
						printf("Introduce las coordenadas del punto al que mirará el foco:\n");
						scanf(" %f %f %f", &luces[luz_actual]->direction[0], &luces[luz_actual]->direction[1], &luces[luz_actual]->direction[2]);
						printf("Introduce el angulo de amplitud del foco:\n");
						scanf(" %f", &luces[luz_actual]->angulo);
						printf("Introduce los valores red green blue alpha separados por espacios: \n");
						scanf(" %f %f %f %f", &luces[luz_actual]->RGBA[0], &luces[luz_actual]->RGBA[1], &luces[luz_actual]->RGBA[2], &luces[luz_actual]->RGBA[3]);
						luces[luz_actual]->position[4] = 1.0;
						printf("Valores cambiados correctamente\n");
						break;

					default:
						printf("Ha habido un error en la assignación del type\n");
						break;
					}

					break;
				}
				else
				{
					if(counter == 3)
					{	
						printf("Quieres dejar la luz como estaba? [y/n]\n");
						scanf(" %s", yesno);
						if(!strcmp("y", yesno))
						{
							printf("De acuerdo\n");
							break;
						}
						else
							counter = 0;
					}
				}
			}

			free(yesno);
		}

		break;

	case '1':
		printf("Luz 1 selecionada\n");
		selected_light = 1;
		break;

	case '2':
		printf("Luz 2 selecionada\n");
		selected_light = 2;
 		break;

	case '3':
		printf("Luz 3 selecionada\n");
		selected_light = 3;
		break;

	case '4':
		printf("Luz 4 selecionada\n");
		selected_light = 4;
		break;

	case '5':
		printf("Luz 5 selecionada\n");
		selected_light = 5;
		break;

	case '6':
		printf("Luz 6 selecionada\n");
		selected_light = 6;
		break;

	case '7':
		printf("Luz 7 selecionada\n");
		selected_light = 7;
		break;

	case '8':
		printf("Luz 8 selecionada\n");
		selected_light = 8;
		break;
		
		default:
			printf("%d %c\n", key, key);
			break;
	}

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
					printf("Rehacer OBJETO\n");
					
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
					printf("Deshacer OBJETO\n");
						
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
			printf("%d %c\n", key, key);
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
			
			double puntoObjeto[4] = {_selected_object->display->M[12], _selected_object->display->M[13], _selected_object->display->M[14], _selected_object->display->M[15]};
			apuntar_punto(_selected_camera->actual, puntoObjeto);
			
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
					printf("Rehacer CAMARA\n");
					
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
					printf("Deshacer CAMARA\n");
						
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
			printf("%d %c\n", key, key);
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
	int mname = 0;
	time_t t;
	srand((unsigned) time(&t));

    switch (k)
	{
	case GLUT_KEY_F1:
		if(glIsEnabled(GL_LIGHT0))
		{
			printf("Luz 1 Desactivada\n");
			glDisable(GL_LIGHT0);
		}
		else
		{
			printf("Luz 1 Activada\n");
			glEnable(GL_LIGHT0);
		}
		break;
		
	case GLUT_KEY_F2:
		if(glIsEnabled(GL_LIGHT1))
		{
			printf("Luz 2 Desactivada\n");
			glDisable(GL_LIGHT1);
		}
		else
		{
			printf("Luz 2 Activada\n");
			glEnable(GL_LIGHT1);
		}
		break;

	case GLUT_KEY_F3:
		if(glIsEnabled(GL_LIGHT2))
		{
			printf("Luz 3 Desactivada\n");
			glDisable(GL_LIGHT2);
		}
		else
		{
			printf("Luz 3 Activada\n");
			glEnable(GL_LIGHT2);
		}
		break;
	
	case GLUT_KEY_F4:
		if(glIsEnabled(GL_LIGHT3))
		{
			printf("Luz 4 Desactivada\n");
			glDisable(GL_LIGHT3);
		}
		else
		{
			printf("Luz 4 Activada\n");
			glEnable(GL_LIGHT3);
		}
		break;
	
	case GLUT_KEY_F5:
		if(glIsEnabled(GL_LIGHT4))
		{
			printf("Luz 5 Desactivada\n");
			glDisable(GL_LIGHT4);
		}
		else
		{
			printf("Luz 5 Activada\n");
			glEnable(GL_LIGHT4);
		}
		break;

	case GLUT_KEY_F6:
		if(glIsEnabled(GL_LIGHT5))
		{
			printf("Luz 6 Desactivada\n");
			glDisable(GL_LIGHT5);
		}
		else
		{
			printf("Luz 6 Activada\n");
			glEnable(GL_LIGHT5);
		}
		break;

	case GLUT_KEY_F7:
		if(glIsEnabled(GL_LIGHT6))
		{
			printf("Luz 7 Desactivada\n");
			glDisable(GL_LIGHT6);
		}
		else
		{
			printf("Luz 7 Activada\n");
			glEnable(GL_LIGHT6);
		}
		break;

	case GLUT_KEY_F8:
		if(glIsEnabled(GL_LIGHT7))
		{
			printf("Luz 8 Desactivada\n");
			glDisable(GL_LIGHT7);
		}
		else
		{
			printf("Luz 8 Activada\n");
			glEnable(GL_LIGHT7);
		}
		break;

	case GLUT_KEY_F9: /* cambia de rejilla a polígonos pintados. */
		if(fill_polygons) //pinto polígonos
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			fill_polygons = 0;
			printf("Estás en modo rejilla (GL_LINE)\n");
		}
		else //ahora en lineas
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			fill_polygons = 1;
			printf("Estás en modo relleno (GL_FILL)\n");
		}
		
		glutPostRedisplay();

		break;

	case GLUT_KEY_F12:
		if(flat_smooth)
		{
			printf("Luces en Flat\n");
			flat_smooth = 0;
		}
		else
		{
			printf("Luces en Smooth\n");
			flat_smooth = 1;
		}
		break;
	
	case GLUT_KEY_INSERT:

		print_lista_materiales();

		scanf("%d", &mname);

		if(mname == 4)
		{
			mname = (rand() % 3) + 1;
			printf("%d\n", mname);
		}
		switch (mname)
			{
			case 1:
					_selected_object->vectorMaterial[0] = 0.75164;
					_selected_object->vectorMaterial[1] = 0.60648;
					_selected_object->vectorMaterial[2] = 0.22648;
					_selected_object->shine = 0,4;
					printf("\nMaterial cambiado satisfactoriamente\n");
				break;
			case 2:
					_selected_object->vectorMaterial[0] = 0.61424;
					_selected_object->vectorMaterial[1] = 0.04136;
					_selected_object->vectorMaterial[2] = 0.04136;
					_selected_object->shine = 0,6;
					printf("\nMaterial cambiado satisfactoriamente\n");
				break;
			case 3:
					_selected_object->vectorMaterial[0] = 0.25;
					_selected_object->vectorMaterial[1] = 0.20725;
					_selected_object->vectorMaterial[2] = 0.922;
					_selected_object->shine = 11.264;
					printf("\nMaterial cambiado satisfactoriamente\n");
				break;
				
			default:
				printf("\nNo has seleccionado una opción válida\n");
				break;
			}
			

		break;

	default:
		funcion_transformacion(k);
		//printf("%d %c\n", k, k);
		break;
	}
	glutPostRedisplay();
}

void funcion_transformacion(int k)
{
	int isAKey;

    isAKey = 0;
    
    if(_selected_object != NULL)
    {	

		glMatrixMode(GL_MODELVIEW);

		switch (mode)
		{
		case 0:

			if(!referencia)
				glLoadMatrixd(_selected_object->display->M);
			else
				glLoadIdentity();

			switch_transformaciones(k, &isAKey); //aquí hi ha el switch on es multiplica tot.

			if(referencia)
				glMultMatrixd(_selected_object->display->M);
			
			if(!isAKey)
				new_transformation(); //crea el nou elem_matrix buit i el posa a la llista

			break;

		case 1:

			if(_selected_camera->type == 0)
			{
				glLoadMatrixd(_selected_camera->actual->M);
				switch_transformaciones(k, &isAKey);
			}
			else
				switch_transformaciones_analisis(k, &isAKey);

			if(!isAKey)
				new_camera_transformation();
			break;

		case 2:

			if(transformacion != 2)
			{

				glLoadMatrixd(luces[selected_light-1]->mptr->M);

				switch (luces[selected_light-1]->type)
				{
				case 0: //sol solo puede trasladarse
					if(transformacion == 0)
						switch_transformaciones(k, &isAKey);
					else
						printf("Los soles no se pueden escalar ni rotar\n");

					break;

				case 1: //tanto bombilla como foco pueden rotar
					switch_transformaciones(k, &isAKey);
					
					break;

				case 2:
					switch_transformaciones(k, &isAKey);

					break;

				default:
					break;
				}

				new_light_transformation();
				
			}
			else
			{
				printf("Las luces no se pueden escalar\n");
			}

			break;
			

		default:
			break;
		}
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

void new_light_transformation()
{
	int i;
	//NO hay redo ni undo ni creo que lo vaya a poner, por eso reutilizamos elem_matrix y no guardamos memoria
	/*elem_matrix *new_mptr, *aux;

	new_mptr = (elem_matrix *)malloc(sizeof(elem_matrix));

	new_mptr->nextptr = luces[(selected_light-1)]->mptr;
	luces[selected_light-1]->mptr = new_mptr;
	*/

	glGetDoublev(GL_MODELVIEW_MATRIX, luces[selected_light-1]->mptr->M);
	inverse(luces[selected_light-1]->mptr->M, luces[selected_light-1]->mptr->inv_M);

	for(i=0; i<3; i++)
	{
		luces[selected_light-1]->position[i] = luces[selected_light-1]->mptr->M[12+i];
		luces[selected_light-1]->direction[i] = luces[selected_light-1]->mptr->M[8+i];
	}

	print_matrix(luces[selected_light-1]->mptr->M);
	
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

	/* Inicializamos las variables del vector normal porsi */

	for(i=0; i<_selected_object->num_vertices; i++)
	{
		_selected_object->vertex_table[i].normal[0] = 0.0;
		_selected_object->vertex_table[i].normal[1] = 0.0;
		_selected_object->vertex_table[i].normal[2] = 0.0;
	}

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

		for(j = 0; j<_selected_object->face_table[i].num_vertices; j++) //porque es menos uno?
		{
			indice = _selected_object->face_table[i].vertex_table[j];
			_selected_object->vertex_table[indice].normal[0] += vn[0];
			_selected_object->vertex_table[indice].normal[1] += vn[1];
			_selected_object->vertex_table[indice].normal[2] += vn[2];
		}
	}
	
	for(i = 0; i<_selected_object->num_vertices; i++)
	{
		double v[3], module;

		v[0] = _selected_object->vertex_table[i].normal[0];
		v[1] = _selected_object->vertex_table[i].normal[1];
		v[2] = _selected_object->vertex_table[i].normal[2];

		module = euclidean_norm(v[0], v[1], v[2]);

		_selected_object->vertex_table[i].normal[0] = v[0]/module;
		_selected_object->vertex_table[i].normal[1] = v[1]/module;
		_selected_object->vertex_table[i].normal[2] = v[2]/module;

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

/* Cambia los vectores de la matriz del primer argumento para que apunten al punto P del segundo argumento. */
void apuntar_punto(elem_matrix *object, double P[4])
{
	int i, centrado_mismo_punto;
	double module_z, module_x;

	double E[3], U[3], x_c[3], y_c[3], z_c[3];

	centrado_mismo_punto = 0;

	for(i = 0; i<3; i++){
		E[i] = object->M[12+i];
		U[i] = object->M[i+4];

		if(E[i]==P[i])
			centrado_mismo_punto++;
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
			object->M[i] = x_c[i];
			object->M[4+i] = y_c[i];
			object->M[i+8] = z_c[i];
			object->M[i+12] = E[i];
		}

		inverse(object->M, object->inv_M);
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
	printf("<A> \t\t Mode lightning\n");
	printf("<L> \t\t Object Reference\n");
	printf("<G> \t\t Global Reference\n");
	printf("<+/- in T> \t Scale up/down all the object axis\n\n");
	printf("<F1-F8> \t Amatatu edo piztu argiak\n");
	printf("<1-8> \t Hautatu modifikatu nahi dozun argia\n");
	printf("<INSERT> \t Material aldaketa menua\n ");
	printf("<0> \t\t Argi aldaketa menua \n");
	printf("<F9> \t\t Aldatu material edo linea modu artean\n");
	printf("<+> \t\t Hautatuko argia foko bat baldin bada, bere proiekzio angelua handitu\n");
	printf("<-> \t\t Hautatuko argia foko bat baldin bada, bere proiekzio angelua txikitu\n");


	
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

void print_lista_materiales()
{
	printf("\nQue material deseas asignarle al objeto?:\n\n");
	printf("1 - Oro\n");
	printf("2 - Rubí\n");
	printf("3 - Zafiro\n");
	printf("4 - Aleatorio\n\n");
}