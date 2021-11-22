#include "planificacion.h"

void iniciar_cpu(){
   pthread_t *cpu[configuracion.GRADO_MULTIPROGRAMACION];
   for(int i = 0 ; i < configuracion.GRADO_MULTIPROCESAMIENTO ; i++){
      pthread_create(cpu[i],NULL, (void*) procesador ,NULL);
   }

}
void procesador(){// resolver la cuestion de administacion de los semaforos de los procesos 
                    // com va a desalojar al proceso
   int operacion;
   while(!terminar){
      sem__wait(&lista_ordenada_por_algoritmo);
      t_pcb *carpincho = (t_pcb*) list_take(lista_ordenada_por_algoritmo, 0);
      sem_post(&lista_ordenada_por_algoritmo);
      carpincho->tiempo.time_stamp_fin = temporal_get_string_time("%H:%M:%S:%MS");// al comenzar a ejecutar corta el eltiempo de espera
      carpincho->tiempo.tiempo_de_espera = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);
      //aca el procesador lo ejecuta por lo tanto hay qu etomar el tiempo d einicio para depues tener lamdiferencia conel timep de salida
      strcpy(carpincho->tiempo.time_stamp_inicio,carpincho->tiempo.time_stamp_fin); // el momento que comeinza a ejecutar es el mismo que cuando termino la espera
      carpincho->estado = 'E';
      enviar_mensaje("OK", carpincho->fd_cliente);
      sem_wait(&carpincho->semaforo_evento);

      switch(carpincho->proxima_instruccion)
      {
      case IO:
         if(verificar_suspension()){//verifiva que el grado de multiprogramacion no este copado por carpinchos bloqueados
            bloquear_por_mediano_plazo(carpincho);
         }
            bloquear_por_io(carpincho);
         
         carpinchos_bloqueados ++;
         break;

      case SEM_WAIT:
        if(sem_kernel_wait2(carpincho) && verificar_suspension()){
           bloquear_por_mediano_plazo(carpincho);
         }
         break; /* ACA SE ATAJAN LOS ENVENTO SUQ HACEN QUE EL CARPINCHO PASE A BLOQUEADO */

      case MATE_CLOSE:
         carpincho->estado = 'F';
         sem_wait(&mutex_cola_finalizados);
         queue_push(cola_finalizados, (void *)carpincho);
         sem_post(&mutex_cola_finalizados);
         sem_post(&cola_finalizados_con_elementos);
         break;
      }
   }   
}
bool verificar_suspension(){
   if(carpinchos_bloqueados == configuracion.GRADO_MULTIPROGRAMACION -1 && list_is_empty(lista_ordenada_por_algoritmo) && !queue_is_empty(cola_new))
   return true;
   else
   
      return false; 
}
   

void iniciar_planificador_corto_plazo()
{ 
   while(!terminar){
      
      sem_wait(&cola_ready_con_elementos);
      t_pcb *carpincho = queue_pop(cola_ready);
      sem_post(&cola_ready_con_elementos);
      if(configuracion.ALGORITMO_PLANIFICACION == "SFJ"){
         estimador(carpincho);
        list_add_sorted(lista_ordenada_por_algoritmo, (void*)carpincho, (void*) comparador_SFJ); //falta, una vez enlistado se le cambia el estado
      }
      else
      {
         estimador_HRRN(carpincho);
         list_add_sorted(lista_ordenada_por_algoritmo, (void*)carpincho, (void*) comparador_HRRN);
      }
   }
}
void estimador(t_pcb *carpincho){// de donde se saca el valor de la rafaga de cpu
   if(carpincho->tiempo.time_stamp_inicio !=NULL){// si es null ES UN PROCESO NUEVO por lo tanto solo hace la etimacion
      carpincho->tiempo.tiempo_ejecutado = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);
   }   
   carpincho->tiempo.estimacion = carpincho->tiempo.estimacion * configuracion.ALPHA + carpincho->tiempo.tiempo_ejecutado * (1 - configuracion.ALPHA);
}

void estimador_HRRN(t_pcb* carpincho){ // esto la complica bastante
   estimador(carpincho);
   carpincho->tiempo.estimacion = (carpincho->tiempo.tiempo_de_espera + carpincho->tiempo.estimacion) / carpincho->tiempo.estimacion;
}
double obtener_tiempo(char* inicio, char* fin){// planteo asi esta funcion para poder usarla con el teimpo de espera tambien
   int aux1, aux2;
	double dif;
	double total = 0.0;
   char ** valores_inicio = string_split(inicio, ":");
	char** valores_fin = string_split(fin, ":");

   for(int i = 0 ; i < 4 ;i ++){

      if (strcmp(valores_inicio[i], valores_fin[i]) != 0)
      {
         aux1 = atoi(valores_inicio[i]);
         aux2 = atoi(valores_fin[i]);
         dif = aux2 - aux1;
         switch (i)
         {
         case 0:
            total += (dif * 3600); //horas
            break;
         case 1:
            total += (dif * 60); //minutos
            break;
         case 2:
            total += dif; //segundos
            break;
         case 3:
            total += (dif * 0.001); //milisegundo
            break;
         } 
      }
	}
	 return total;
}

