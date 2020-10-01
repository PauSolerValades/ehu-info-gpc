//	Program developed by
//	
//	Informatika Fakultatea
//	Euskal Herriko Unibertsitatea
//	http://www.ehu.eus/if
//
// to compile it: gcc dibujar-puntos.c -lGL -lGLU -lglut
//
// This example shows :the use of GL_POINTS
//


#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include "hiruki.h"


// texturaren informazioa

extern void load_ppm(char *file, unsigned char **bufferptr, int *dimxptr, int * dimyptr);
unsigned char *bufferra;
int dimx,dimy;

int indice, num_triangles;
hiruki *triangulosptr; //pointer to triangles.

/* bufferra els guarda en ordre desde 0 a l'últim, tots en ordre (fila1, fila2, ... fila n)
el sistema de referencia del buffer comença a dalt. així que u va igual però v és 1-va
*/
/*
colors_textura returns the colour of the pixel in r,g,b given two entrances, u and v, which represent where in the picture these pixels where in the first place.
*/
unsigned char * color_textura(float u, float v)
{

	int i, j;
	
	i = trunc(u*(dimx));
	j = trunc((1-v)*(dimy));

	return(bufferra + 3*(j*dimx + i));
}

/*
This function assignates three pointers from higest to lowest in y axis for the higest verticie is always called C and the lowest always A of the triangle pointed in the global variable triangulosptr[indice]. In case of A and C have the same y, will be sorted with the value of the x of eachother, being A the closer to (0,0) and C the furthest.
ARGS:
punto **Aptrptr: pointer of pointer of the first point of the tirangle
punto **Bptrptr: pointer of pointer of the second point of the tirangle
punto **Cptrptr: pointer of pointer of the third point of the tirangle
*/
void determinar_orden(punto **Aptrptr, punto **Bptrptr, punto **Cptrptr)
{
	punto *aux;
	*Aptrptr = &(triangulosptr[indice].p1);
	*Bptrptr = &(triangulosptr[indice].p2);
	*Cptrptr = &(triangulosptr[indice].p3);

	//printf("Entra: A %f %f, B %f %f, C %f %f \n", (*Aptrptr)->x, (*Aptrptr)->y, (*Bptrptr)->x, (*Bptrptr)->y, (*Cptrptr)->x, (*Cptrptr)->y);

	if((*Aptrptr)->y > (*Cptrptr)->y)
	{
		aux = *Aptrptr;
		*Aptrptr = *Cptrptr;
		*Cptrptr = aux;
	}
	
	if((*Aptrptr)->y > (*Bptrptr)->y)
	{
		aux = *Aptrptr;
		*Aptrptr = *Bptrptr;
		*Bptrptr = aux;
	}
	
	if((*Bptrptr)->y > (*Cptrptr)->y)
	{
		aux = *Bptrptr;
		*Bptrptr = *Cptrptr;
		*Cptrptr = aux;
	}
	
	//if Aprt==Bptr
	
	if((*Cptrptr)->x == (*Aptrptr)->x)
	{	
		if((*Aptrptr)->y > (*Cptrptr)->y)
		{
			aux = *Aptrptr;
			*Aptrptr = *Cptrptr;
			*Cptrptr = aux;
			printf("A>C\n");
		}
		else
		{
			aux = *Cptrptr;
			*Cptrptr = *Aptrptr;
			*Cptrptr = aux;
			printf("A<C\n");
		}
		
	}
	
	
	//printf("Surt: A %f %f, B %f %f, C %f %f \n", (*Aptrptr)->x, (*Aptrptr)->y, (*Bptrptr)->x, (*Bptrptr)->y, (*Cptrptr)->x, (*Cptrptr)->y);
	
}

/*This function, given two points, interpolates the value of the variables x, u and v given a high to start.
ARGS:
punto *A: pointer of the starting point
punto *B: pointer of the ending point
punto *pin: punto where the results shall be stored.
int h: coordinate y where x has to be interpolated 
*/
void calcular_interseccion(punto *A, punto *B, punto *pin, int h)
{

	int x, y;
	punto aux;
	float y_bis, u, v;
	
	y = B->y - A->y;
	x = B->x - A->x;
	u = B->u - A->u;
	v = B->v - A->v;

	y_bis = h - A->y;
	
	//we cannot divide per 0
	if(y != 0)
	{
		pin->x = (int) round(A->x+(x*y_bis/y));
		pin->u = A->u + (u*y_bis/y);
		pin->v = A->v + (v*y_bis/y);
	}
	//if point A and B are aligned, we define to always return the x in the first point
	else if(x==0)
	{
		printf("pin es A \n");
		pin->x = (int) A->x;
	}
	else
	{
		pin->x = (int) (A->x);
		pin->u = A->u;
		pin->v = A->v;
	}

	pin->y = h;
	pin->z = 0;
	
	//printf("%f\n", pin->x);
	
	//printf("%d: pin_u, pin_v: %f, %f\n", h, u, v);
	
}

/*
dibujar_pixel draws a pixel in the coordinates proportioned, and chooses the colour with color_textura.
int x: coord x
int y: coor y
float u: colour at u
float v: colour at v
*/
void dibujar_pixel(int x, int y, float u, float v)
{

	unsigned char* colorv;
	unsigned char r,g,b;
	
	//printf("(%d, %d) %f %f\n",x, y, u,v);
	colorv = color_textura(u, v); 	
	r=colorv[0];
	g=colorv[1];
	b=colorv[2];     
	glBegin( GL_POINTS );
	glColor3ub(r,g,b);
	glVertex3f(x,y,0.);
	glEnd();
}

