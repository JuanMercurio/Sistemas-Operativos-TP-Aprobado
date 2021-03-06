#ifndef _ESQUEMA_PAGINACION_
#define _ESQUEMA_PAGINACION_

#include "estructuras.h"

#include <stdint.h>

#define PAGINA        segmento
#define MARCO         segmento
#define NO_VACIO             1
#define VACIO                0
#define NOT_ASIGNED         -1

/* MEMORIA PRINCIPAL */
extern memoria_t ram;

/* "Bitmap" de marcos */
extern t_list* marcos; 

extern bool primero;

/*
    @NAME:  iniciar_paginacion
    @DESC:  inicia todas las estructuras necesarias 
            para utilizar paginacion
 */
void iniciar_paginacion();

/*
    @NAME:  nro_marco
    @DESC:  retorna un numero de marco segun una pagina y la tabla del proceso.
            se encarga de buscar y hacer los reemplazos que se requieren
 */
int nro_marco(int pagina, tab_pags* tabla, int codigo);

/*
    @NAME:  init_ram
    @DESC:  aloca la memoria principal y inicia el mutex
 */
void init_ram();

/*
    @NAME:  init_bitmap_frames
    @DESC:  iniciar el bitmap de frames de la memoria fisica
 */
void init_bitmap_frames();

/*
    @NAME:  init_estructuras
    @DESC:  inicia las estrucutas de paginacion
 */
void init_estructuras();

/*
    @NAME:  crear_pagina
    @DESC:  crea una pagina vacia y retorna el numero de pagina
 */
int crear_pagina(t_list *paginas);

/*
    @NAME:  marco_libre
    @DESC:  retorna el numero de marco libre
            si no hay libre recorda NOT_ASIGNED
 */
int marco_libre();

/*
    @NAME:  add_new_page_table
    @DESC:  agrega un tab_pag* a la lista de todos ellos 
            Es thread safe
 */
void add_new_page_table(tab_pags*);


/*
    @NAME:  pagina_valida
    @DESC:  nos dice sin pedido de pagina es valido
 */
bool pagina_valida(tab_pags* tabla, int pagina);


/*
    @NAME:  buscar_en_tabPags
    @DESC:  busca en la tabla de paginas del proceso si 
            la pagina esta presente en memoria y retorna su numero de frame
            si no esta en memoria retorna -1
 */
pag_t* buscar_en_tabPags(tab_pags* tabla, int pagina);

/*
    @NAME:  buscar_en_swap
    @DESC:  busca en swap la pagina que queremos.
            Se encarga de todas las tareas administrativas
            que requiere pedir una pagina a memoria virtual
 */
int buscar_en_swap(tab_pags* tabla, int pagina);

/*
    @NAME:  reemplazar_pagina
    @DESC:  reemplaza la pagina "victima" con "pagina"
            se encarga de actualizar swamp si "victima" 
            fue modificada
 */
void reemplazar_pagina(t_victima victima, void* buffer, int pagina, tab_pags* tabla);

/*
    @NAME:  actualizar_nueva_pagina
    @DESC:  actualiza el marco de "pagina"
 */
void actualizar_nueva_pagina(int pagina, int marco, tab_pags* tabla);

/*
    @NAME:  insertar_pagina
    @DESC:  inserta el contenido de "pagina" 
            en memoria segun "marco"
 */
void insertar_pagina(void* pagina, int marco);

/* 
    @NAME:  recibir_contenido
    @DESC:  recibe el contenido de una pagina desde swap
 */
void* recibir_contenido(int swap);

/* 
    @NAME:  serializar_pedido_pagina
    @DESC:  recibe el contenido de una pagina desde swap
 */
void* serializar_pedido_pagina( int pid, int pagina);

/* 
    @NAME:  enviar_pagina_a_swap
 */
void enviar_pagina_a_swap(int pid, int pagina, int marco);

/* 
    @NAME:  enviar_pagina_a_swap
 */
void* recibir_marco(int cliente);

/*
    @NAME:  set_asignacion
    @DESC:  segun el config les da valoes a las funciones puntero
 */
void set_asignacion();

/*
    @name:  obinario_a_decimal
    @desc:  dado un binario retorna su decimal
 */
int binario_a_decimal(uint32_t binario);

/*
    @NAME:  buscar_page_table
    @DESC:  segun un pid busca su tabla de paginas
            no es thread safe
 */

tab_pags* buscar_page_table(int pid);
/*
    @NAME:  offset_memoria
    @DESC:  dada una dir_t fisica retorna el byte en memoria
 */
uint32_t crear_dl(dir_t dl);

/*
    @NAME:  traducir_dir_log
    @DESC:  reduce una direccion logica en un dir_t logico
 */
dir_t traducir_dir_log(uint32_t dir_log);

/*
    @NAME:  convertir_a_df
    @DESC:  Dado una tabla y un dir_t, retorna un dir_t fisico
 */
dir_t convertir_a_df(t_list* tabla, dir_t dl);

/*
    @NAME:  offset_memoria
    @DESC:  dada una dir_t fisica retorna el byte en memoria
 */
int offset_memoria(dir_t df);

/*
    @name:  decimal_a_binario
    @desc:  dado un decimal retorna su binario
 */
uint32_t decimal_a_binario(int decimal);

/*
    @NAME:  oget_offset
    @DESC:  retorna el desplazamiento de un dl binario
 */
int get_offset(uint32_t dl, int multiplier);

bool marcos_maximos_asignados(tab_pags* tabla);

int paginas_presentes(tab_pags* t);

void memoria_asignar_pagina_vacia(tab_pags* tabla, int pagina, int marco);

int pagina_iniciar(tab_pags *tabla);

dir_t decimal_a_dl(int dir_log);

int dl_a_decimal(dir_t dl);

void actualizar_victima_de_tlb(tlb_t* reg);

void tlb_limpiar_registro(int pid, int pagina);

pag_t* obtener_pagina(int pid, int pagina);

int proceso_puede_iniciar(tab_pags* tabla);

bool proceso_tiene_frames_asignados(tab_pags* tabla);

void pagina_liberar(pag_t *pagina, int n_pagina, int pid);

void tablas_imprimir_saturno();

void tablas_loggear_saturno();

void marco_ocupar(int marco, int pid, int pagina);

bool fija_es_valido_iniciar(tab_pags* tabla, int marco);

void pagina_liberar_tlb(int pid, int pagina);

void pagina_liberar_marco(pag_t *pagina);

bool dinamica_marco_libre(int marco);
#endif