bool comparador_SFJ(t_pcb* carpincho1, t_pcb* carpincho2){
   if(carpincho1->tiempo.estimacion < carpincho2->tiempo.estimacion){
      return true;
   }
   return false;
}
bool comparador_HRRN(t_pcb* carpincho1, t_pcb* carpincho2){
     if(carpincho1->tiempo.estimacion > carpincho2->tiempo.estimacion){
      return true;
   }
   return false;
}
void bloquear_por_mediano_plazo(t_pcb *carpincho)
{
   //AVISAR A MEMORIA QUE EL CARPINCHO ESTA SUSPENDIDO (SWAMP)

   enviar_mensaje_y_cod_op("suspenderme",  carpincho->fd_memoria, SUSPENCION);
   sem_wait(&mutex_cola_bloqueado_suspendido);
   queue_push(suspendido_bloqueado, carpincho);
   sem_post(&mutex_cola_bloqueado_suspendido);
   sem_post(&cola_suspendido_bloquedo_con_elementos);
  
}
void  planificador_mediano_plazo(){
   t_pcb *carpincho;
   while(!terminar){
   sem_wait(&cola_suspendido_bloquedo_con_elementos);
   sem_wait(&mutex_cola_bloqueado_suspendido);
   carpincho = (t_pcb*) queue_pop(suspendido_bloqueado);
   sem_post(&mutex_cola_bloqueado_suspendido);

   sem_wait(&carpincho->semaforo_fin_evento);

   sem_wait(&mutex_cola_listo_suspendido);
   queue_push(suspendido_listo, carpincho);
   sem_post(&mutex_cola_listo_suspendido);
   sem_post(&cola_suspendido_listo_con_elementos);
}
}

void iniciar_planificador_largo_plazo() 
{
   t_pcb *carpincho = NULL;
   while (1)
   {
      sem_wait(&controlador_multiprogramacion);// este semaforo contador controla la cantidad de procesos que estaran ordenaod en ready el post lo hara el encargado de finalizar los procesos
      sem_wait(&cola_new_con_elementos);

       if(queue_is_empty(suspendido_listo)){ //si el mediano plazo suspendio algo tiene prioridad ese 
      sem_wait(&mutex_cola_new);
      carpincho = queue_pop(cola_new);
      sem_post(&mutex_cola_new);
      inicializar_proceso_carpincho(carpincho); 
      }
      else{
         sem_wait(&mutex_cola_listo_suspendido);
         carpincho = queue_pop(suspendido_listo);
         sem_post(&mutex_cola_listo_suspendido);
      }
      sem_wait(&mutex_cola_ready);
      queue_push(cola_ready, carpincho);
      carpincho->tiempo.time_stamp_inicio = temporal_get_string_time("%H:%M:%S:%MS");
      enviar_mensaje_y_cod_op("sali de suspension", carpincho->fd_memoria, VUELTA_A_READY);
      sem_post(&mutex_cola_ready);
      sem_post(&cola_ready_con_elementos);
   }
}

void inicializar_proceso_carpincho(t_pcb *carpincho)
{
   carpincho->tiempo.estimacion = configuracion.ESTIMACION_INICIAL;
   carpincho->tiempo.tiempo_ejecutado = 0;
   carpincho->tiempo.time_stamp_inicio = NULL;
   carpincho->tiempo.time_stamp_fin = NULL;
   carpincho->estado = 'N';
   sem_init(&carpincho->semaforo_evento, NULL, 0);
   sem_init(&carpincho->semaforo_fin_evento, NULL, 0);
}
void iniciar_gestor_finalizados(){
   while(!terminar){
      sem_wait(&cola_finalizados_con_elementos);
      sem_wait(&mutex_cola_finalizados);
      t_pcb *carpincho = queue_pop(cola_finalizados);
      sem_post(&mutex_cola_finalizados);
      elminar_carpincho((void*)carpincho);
      sem_post(&controlador_multiprogramacion);
   }
}
void eliminar_carpincho(void *arg){// revisar que este este borrando lo necesario y no haya free's de mas
   t_pcb * carpincho = (t_pcb*) arg;
    free(carpincho->tiempo.time_stamp_fin);
    free(carpincho->tiempo.time_stamp_inicio);
    free(carpincho->io_solicitada);
    free(carpincho->semaforo_a_modificar);
    sem_destroy(carpincho->semaforo_evento);
    sem_destroy(carpincho->semaforo_fin_evento);
    free(carpincho);
}

void ejecutando_a_bloqueado( t_pcb *carpincho, t_queue *cola, sem_t * mutex){
   //¿Está bien este cálculo de tiempos o falta algo?
   carpincho->estado='B';
   carpincho->tiempo.time_stamp_fin = temporal_get_string_time("%H:%M:%S:%MS");
   carpincho->tiempo.tiempo_ejecutado = obtener_tiempo(carpincho->tiempo.time_stamp_inicio, carpincho->tiempo.time_stamp_fin);

  //sem_wait(&mutex_lista_ejecutando);
   //sacar carpincho de la lista
  // sem_post(&mutex_lista_ejecutando); AL CARPINCHO LO SACA DE LA LISTA EL PROCESADOR, NO HACE FALTA
   
   sem_wait(&mutex);
   queue_push(cola, (void*)carpincho); 
   sem_post(&mutex);

  
}

void bloqueado_a_listo( t_queue *cola, sem_t * mutex){
   sem_wait(&mutex);
   t_pcb *carpincho = queue_pop(cola);
   sem_post(&mutex); 
   carpincho->estado = 'R';
   sem_wait(&mutex_cola_ready);
   queue_push(cola_ready, (void*)carpincho);
   sem_post(&mutex_cola_ready);
   //sem_post(&carpincho->semaforo_fin_evento);
   sem_post(&cola_ready_con_elementos); 
   carpincho->tiempo.time_stamp_inicio=temporal_get_string_time("%H:%M:%S:%MS"); //Tomo el tiempo de cuando inicia la espera

}


