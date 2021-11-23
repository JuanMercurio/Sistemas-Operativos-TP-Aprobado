#ifndef _MENSAJES_
#define _MENSAJES_

#include "../conexiones/conexiones.h"

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	int codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef struct
{
   int cod_op;
   t_buffer* buffer;
   int valor; 
}t_paquete_semaforo;

typedef struct
{
   int cod_op;
   int pid;
   int value;
}t_paquete_mem_allocfree;

typedef struct 
{
   int cod_op;
   int pid;
   int origin;
   int dest;
   int size;
}t_paquete_mem_read;


/* 
    @NAME:  handshake
    @DESC:  dado un cliente el server le manda el modulo que es 
 */
void handshake(int cliente, char* modulo);

/* 
    @NAME:  enviar_mensaje
    @DESC:  enviar a un cliente un mensaje
 */
void enviar_mensaje(int cliente, char* mensaje);

/* 
    @NAME:  recibir_operacion
    @DESC:  recibe la operacion que quiere ejecutar el cliente
 */
int recibir_operacion(int socket_cliente);

/* 
    @NAME:  recibir_tamanio
    @DESC:  recibe el tamanio del siguiente buffer
 */
int recibir_tamanio(int socket_cliente);

/* 
    @NAME: recibir_buffer
    @DESC: recibe un buffer dado un tamanio y el socket 
 */
void* recibir_buffer(int size, int socket_cliente);

/* 
    @NAME: recibir_mensaje
    @DESC: recibe un mensaje del socket/server
 */
char* recibir_mensaje(int socket);

/* 
    @NAME: serializar_mensaje
    @DESC: serializa un mensaje listo para enviar
 */
void* serializar_mensaje(char* mensaje);

/* 
    @NAME: serializar_paquete
    @DESC: serializa un paquete
 */
void* serializar_paquete(t_paquete* paquete, int bytes);

void enviar_sem_init(char* sem, int valor, int conexion, int cod_op);
void* serializar_paquete_semaforo(t_paquete_semaforo * paquete, int bytes);
void* recibir_buffer2(int* size, int socket_cliente);
int recibir_valor_sem(int conexion);
t_paquete_semaforo recibir_semaforo(int conexion);

void enviar_mensaje_y_cod_op(char* mensaje, int socket_cliente, int codigo_op);

void enviar_mem_allocfree(int conexion, int cod_op, int pid, int value);
void* serializar_paquete_mem_allocfree(t_paquete_mem_allocfree * paquete, int bytes);
void enviar_mem_read(int conexion, int cod_op, int pid, int origin, int dest, int size);
void* serializar_paquete_mem_read(t_paquete_mem_read * paquete, int bytes);
t_paquete_mem_allocfree recibir_mem_allocfree(int conexion);
t_paquete_mem_read recibir_mem_rw(int conexion);
int recibir_valor_int(int conexion);


#endif