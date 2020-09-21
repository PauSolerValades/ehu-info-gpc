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


/*
This function gets the triangle hiruki and changes the order of the vertices from higest to lowest in y axis for the higest verticie is always called A and the lowest always C.

Probablement aquesta funció no serveixi de res. En plan, segurament no serveix.

Podria canviar la definició dels punts per només canviar coordenades i deixar el color igual, però tocar els structs del profe no es la millor idea.
*/

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

/*Això no funciona encara, falta determinar com. 
Idees: ordenar una còpia dels punts en una llista, després ja faria punters fins als punts REALS del triangle
*/
void determinar_orden(hiruki triangulo, punto *Aptr, punto* Bptr, punto* Cptr)
{
	
	punto aux;
	*Aptr = triangulo.p1;
	*Bptr = triangulo.p2;
	*Cptr = triangulo.p3;
	
	if(triangulo.p1.y > triangulo.p3.y)
	{
		printf("Canvio A per C\n");
		*Aptr = triangulo.p3;
		*Cptr = triangulo.p1;
	}
	
	if(triangulo.p1.y > triangulo.p2.y)
	{
		printf("Canvio A per B\n");
		*Aptr = triangulo.p2;
		*Bptr = triangulo.p1;
	}
	
	if(triangulo.p2.y > triangulo.p3.y)
	{
		printf("Canvio B per C\n");
		*Aptr = triangulo.p2;
		*Bptr = triangulo.p1;
	}
}

/*punto o directamente void?*/
void calcular_interseccion(punto A, punto B, float *pin, int h)
{
	int x, y;
	float y_bis, prova;
	
	y = B.y - A.y;
	x = B.x - A.x;
	
	y_bis = h - A.y;
	
	*pin = A.x+(x*y_bis/y);
	
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
	punto *Aptr, *Bptr, *Cptr;
	float pin1, pin2;
	
	//aquests valors s'hauran de carregar de determinar_orden.
	
		
	Aptr = &triangulo.p1;
	Bptr = &triangulo.p2;
	Cptr = &triangulo.p3;
	
	
	//determinar_orden(triangulo, &Aptr, &Bptr, &Cptr);
	
	/*
	print_triangle(triangulo);
	
	//La lamamos con & para passar EL VALOR EXACTO, no una cópia de los puntos.
	determinar_orden(triangulo, &triangulo.p1, &triangulo.p2, &triangulo.p3);
	*/

	/*
	for(h=Aptr->y; h>Bptr->y; h--)
	{
		calcular_interseccion(*Aptr, *Bptr, &pin1, h);
		
	}
	*/
	calcular_interseccion(*Aptr, *Bptr, &pin1, 150);
	calcular_interseccion(*Cptr, *Bptr, &pin2, 150);
	printf("%f, %f\n", pin1, pin2);
		
	
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

	// por ahora dibujamos todos los pixels de la ventana de 500x500 con el color que devuelve la función color_textura
	// pero hay que llamar a la función que dibuja un triangulo con la textura mapeada:
	/* //TODO: amb les coordenanades del triangle treus les equacions de les rectes al pla i ho poses com a condició del for, molt semblant a com està escrit ja.
	dibujar_triangulo(triangulosptr[indice]); //aquí el podem cridar com una llista, ja que el punter d'una taula és el mateix que el punter d'una llista, excepte que aquest el podem iterar.


	*/
	
	dibujar_triangulo(triangulosptr[indice]);

	/* Aquest bucle dibuixa tots els píxels de la pantalla, no res més. Això s'ha de fer a dibujar_triangulo adequadament.*/
	for (i=0;i<500;i++)
	    for (j=0;j<500;j++)
		{
			u = i/500.0;
			v = j/500.0;
			colorv=  color_textura(u, v); //TODO: si esta función es correcta se ve la foto en la ventana
			r= colorv[0];
			g=colorv[1];
			b=colorv[2];     
			glBegin( GL_POINTS );
			glColor3ub(r,g,b);
			glVertex3f(i,j,0.);
			glEnd();
		}
	glFlush();
}
// This function will be called whenever the user pushes one key
static void teklatua (unsigned char key, int x, int y)
{
	switch(key)
		{
		case 13: // <INTRO>
			
			
			/*indice itera de 0 a num_triangles*/
			indice=(indice+1)%num_triangles;
			
			printf("Triangulo %d/%d\n", indice+1, num_triangles);
			printf ("Pulsa ENTER para dibujar el siguente triangulo\n");
			
			//printf("%d indice\n", indice);
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
	num_triangles = 3; //TODO: comptar les t al fitxer.
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
