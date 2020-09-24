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

/*
VARIABLES GLOBALS
*/

// texturaren informazioa

extern void load_ppm(char *file, unsigned char **bufferptr, int *dimxptr, int * dimyptr);
unsigned char *bufferra;
int dimx,dimy;

int indice, num_triangles;
hiruki *triangulosptr; //pointer to triangles.


unsigned char * color_textura(float u, float v)
{
/* debe devolver un puntero al pixel adecuado, no al primero!! */
return(bufferra);
}

void print_triangle(hiruki triangulo)
{
	printf("A: x: %f, y: %f, z: %f\nB: x: %f, y: %f, z: %f\nC: x: %f, y: %f, z: %f\n", triangulo.p1.x, triangulo.p1.y, triangulo.p1.z, triangulo.p2.x, triangulo.p2.y, triangulo.p2.z, triangulo.p3.x, triangulo.p3.y, triangulo.p3.z);
}

void ordenar_vertices(hiruki *triangulo)
{
	punto aux;
	
	
	if(triangulo->p1.y > triangulo->p3.y)
	{
		printf("Canvio A per C\n");
		aux = triangulo->p1;
		triangulo->p1 = triangulo->p3;
		triangulo->p3 = aux;
	}
	/*
	if(triangulo->p1.y > triangulo->p2.y)
	{
		printf("Canvio A per B\n");
		triangulo->p1 = triangulo.p2;
		triangulo->p2 = aux;
	}
	
	if(triangulo.p2.y > triangulo.p3.y)
	{
		printf("Canvio B per C\n");
		aux = triangulo.p2;
		triangulo.p2 = triangulo.p3;
		triangulo.p3 = aux;
	}
	*/
	
}

/*
This function gets the triangle hiruki and changes the order of the vertices from higest to lowest in y axis for the higest verticie is always called A and the lowest always C.
*/
void determinar_orden(hiruki t, punto *Aptr, punto *Bptr, punto *Cptr)
{
	printf("Triangle original DINS: %f, %f, %f\n", t.p1.y, t.p2.y, t.p3.y);
	
	punto aux;
	*Aptr = t.p1;
	*Bptr = t.p2;
	*Cptr = t.p3;

	printf("Arriba: %f, %f, %f\n", Aptr->y, Bptr->y, Cptr->y);
	

	if(Aptr->y > Cptr->y)
	{
		printf("Canvio A per C\n");
		aux = *Cptr;
		*Cptr = *Aptr;
		*Aptr = aux;
	
	}

	if(Aptr->y > Bptr->y)
	{
		printf("Canvio A per B\n");
		aux = *Bptr;
		*Bptr = *Aptr;
		*Aptr = aux;
	}
	
	if(Bptr->y > Cptr->y)
	{
		printf("Canvio B per C\n");
		aux = *Cptr;
		*Cptr = *Bptr;
		*Bptr = aux;
	}
	
	/*
	if(Aptr->y == Bptr->y)
	{
		printf("Vertex contiguus iguals: Canvio A per C\n");
		aux = *Cptr;
		*Cptr = *Aptr;
		*Aptr = aux;
	}
	*/
	

	printf("Marxa: %f, %f, %f\n", Aptr->y, Bptr->y, Cptr->y);
	printf("Triangle original: %f, %f, %f\n", t.p1.y, t.p2.y, t.p3.y);
}
//si ho posem com he passat el pin_left,pin_right podria anar bé. Tot i que no entenc ara matiex perque els canvia...

/*punto o directamente void?*/
void calcular_interseccion(punto A, punto B, int *pin, int h)
{
    //nota: no hi posis valors absoluts que es desmadarda que filpes.
	int x, y;
	punto aux;
	float y_bis, prova;
	
	if(B.y<A.y){
		aux = A;
		A = B;
		B = aux;
	}
	
	y = B.y - A.y;
	x = B.x - A.x;
	
	y_bis = h - A.y;
	
	if(y != 0) //just in case...
	{
		*pin = (int) round(A.x+(x*y_bis/y));
	}
	else
	{
		*pin = (int) round(A.x);
	}
	
}

void dibujar_pixel(int x, int y)
{

	float u,v;
	unsigned char* colorv;
	unsigned char r,g,b;
	
	u = x/500.0;
	v = y/500.0;
	colorv = color_textura(u, v); //TODO: si esta función es correcta se ve la foto en la ventana
	r= colorv[0];
	g=colorv[1];
	b=colorv[2];     
	glBegin( GL_POINTS );
	glColor3ub(r,g,b);
	glVertex3f(x,y,0.);
	glEnd();
}


