#include "io_semaforos.h"
#include "../planificacion/planificacion.h"

sem_kernel *buscar_semaforo2(char *nombre, int *pos)
{
   sem_kernel *comparado;
   bool encontrado = false;
   int i = 0;
   while (i < list_size(lista_sem_kernel) && !encontrado)
   {
      comparado = (sem_kernel *)list_get(lista_sem_kernel, i);
      if (strcmp(comparado->id, nombre) == 0)
      {
         *pos = i;
         encontrado = true;
      }
      i++;
   }
   if (encontrado)
   {
      return comparado;
   }
   else
   {
      return NULL;
   }
}

bool sem_kernel_wait2(t_pcb *carpincho)
{
   sem_wait(&carpincho->bloqueado_en->mutex);
   carpincho->bloqueado_en->val--;
   sem_post(&carpincho->bloqueado_en->mutex);
   if (carpincho->bloqueado_en->val == 0)
   {
      carpincho->bloqueado_en->tomado_por = carpincho->pid;
      return false;
   }
   if (carpincho->bloqueado_en->val < 0)
   {

      bloquear_por_semaforo(carpincho);
      return true;
   }
   log_info(logger, "El carpincho %d realizo un SEM WAIT al semaforo %s", carpincho->pid, carpincho->bloqueado_en->id);

   return false;
}

bool verificar_bloqueo_por_semaforo(t_pcb *carpincho)
{
   int sem_valor_actual = obtener_valor_semaforo(carpincho->bloqueado_en->id);
   if (sem_valor_actual - 1 < 0)
   {
      return true;
   }
   else
   {
      return false;
   }
}
int obtener_valor_semaforo(char *semaforo_a_modificar)
{
   int pos;
   sem_kernel *sem = buscar_semaforo2(semaforo_a_modificar, &pos);
   return sem->val;
}

void bloquear_por_semaforo(t_pcb *carpincho)
{
   sem_wait(&carpincho->bloqueado_en->mutex_cola);
   list_add(carpincho->bloqueado_en->bloqueados, (void *)carpincho);
   sem_post(&carpincho->bloqueado_en->mutex_cola);
   log_info(logger, "El carpincho %d está bloqueado por el semáforo %s", carpincho->pid, carpincho->bloqueado_en->id);
}
int sem_kernel_post(char *nombre)
{
   int pos;
   sem_kernel *sem = buscar_semaforo2(nombre, &pos);
   if (sem != NULL)
   {
      sem_wait(&mutex_lista_sem_kernel);
      sem->val++;
      if (sem->val >= 1)
         sem->tomado_por = -1;
      //printf("---------------------------------------------------tomado por %d\n", sem->tomado_por);
      if (!list_is_empty(sem->bloqueados))
      {
         bloqueado_a_listo(sem->bloqueados, &sem->mutex_cola);
      }

      sem_post(&mutex_lista_sem_kernel);
      log_info(logger, "SEM POST EXITOSO a semaforo %s VALOR DEL SEMAFORO ============= %d", sem->id, sem->val);
      return 0;
   }
   else
   {
      //printf("sem post : no se encontro semaforo\n");
      return -1;
   }
}

int sem_kernel_init(char *nombre, int value)
{
   int *pos = malloc(sizeof(int));
   sem_kernel *comparador = buscar_semaforo2(nombre, pos);
   free(pos);
   if (comparador != NULL)
   {
      log_info(logger, "SEMAFORO YA INICIALIZADO");
     
      return -1;
   }
   else
   {
      sem_kernel *nuevo_sem = malloc(sizeof(sem_kernel));
      nuevo_sem->id = malloc(strlen(nombre) + 1);
      strcpy(nuevo_sem->id, nombre);
      nuevo_sem->max_val = value;
      nuevo_sem->val = value;
      nuevo_sem->bloqueados = list_create();
      nuevo_sem->tomado_por = -1;
      sem_init(&nuevo_sem->mutex_cola, 0, 1);
      sem_init(&nuevo_sem->mutex, 0, 1);
      sem_wait(&mutex_lista_sem_kernel);
      list_add(lista_sem_kernel, (void *)nuevo_sem);
      sem_post(&mutex_lista_sem_kernel);
      //log_info(logger, "SEMAFORO %s inicializado con %d",nuevo_sem->id, value);
     
      return 0;
   }
    
}