void linea_triangulo(punto pin_left, punto pin_right, int h)
{
	int x;
	float u_dif, v_dif, u, v;
	
	u = pin_left.u;
	v = pin_left.v;
	
	if(pin_right.x != pin_left.x)
	{
		v_dif = (pin_right.v - pin_left.v)/(pin_right.x - pin_left.x);
		u_dif = (pin_right.u - pin_left.u)/(pin_right.x - pin_left.x); 
	}
	else
	{
		v_dif = 0.0;
		u_dif = 0.0;
	}
	
	//printf("%f, %f\n", u_dif, v_dif);
	
	for(x=pin_left.x; x<=pin_right.x; x++)
	{
		dibujar_pixel(x, h, u, v);
		u = u + u_dif;
		v = v + v_dif;
		//printf("%d %d\n", x, h);
			
	}
	

	//printf("%d: (U, V): (%f, %f)\n", h, u, v);
}

/*
dibujar_triangulo draws the triangle stored in triangulosprt[indice]. It divides the triangle in two, and draws by line from the lowest vertice to the highest, stoping at the middle one to changes how to calculate the intersections.
*/
void dibujar_triangulo()
{
	
	int h;
	punto *Aptr, *Bptr, *Cptr;
	punto pin_left, pin_right;

	determinar_orden(&Aptr, &Bptr, &Cptr);
	
	h = 0;
	
	//if A and C are aligned, we draw the pixels between them
	if((Aptr->y==Cptr->y))
	{
		printf("Entro A=C\n");
		calcular_interseccion(Aptr, Cptr, &pin_left, h);
		calcular_interseccion(Cptr, Aptr, &pin_right, h);
		
		linea_triangulo(pin_left, pin_right, Aptr->y);
	}
	//in the same way, if A and B are aligned, we draw the pixels between them
	if(Aptr->y==Bptr->y)
	{
		printf("Entro A=B\n");
		calcular_interseccion(Aptr, Bptr, &pin_left, h);
		calcular_interseccion(Bptr, Aptr, &pin_right, h);
		
		linea_triangulo(pin_left, pin_right, Aptr->y);
	}
	
  	//the first for goes from A-> (the nearest from 0) to B->y.
	for(h=Aptr->y; h<=Bptr->y; h++)
	{
		calcular_interseccion(Aptr, Cptr, &pin_left, h);
		calcular_interseccion(Aptr, Bptr, &pin_right, h);
		
		if(pin_left.x < pin_right.x)
			linea_triangulo(pin_left, pin_right, h);
		else
			linea_triangulo(pin_right, pin_left, h);
		
		//printf("INT: %f, %f\n", pin_left.x, pin_right.x);
		//printf("LEFT: %f, %f\n", pin_left.u, pin_left.v);
		//printf("RIGHT: %f, %f\n", pin_right.u, pin_right.v);
		
		
	}
	//the second goes from B to C, thus making the h all the way up to 500
	for(; h<=Cptr->y; h++)
	{

		calcular_interseccion(Cptr, Bptr, &pin_left, h);
		calcular_interseccion(Cptr, Aptr, &pin_right, h);
		

		//printf("INT: %d, %d\n", pin_left, pin_right);
		//printf("LEFT: %f, %f\n", pin_left.u, pin_left.v);
		//printf("RIGHT: %f, %f\n", pin_right.u, pin_right.v);
		
		if(pin_left.x < pin_right.x)
			linea_triangulo(pin_left, pin_right, h);
		else
			linea_triangulo(pin_right, pin_left, h);

	}
	
			
}

static void marraztu(void)
{
	float u,v;
	float i,j;
	unsigned char* colorv;
	unsigned char r,g,b;

	// borramos lo que haya...
	glClear( GL_COLOR_BUFFER_BIT );

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 500.0, 0.0, 500.0,-250.0, 250.0);

	/*Drawing the triangles and printing the foto*/
	dibujar_triangulo();

	glFlush();
	
}
// This function will be called whenever the user pushes one key
static void teklatua (unsigned char key, int x, int y)
{
	switch(key)
		{
		case 13: // <INTRO>
			

			//indice iterates from 0 to num_triangles
			indice=(indice+1)%num_triangles;
			//printf("%d indice\n", indice);
			
			
			printf("Triangulo %d/%d\n", indice+1, num_triangles);
			printf ("Pulsa ENTER para dibujar el siguente triangulo\n");
			
			
			break;
		case 27:  // <ESC>
			exit( 0 );
			break;
		default:
			printf("%d %c\n", key, key );
		}

	// The screen must be drawn to show the new triangle
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	indice = 0;	
	num_triangles = 1; //TODO: comptar les t al fitxer.
	triangulosptr = 0; //inicialitzem el punter a null.
	
	printf("This program draws points in the viewport \n");
	printf("Press <ESC> to finish\n");
	glutInit(&argc,argv);
	glutInitDisplayMode ( GLUT_RGB );
	glutInitWindowSize ( 500, 500 );
	glutInitWindowPosition ( 100, 100 );
	glutCreateWindow( "GL_POINTS" );

	glutDisplayFunc( marraztu );
	glutKeyboardFunc( teklatua );
	/* we put the information of the texture in the buffer pointed by bufferra. The dimensions of the texture are loaded into dimx and dimy */ 
    	load_ppm("foto.ppm", &bufferra, &dimx, &dimy);
    	          
    	/*Loading the contents of triangles.txt*/
    	cargar_triangulos(&num_triangles, &triangulosptr);
        
	glClearColor( 0.0f, 0.0f, 0.7f, 1.0f );

	glutMainLoop(); //espera per caputrar un event.

	return 0;   
}