void dibujar_triangulo(hiruki triangulo)
{

	/*
	La definición del triangulo es la siguiente:
	VERTICE A: el vertice con las y mas cercanas a 0
	VERTICE C: el vertice con las y mas lejanas a 0
	VERTICE B: el sobrante.
	*/
	
	int h,x;
	punto Aptr, Bptr, Cptr;
	int pin_left, pin_right, aux;
	
	h = 0;
	x = 0;
	aux = 0;
	
	/*
	This pointers have to be inicialitzed pointing to a point struct because the program 	runs itself a first time without opening openGL. If you point null, the for above does 		not work in any way due to not being albe to acess y, therefore segmentation fault.
	*/
	Aptr = triangulo.p1;
	Bptr = triangulo.p2;
	Cptr = triangulo.p3;

	determinar_orden(triangulo, &Aptr, &Bptr, &Cptr);

    	//the first for goes from A-> (the nearest from 0) to B->y.
	for(h=Aptr.y; h<=Bptr.y; h++)
	{
		calcular_interseccion(Aptr, Cptr, &pin_left, h);
		calcular_interseccion(Aptr, Bptr, &pin_right, h);
		
		/*
		Here we determine which intersection point is adequate. If left is bigger than right, we swap them
		*/
		if(pin_left>=pin_right)
		{
			aux = pin_left;
			pin_left = pin_right;
			pin_right = aux;
		}
		
		//printf("%d, %d\n", pin_left, pin_right);
		
		for(x=pin_left; x<=pin_right; x++)
		{
			dibujar_pixel(x, h);
		}
	}
	//the second goes from B to C, thus making the h all the way up to 500
	for(h=Bptr.y; h<=Cptr.y; h++)
	{

		calcular_interseccion(Aptr, Cptr, &pin_left, h);
		calcular_interseccion(Bptr, Cptr, &pin_right, h);
		
		if(pin_left>=pin_right)
		{
			aux = pin_left;
			pin_left = pin_right;
			pin_right = aux;
		}
		
		//printf("%d, %d\n", pin_left, pin_right);
		
		for(x=pin_left; x<=pin_right; x++)
		{
			dibujar_pixel(x, h);
		}
	}
		
}

/*
void dibujar_tiangulo(hiruki triangulo)
{

1. determinar on estan ubicats el punts. Quin és el més alt bla bla. determinar_orden(hiruki, &altoptr, &medioptr, &bajoptr)
	for(h=altoptr->y;h>medioprt->y;h--) //controlar quan els punts estiguin alienats/dos siguin el mateix.
	{
		calcular_interseccion(altoptr, bajoptr, &pinterseccion1)
		calcular_interseccion(altoptr, bajoptr, &pinterseccion2)
		for(x=izda(pin1,pin2);x<=dcha(pin1,pin2); x++){
			dibujar(x,h,color);
		}
	}
	for(; h>bajoptr->y;h--){
		//lo mismo que arriba cambiando los puntos
	}

}
*/
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

	// pero hay que llamar a la función que dibuja un triangulo con la textura mapeada:
	
	dibujar_triangulo(triangulosptr[indice]);

	glFlush();

	/* Aquest bucle dibuixa tots els píxels de la pantalla, no res més. Això s'ha de fer a dibujar_triangulo adequadament.
	for (i=0;i<500;i++)
	    for (j=0;j<500;j++)
		{
			u = i/500.0;
			v = j/500.0;
			colorv = color_textura(u, v); //TODO: si esta función es correcta se ve la foto en la ventana
			r= colorv[0];
			g=colorv[1];
			b=colorv[2];     
			glBegin( GL_POINTS );
			glColor3ub(r,g,b);
			glVertex3f(i,j,0.);
			glEnd();
		}
	glFlush();
	*/
}
// This function will be called whenever the user pushes one key
static void teklatua (unsigned char key, int x, int y)
{
	switch(key)
		{
		case 13: // <INTRO>
			
			
			/*indice itera de 0 a num_triangles*/
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

	glutDisplayFunc( marraztu ); // Aquí escriure com es dibuixen les imatges.
	glutKeyboardFunc( teklatua ); // Aquí arrelgar el bucle (apretar intro avançar, apretar esc seguir.)
	/* we put the information of the texture in the buffer pointed by bufferra. The dimensions of the texture are loaded into dimx and dimy */ 
    	load_ppm("foto.ppm", &bufferra, &dimx, &dimy);
              
    	/*Loading the contents of triangles.txt*/
    	cargar_triangulos(&num_triangles, &triangulosptr);
        
	glClearColor( 0.0f, 0.0f, 0.7f, 1.0f );

	glutMainLoop(); //espera per caputrar un event.

	return 0;   
}