int sem_kernel_destroy(char *nombre)
{ // ARREGLAR
   int pos;
   sem_kernel *sem = buscar_semaforo2(nombre, &pos);
   if (sem == NULL)
   {
      log_info(logger, "SEMAFORO YA destruido");
      return -1;
   }
   else
   {
      while (!list_is_empty(sem->bloqueados))
      {
         bloqueado_a_listo(sem->bloqueados, &sem->mutex_cola);
      }
     // printf("aca\n");
      sem_wait(&mutex_lista_sem_kernel);
      sem = list_remove(lista_sem_kernel, pos);
      sem_destroyer(sem);
      sem_post(&mutex_lista_sem_kernel);
      log_info(logger, "SEM DESTROY EXITOSO");
      return 0;
   }
}
void sem_destroyer(void *semaforo)
{
   sem_kernel *a_destruir = (sem_kernel *)semaforo;
   free(a_destruir->id);
   list_destroy(a_destruir->bloqueados);
   sem_destroy(&a_destruir->mutex);
   sem_destroy(&a_destruir->mutex_cola);
   free(a_destruir);
}

void io_destroyer(void *arg)
{
   io_kernel *a_destruir = (io_kernel *)arg;
   if (queue_is_empty(a_destruir->bloqueados))
   {
      queue_destroy(a_destruir->bloqueados);
   }
   else
   {
      queue_destroy_and_destroy_elements(a_destruir->bloqueados, (void *)eliminar_carpincho);
   }
   free(a_destruir->id);
   sem_destroy(&a_destruir->mutex_io);
   sem_destroy(&a_destruir->cola_con_elementos);
   free(a_destruir);
  // printf("IOs destruidas\n");
}

io_kernel *buscar_io(char *nombre)
{
   io_kernel *comparado = NULL;
   bool encontrado = false;
   int i = 0;
   while (i < list_size(lista_io_kernel) && !encontrado)
   {
      comparado = (io_kernel *)list_get(lista_io_kernel, i);

      if (strcmp(comparado->id, nombre) == 0)
      {
         encontrado = true;
      }
      i++;
   }
   if (encontrado)
      return comparado;
   else
   {
      return NULL;
   }
}

void init_dispositivos_io()
{
   int i = 0;
   while (configuracion.DISPOSITIVOS_IO[i] != NULL)
   {
      io_kernel *nueva_io = malloc(sizeof(io_kernel));
      nueva_io->id = (char *)configuracion.DISPOSITIVOS_IO[i];
      nueva_io->retardo = atoi((char *)configuracion.DURACIONES_IO[i]);
      nueva_io->bloqueados = queue_create();

      sem_init(&nueva_io->mutex_io, 0, 1);
      sem_init(&nueva_io->cola_con_elementos, 0, 0);
      sem_wait(&mutex_lista_io_kernel);
      list_add(lista_io_kernel, nueva_io);
      sem_post(&mutex_lista_io_kernel);
      i++;
   }
   log_info(logger, "Colas de dispositivos IO creadas");
   iniciar_hilos_gestores_de_io();
}
void iniciar_hilos_gestores_de_io()
{

   pthread_attr_init(&detached2);
   pthread_attr_setdetachstate(&detached2, PTHREAD_CREATE_DETACHED);

   int i = list_size(lista_io_kernel);
   io_kernel *io;
   while (i > 0)
   {
      pthread_t hilo2;
      io = (io_kernel *)list_get(lista_io_kernel, i - 1);
      if ((pthread_create(&hilo2, &detached2, (void *)gestor_cola_io, (void *)io)) != 0)
      {
         log_info(logger, "No se pudo crear el hilo gestor de la IO %s", io->id);
      }
      else
      {
         log_info(logger, "Se creó el hilo gestor de la IO %s", io->id);
      }

      i--;
   }
}

void gestor_cola_io(void *datos)
{
   io_kernel *io = (io_kernel *)datos;
   t_pcb *carpincho;
   while (!terminar)
   {
      //log_info(logger,"IO %s lista para recibir carpinchos", io->id);
      sem_wait(&io->cola_con_elementos);
      if (!terminar)
      {
         sem_wait(&io->mutex_io);
         carpincho = (t_pcb *)queue_pop(io->bloqueados);
         sem_post(&io->mutex_io);
         sleep(io->retardo * 0.001);
         if (carpincho->estado == 'S')
         {
            sem_post(&carpincho->semaforo_fin_evento);
         }
         else
         {
            carpinchos_bloqueados--;
            iniciar_planificador_corto_plazo(carpincho);
         }
      }
      //log_info(logger,"GESTOR IO: desbloqueando carpincho PID %d", carpincho->pid);
   }
}
io_kernel *bloquear_por_io(t_pcb *carpincho)
{
   io_kernel *io = buscar_io(carpincho->io_solicitada);
   return io;
}
