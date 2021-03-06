#include "config_pag.h"

#include <mensajes/mensajes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



int swap = -1;

int (*algoritmo_tlb)();
void (*tlb_page_use)(tlb_t* reg);
int (*alg_comportamiento)();
t_victima (*algoritmo_mmu)(int, tab_pags*);
int (*alg_comportamiento_tlb)();

void set_algoritmos(){
    set_alg_tlb();
    set_alg_principal();
}

void set_alg_tlb(){

     if ( strcmp(configuracion.ALGORITMO_REEMPLAZO_TLB, "FIFO") == 0 ) 
          { 
               algoritmo_tlb = fifo_tlb;
               tlb_page_use = tlb_page_use_fifo;
               alg_comportamiento_tlb = alg_comportamiento_tlb_fifo;
               return;
          }
     if ( strcmp(configuracion.ALGORITMO_REEMPLAZO_TLB, "LRU")  == 0 )
          { 
               algoritmo_tlb = lru_tlb;
               tlb_page_use = tlb_page_use_lru;
               alg_comportamiento_tlb = alg_comportamiento_tlb_lru;
               return;
          }

    printf("Configuracion Invalida en ~/memoria/cgf/memoria.config:  \n\n");
    printf("    - ALGORITMO_REEMPLAZO_TLB=%s --> Elegir entre FIFO y LRU \n\n", configuracion.ALGORITMO_REEMPLAZO_TLB);

    abort();
}

void set_alg_principal(){

    if ( strcmp(configuracion.TIPO_ASIGNACION, "FIJA") == 0 && \
         strcmp(configuracion.ALGORITMO_REEMPLAZO_MMU, "CLOCK-M") == 0)        
       { 
          algoritmo_mmu = clock_fijo;
          alg_comportamiento = alg_comportamiento_clock_modificado;
          return;
       }
                                                                               

    if ( strcmp(configuracion.TIPO_ASIGNACION, "DINAMICA") == 0 && \
         strcmp(configuracion.ALGORITMO_REEMPLAZO_MMU, "CLOCK-M") == 0)     
     {
          algoritmo_mmu = cd; 
          alg_comportamiento = alg_comportamiento_clock_modificado;
          return; 
     }

    if ( strcmp(configuracion.TIPO_ASIGNACION, "FIJA") == 0 && \
         strcmp(configuracion.ALGORITMO_REEMPLAZO_MMU, "LRU") == 0)
     {
          algoritmo_mmu = lru_fijo; 
          alg_comportamiento = alg_comportamiento_lru;
          return; 
     }

    if ( strcmp(configuracion.TIPO_ASIGNACION, "DINAMICA") == 0 && \
         strcmp(configuracion.ALGORITMO_REEMPLAZO_MMU, "LRU") == 0)
     {
          algoritmo_mmu = lru_dinamico; 
          alg_comportamiento = alg_comportamiento_lru;
          return; 
     }
         
    printf("Configuracion Invalida en ~/memoria/cgf/memoria.config:  \n\n");
    printf("    - ALGORITMO_REEMPLAZO_MMU=%s --> Elegir entre CLOCK-M y LRU \n", configuracion.ALGORITMO_REEMPLAZO_MMU);
    printf("    - TIPO_ASIGNACION=%s --> Elegir entre FIJA y DINAMICA \n\n",     configuracion.TIPO_ASIGNACION);

    abort();
}

int set_campo_algoritmo(){
    if(strcmp(configuracion.ALGORITMO_REEMPLAZO_MMU, "CLOCK-M") == 0) return 0;
    if(strcmp(configuracion.ALGORITMO_REEMPLAZO_MMU, "LRU") == 0) return LRU_C;
    printf("Algoritmo de MMU erroneo\n");
    printf("Terminando el programa");
    abort();
}


void asignacion_swap(){
     enviar_mensaje(swap, configuracion.TIPO_ASIGNACION);
}

void swap_conexion_init(int server){
     swap = aceptar_cliente(server);
     asignacion_swap();
}