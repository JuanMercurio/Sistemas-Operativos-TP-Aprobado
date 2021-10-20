#ifndef _MAIN_KERNEL_
#define _MAIN_KERNEL_

#include "tests/tests.h"
#include "configuracion/config.h"

#include <utils/utils.h> 
#include <conexiones/conexiones.h>
#include <semaphore.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
// type struct
typedef struct{
    u_int32_t pid;
}t_pcb;

typedef enum{
    NEW_INSTANCE,
    IO,
    INIT_SEMAFORO,
    SEM_WAIT,
    SEM_POST,
    SEM_DESTROY,
    MEMALLOC,
    MEMFREE,
    MEMREAD,
    MEMWRITE
}cod_op;

typedef struct{
    char* id;
    int val;
    int max_val;
    t_queue* bloqueados;
}sem_kernel;

typedef struct{
    char* id;
    int retardo;
    t_queue* bloqueados;
}io_kernel;

//   colas
t_queue *cola_new;
t_queue *cola_ready;
t_queue *suspendido_bloqueado;
t_queue *suspendido_listo;
// listas
t_list lista_ejecutando;
t_list *lista_sem_kernel;
t_list *lista_io_kernel;

// semaforos
sem_t *cola_new_con_elementos;
sem_t *cola_ready_con_elementos;
sem_t *cola_suspendido_bloquedo_con_elementos;
sem_t *cola_suspendido_listo_con_elementos;
sem_t *lista_ejecutando_con_elementos;
sem_t *mutex_cola_new;
sem_t *mutex_cola_ready;
sem_t *mutex_cola_suspendido;
sem_t *mutex_cola_suspendido;
sem_t *mutex_lista_ejecutando;
sem_t *mutex_lista_sem_kernel;
sem_t *mutex_lista_io_kernel;


/*
    @NAME: terminar_programa
    @DESC: Se encarga de liberar todas las estructuras y de hacer lo necesario para
           que el programa termine sin memory leaks y sin errores
 */
void terminar_programa();

void iniciar_colas();

void incializar_planificacion();

void iniciar_planificador_corto_plazo();

void  iniciar_planificador_mediano_plazo();

void iniciar_planificador_largo_plazo();
   
void crear_estructuras(t_pcb *carpincho);

void inicializar_listas_sem_io();

sem_kernel* buscar_semaforo(char *nombre, t_list *sems);
void sem_kernel_wait(char *nombre, t_pcb *carpincho);
void sem_kernel_post(char *nombre);
void sem_kernel_init(char* nombre, int value);
void sem_kernel_destroy(char* nombre);

io_kernel* buscar_io(char *nombre, t_list *ios);
void init_dispositivos_io();
void call_io(char *nombre, t_pcb *carpincho);
void bloquear_proceso_io(t_pcb *carpincho, io_kernel *io);


void receptor(void*);

#endif