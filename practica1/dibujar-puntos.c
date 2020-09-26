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

x=250 (del 0 al 249)
y=800 (del 0 al 799)
u=0.25
v=0.75
buffer[0] = primera fila primera columna
buffer[1] = primera fila segona columna
buffer[n] = primera fila n columna
buffer[n+1] = segona fila primera columna
buffer[n^2-1] = última fila ultima columna. 

en n*n
bufferra[i,j] =  

bufferra + [199*250+63]*3
volem doncs u*x = 63, (1-v)*y = 199.
*/
unsigned char * color_textura(float u, float v)
{

	int i, j;
	
	i = trunc(u*(dimx));
	j = trunc((1-v)*(dimy));

return(bufferra + 3*(j*dimx + i));
}

/*
This function gets the triangle hiruki and changes the order of the vertices from higest to lowest in y axis for the higest verticie is always called A and the lowest always C.
*/

/*Això es pot fer sense passar el triangle i usant que és una vairable global, però necessites un doble punter, el primer a on és el triangle i el segon al punt concret*/
void determinar_orden(hiruki t, punto *Aptr, punto *Bptr, punto *Cptr)
{
	
	punto aux;
	*Aptr = t.p1;
	*Bptr = t.p2;
	*Cptr = t.p3;


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
	
}

/*This function, given two points and a entrance, gives the image of f(x) = line between A and B*/
void calcular_interseccion(punto A, punto B, punto *pin, int h)
{

	int x, y;
	punto aux;
	float y_bis, u, v;
	
	y = B.y - A.y;
	x = B.x - A.x;
	u = B.u - A.u;
	v = B.v - A.v;

	y_bis = h - A.y;
	
	if(y != 0)
	{
		pin->x = (int) round(A.x+(x*y_bis/y));
		pin->u = A.u + (u*y_bis/y);
		pin->v = A.v + (v*y_bis/y);
	}
	else
	{
		pin->x = (int) round(A.x);
		pin->u = A.u;
		pin->v = A.v;
	}

	pin->y = h;
	pin->z = 0;
	
	//printf("%d: pin_u, pin_v: %f, %f\n", h, u, v);
	
}


//(x, y, r,g,b) D'on putes treus els valors de rgb? de bufferra amb u i v, entenc que es una matriu.
void dibujar_pixel(int x, int y, float u, float v)
{

	unsigned char* colorv;
	unsigned char r,g,b;
	
	//printf("(%d, %d) %f %f\n",x, y, u,v);
	colorv = color_textura(u, v); 	
	r= colorv[0];
	g=colorv[1];
	b=colorv[2];     
	glBegin( GL_POINTS );
	glColor3ub(r,g,b);
	glVertex3f(x,y,0.);
	glEnd();
}

void linea_triangulo(punto pin_left, punto pin_right, int h)
{
	int x, a;
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
			
	}
	

	//printf("%d: (U, V): (%f, %f)\n", h, u, v);
}


void dibujar_triangulo(hiruki triangulo)
{
	
	int h,x;
	punto Aptr, Bptr, Cptr, pin_left, pin_right, aux;
		
	h = 0;
	

	determinar_orden(triangulo, &Aptr, &Bptr, &Cptr);

    	//the first for goes from A-> (the nearest from 0) to B->y.
	for(h=Aptr.y; h<=Bptr.y; h++)
	{
		calcular_interseccion(Aptr, Cptr, &pin_left, h);
		calcular_interseccion(Aptr, Bptr, &pin_right, h);
		
		/*
		Here we determine which intersection point is adequate. If left is bigger than right, we swap them
		*/
		
		//printf("INT: %d, %d\n", pin_left, pin_right);
		//printf("LEFT: %f, %f\n", pin_left.u, pin_left.v);
		//printf("RIGHT: %f, %f\n", pin_right.u, pin_right.v);
		
		if(pin_left.x < pin_right.x)
			linea_triangulo(pin_left, pin_right, h);
		else
			linea_triangulo(pin_right, pin_left, h);
		
	}
	//the second goes from B to C, thus making the h all the way up to 500
	for(h=Bptr.y; h<=Cptr.y; h++)
	{

		calcular_interseccion(Aptr, Cptr, &pin_left, h);
		calcular_interseccion(Bptr, Cptr, &pin_right, h);
		

		//printf("INT: %d, %d\n", pin_left, pin_right);
		//printf("LEFT: %f, %f\n", pin_left.u, pin_left.v);
		//printf("RIGHT: %f, %f\n", pin_right.u, pin_right.v);
		
		if(pin_left.x < pin_right.x)
			linea_triangulo(pin_left, pin_right, h);
		else
			linea_triangulo(pin_right, pin_left, h);

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

	/*Drawing the triangles*/
	dibujar_triangulo(triangulosptr[indice]);

	glFlush();


	/*
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
