#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "array.h"

#define MIN_CNT 10
#define MAX_CNT 20

void print_array(const char* name, array_t* array)
{
	printf("%s =", name);
	for ( size_t index = 0; index < array_get_count(array); ++index ){
		printf(" %zu", (size_t)array_get_element(array, index));
	}
	putchar('\n');
	fflush(stdout);
}

int main(){
	array_t* array1 = array_create(MAX_CNT);//MAX COUNT
	array_t* array2 = array_create(MAX_CNT);//MAX COUNT

	srand( (unsigned int)((unsigned long)time(NULL) + clock()) );
	for ( size_t index = 0, count = MIN_CNT + rand() % MAX_CNT; index < count; ++index ){
		array_append( array1, (void*)(10 + (size_t)rand() % 90) );
		array_append( array2, (void*)(100 + (size_t)rand() % 900) );
	}

	print_array("array1", array1);
	print_array("array2", array2);

	array_destroy(array1);
	array_destroy(array2);

	return 0;
}

// funcion reentrante: invocaciones pasadas no afectan a invocaciones posteriores -> no son versatiles
// ejemplo de funcion no reentrante: strtok(string, ".")
//									strtok(NULL, ".") -> sigue buscando con variable global
// Por lo que NO USAR VARIABLES GLOBALES NI DATOS ESTATICOS EN EL SEGMENTO DE DATOS
