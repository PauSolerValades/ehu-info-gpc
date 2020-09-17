#include <stdio.h>
#include <stdlib.h>

/*
Manies del senyor aquest:
1. Totes les variables es declaren al principi de cada funció/programa i després s'instancien. Les variables pels fors es reutilitzen i ja està
2. La clau de les funcions es posa sota de la seva declaració, no al costat. 
*/

typedef struct elem
{
int bal;
struct elem *hptr;
} elem;

// TODO code to insert an element into the list in ascending order

void insert_in_ascendent_order(elem **pptrptr, elem *nptr)
{
	int n;

	n = nptr->bal;
	
	if(n < (*pptrptr->bal))
	{
	
	}
}


int main(int argc, char *arg[])
{
int entrada,valor;
elem *first_ptr,*lptr;
char kaka[80];

first_ptr = 0;
printf("insert number\n  ");
for ( valor = scanf("%d",&entrada);
      entrada != 0;
      valor = scanf("%d",&entrada) )
    {
    if (valor > 0)
        {
        printf("%d has been read\n",entrada);
        // create elem with the value inserted from keyboard
        lptr = (elem *) malloc(sizeof(elem));
        lptr->bal= entrada;
        // this code inserts the element into the list pointed by leptr as the first element of the list (LIFO).
        
        // TODO Change the code to insert the element in ascendent order: erase the two lines above and use a function 
        // void insert_in_ascendent_order(...) with two parameters, one for the list and the second for the element. How should be passed the parameters?
        insert_in_ascendent_order(&first_ptr, lptr);
        }
      else
        {
        printf("only numbers!!!\n\n");
        exit(0);
        }
      printf("insert number\n  ");
    }
for (lptr = first_ptr; lptr != 0; lptr = lptr->hptr)
    printf( "valor = %d\n",lptr->bal);
}
