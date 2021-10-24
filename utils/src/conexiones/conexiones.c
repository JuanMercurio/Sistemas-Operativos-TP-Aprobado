#include "conexiones.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <pthread.h>

void administrar_clientes(char* IP, char* PUERTO, void (*funcion)(void*)){

  int servidor = iniciar_servidor(IP, PUERTO);

   pthread_attr_t detached;
   pthread_attr_init(&detached);
   pthread_attr_setdetachstate(&detached, PTHREAD_CREATE_DETACHED);

   /* Revisar Condicion para terminar este while */
   while(1){
      pthread_t hilo;
      int *cliente = malloc(sizeof(int));
      *cliente= aceptar_cliente(servidor);
      pthread_create(&hilo, &detached, (void*)funcion,(void*) cliente);
   }

   pthread_attr_destroy(&detached); 
}

int crear_conexion(char *ip, char* puerto){		

	struct addrinfo hints, *server_info, *p;
    int socket_serv;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

    for(p = server_info; p != NULL; p = p->ai_next) {
        if ((socket_serv = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            printf("client: socket\n");
            continue;
        }

        if (connect(socket_serv, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_serv);
            printf("client: connect\n");
            continue;
        }

        break;
    }

	freeaddrinfo(server_info);

   	return socket_serv;
}


int iniciar_servidor(char* ip, char *puerto)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &servinfo);


    int activado = 1;
    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &activado,
                sizeof(int)) == -1) {
            perror("setsockopt");

        };

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }
    
    freeaddrinfo(servinfo);

    listen(socket_servidor, SOMAXCONN);

    printf(" INICIO SERVIDOR\n");
    return socket_servidor;
}


int aceptar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, (void*)&tam_direccion);

	return socket_cliente;
}

int recibir_operacion(int socket_cliente) {
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) != 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

int recibir_tamanio(int socket_cliente) {
    int size;
	recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
    return size;
}

void* recibir_buffer(int size, int socket_cliente) {
	void * buffer = malloc(size);
	recv(socket_cliente, buffer, size, MSG_WAITALL);
	return buffer;
}