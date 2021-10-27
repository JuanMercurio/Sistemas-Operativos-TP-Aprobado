#include "clientes.h"
#include "../esquema/paginacion.h"

#include <matelib/matelib.h>
#include <mensajes/mensajes.h>
#include <utils/utils.h>
#include <sys/socket.h>
#include <pthread.h>

void atender_proceso(void* arg){
    int cliente = *(int*)arg;
    free(arg);

    handshake(cliente, "memoria");
    ejecutar_proceso(cliente);
}

void ejecutar_proceso(int cliente) {

    int pid = PID_EMPTY;
    t_list* tabla;
    while(1) //TODO: feo while
    {
        int operacion = recibir_operacion(cliente);
        switch (operacion)
        {
        case NEW_INSTANCE:
            pid = desginar_PID(cliente);
            tabla = iniciar_paginas(cliente, pid);
            // enviar_PID(cliente);
            //enviar info a carpincho
            break;
        
        case MEMALLOC:

            break;

        case MEMFREE:
            break;

        case MEMREAD:
            break;

        case MEMWRITE:
            break;

        default:
            break;
        }
    }
}

t_list* iniciar_paginas(int cliente, int pid){
    // checkear si puede iniciar
    // si no puede ser iniciado: pthread_exit(0)
    tab_pags* tabla = malloc(sizeof(tab_pags));
    tabla->pid = pid;
    tabla->tabla_pag = list_create();

    add_new_page_table(tabla);
    // enviar_instancia();  ?

    return tabla->tabla_pag;
 }

int desginar_PID(int cliente){
    int pid = recibir_PID(cliente);
    if(pid == PID_EMPTY) return crearID(&ids_memoria);
    return pid;
}

int recibir_PID(int cliente){
    return recibir_tamanio(cliente);
}