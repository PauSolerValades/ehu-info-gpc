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


// texturaren informazioa

extern void load_ppm(char *file, unsigned char **bufferptr, int *dimxptr, int * dimyptr);
unsigned char *bufferra;
int dimx,dimy;

unsigned char * color_textura(float u, float v)
{
/* debe devolver un puntero al pixel adecuado, no al primero!! */
return(bufferra);
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

	// por ahora dibujamos todos los pixels de la ventana de 500x500 con el color que devuelve la función color_textura
	// pero hay que llamar a la función que dibuja un triangulo con la textura mapeada:
	/*
	dibujar_triangulo(triangulosptr[indice]);
	*/
	for (i=0;i<500;i++)
	    for (j=0;j<500;j++)
		{
		u = i/500.0;
		v = j/500.0;
		colorv=  color_textura(u, v); // si esta función es correcta se ve la foto en la ventana
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
		case 13: 
			printf ("ENTER: que hay que dibujar el siguiente triángulo.\n");
			/* hacer algo para que se dibuje el siguiente triangulo */
			/*
			indice ++;  // pero si es el último? hay que controlarlo!
			*/
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

	int num_triangles;
	hiruki *triangulosptr; //aquest és el punter que apuntarà a la linked list de triangles.
	
	num_triangles = 3; //hi ha 3 triangles a triangles.txt. Es podria fer una funció que obrís el fitxes i en comptes quantes t té.
	*triangulosptr = 0; //inicialitzem el punter a null.

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
        
        /* preguntar si definint un nombre com a int, si l'argument de la funció és un punter i li passes la direcció de memòria, la funció rep el punter que apunta al valor de la direcció de memòria de l'int.
        preguntar si el triangulos ptr hem de passat un punter d'un struct hiruki, per anar apuntant als triangles del fitxer triangles.txt.
        */
        
        /***** lee la información de los triángulos... pero hay que definir las variables! 
        cargar_triangulos(&num_triangles, &triangulosptr)
        ****/
	glClearColor( 0.0f, 0.0f, 0.7f, 1.0f );

	glutMainLoop();

	return 0;   
}
