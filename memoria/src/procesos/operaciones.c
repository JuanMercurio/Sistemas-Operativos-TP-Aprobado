#include "operaciones.h"
#include "../configuracion/config.h"
#include "../esquema/paginacion.h"

#include <string.h>
#include <stdlib.h>

#define SIZE_METADATA sizeof(HeapMetadata)

int memalloc(int tamanio, int pid){ //quizas al igual que antes, el carpincho se guarda local en el hilo? entonces no habría que pasarlo como parametro

	tab_pags* tabla_paginas = buscar_page_table(pid);
	HeapMetadata* ptr_potencial_segmento = primer_segmento(tabla_paginas);
	int inicio_actual = 0, pagina_actual=0;

	do{
		if(ptr_potencial_segmento->isFree && ptr_potencial_segmento->nextAlloc - SIZE_METADATA - inicio_actual >= tamanio) {
			//encontre el segmento donde entra
			//ahora debo evaluar si entra justo o si tengo que dividirlo en 2 partes

			if(ptr_potencial_segmento->nextAlloc - SIZE_METADATA - inicio_actual == tamanio){
				//entra justo
				ptr_potencial_segmento->isFree = false;
				return inicio_actual; //retorno la direccion logica
			}

			//tengo que dividir en 2                   			DUDA: que pasa si el lugar que queda es menor al tamaño de un metadata?
			ptr_potencial_segmento->isFree = false;
			int aux_next_alloc = ptr_potencial_segmento->nextAlloc;
			ptr_potencial_segmento->nextAlloc = inicio_actual + SIZE_METADATA + tamanio;


			HeapMetadata* new;
			new->prevAlloc = inicio_actual;
			new->nextAlloc = aux_next_alloc;
			new->isFree = true;

			//si el nuevo metadata esta en otra pagina
			if(hay_cambio_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc)){
				int num_pag_a_traer = pagina_actual + 1; //IMPORTANTE: y si un segmento puede ser mas grande que 1 pagina? es decir, que hay 2 saltos de pagina

				//traigo la siguiente pagina
				pag_t* pagina__a_traer = list_get(tabla_paginas->tabla_pag, num_pag_a_traer);

				//TODO: la cargo en memoria

				memcpy(ubicacion_nuevo_segmento(pagina__a_traer->marco, ptr_potencial_segmento->nextAlloc), new, SIZE_METADATA);

				//caso borde, el segmento a cambiarle el prevalloc tambien esta en otra pagina
				if(hay_cambio_de_pagina(ptr_potencial_segmento->nextAlloc, new->nextAlloc)){
					pag_t* pagina_next = list_get(tabla_paginas->tabla_pag, num_pag_a_traer+1);
					//TODO: la cargo en memoria
					HeapMetadata* next = ubicacion_nuevo_segmento(pagina_next->marco, new->nextAlloc);
					next->prevAlloc = ptr_potencial_segmento->nextAlloc;
				}
			}

			memcpy((ram.memoria+ptr_potencial_segmento->nextAlloc), new, SIZE_METADATA); //mal!
			HeapMetadata* next = ram.memoria + aux_next_alloc; //quizas esta mal
			next->prevAlloc = ptr_potencial_segmento->nextAlloc; //el prevAlloc es igual al nextAlloc de 2 metadatas atras

			return inicio_actual;
		}

		//Si no entre al if anterior, significa que no encontre un hueco todavia. Tengo que pasar al siguiente alloc.
		if(hay_cambio_de_pagina(inicio_actual, ptr_potencial_segmento->nextAlloc)){
			//El siguiente metadata esta en otra pagina
			//Obtengo dir fisica de inicio de pagina nueva, luego el metadata esta en [direccion.segmento + configuracion.TAMANIO_PAGINA - inicio_actual]
			pagina_actual++;
		}

		inicio_actual = ptr_potencial_segmento->nextAlloc;
		ptr_potencial_segmento += ptr_potencial_segmento->nextAlloc; //TODO: REVISAR

	} while(ptr_potencial_segmento->nextAlloc != NULL);

	// no hay espacio en ninguna pagina

	//comportamientos distintos: a. si "se genera un nuevo alloc al final del espacio de direcciones" / b. se deniega la operación porque no hay más espacio
}

void* ubicacion_nuevo_segmento(int num_marco, int dir_log){
	return (ram.memoria + num_marco * configuracion.TAMANIO_PAGINAS + dir_log % configuracion.TAMANIO_PAGINAS);
	     // memoria              inicio                                      desplazamiento
}


int num_pagina_a_traer(uint32_t inicio){ //inicio del nuevo metadata
	int contador = 0;
	while (inicio / configuracion.TAMANIO_PAGINAS > 1){
		inicio -= configuracion.TAMANIO_PAGINAS;
		contador++;
	}
	return contador;
}

HeapMetadata* primer_segmento(tab_pags* tabla){
	dir_t direccion_logica;
	direccion_logica.segmento=0;
	direccion_logica.offset=0;

	dir_t direccion_fisica = convertir_a_df(tabla, direccion_logica);
	int offset = offset_memoria(direccion_fisica);

	HeapMetadata* seg = memoria_principal + offset_memoria;
	return seg;
}

bool hay_cambio_de_pagina(int direc_actual, uint32_t next_alloc){
	for (int i=direc_actual+9; i < next_alloc; i++) {
		if (i % configuracion.TAMANIO_PAGINAS == 0) {
			return true;
		}
	}
	return false;
}

void memfree();



void* memread(tab_pags* tabla, int dir_log, int tamanio){

   dir_t dl = traducir_dir_log(dir_log);
   dir_t df = convertir_a_df(tabla->tabla_pag, dl);

   int leido = 0;
   int size_in_page = configuracion.TAMANIO_PAGINAS - tamanio;

   void* buffer = malloc(tamanio);

   while(leido<tamanio)
   {
     if(size_in_page > tamanio)
     {
        memcpy(buffer + leido, ram.memoria + offset_memoria(df), tamanio);
     }
   }

   return NULL;
}


void memwrite();
