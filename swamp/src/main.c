#include "main.h"



int main(int argc, char* argv[]) {
   
   //solo corre si corremos el binario asi: binario test
   tests(argc, argv[1]);    

   /* Este modulo funciona sin hilos. Los clientes esperan a ser atendidos en cola  */
   obtener_config();
   
  /* printf("El ip del server va a ser: %s \n", configuracion.IP);   
   printf("El ip del server va a ser: %s \n", configuracion.PUERTO);   
   printf("El ip del server va a ser: %d \n", configuracion.TAMANIO_SWAP);   
   printf("El ip del server va a ser: %d \n", configuracion.TAMANIO_PAGINA);   
   printf("El ip del server va a ser: %d \n", configuracion.RETARDO_SWAP);  
   printf("Los archivos del server van a ser: %s \n", configuracion.ARCHIVOS_SWAP_list[0]);  
   printf("Los archivos del server van a ser: %s \n", configuracion.ARCHIVOS_SWAP_list[1]);
  */
    //printf("La cantidad de elementos %d \n", lenght(configuracion.ARCHIVOS_SWAP_list));   
   
   crearArchivos();
   //int numMejor= elegirMejorArchivo();
    crearCarpincho (5);
    crearCarpincho (6);
    //crearCarpincho (7);
   /*iniciar_swamp();
   atender_clientes();
   terminar_programa();*/

   return 0;
}

void crearArchivos(){
    
    int i = 0;
    while(configuracion.ARCHIVOS_SWAP_list[i]){
        // intenta abrirlo apra escritura, si no existe lo crea
        if (fopen (configuracion.ARCHIVOS_SWAP_list[i],"r") == -1){
                printf("EL archivo no existe. \n");
        }
        FILE* archivo = fopen (configuracion.ARCHIVOS_SWAP_list[i],"a");
        fputs("/0",archivo);
        fclose(archivo);
        // abre el archivo en modo escritura
        int file = open (configuracion.ARCHIVOS_SWAP_list[i] , 2);
        // le trunca (coloca) un espacio fijo. puede que tenga datos y lo borre
        int trun2 = ftruncate(file,configuracion.TAMANIO_SWAP);
        struct stat statbuf;
        int size = fstat(file,&statbuf);
        close(file);
        int tamano = statbuf.st_size;
        
        printf("El valor de truncate es %d\n",tamano);
         printf("el valor de i es %d.\n",i);
        i++;
       
    }
     printf("FIN aapertura archivo y truncate\n");
        
}
int elegirMejorArchivo(){

    int i = 0;
    int mayor = 0;
    int indiceMayor = 0;
    while(configuracion.ARCHIVOS_SWAP_list[i]){
       printf("el valor de i es %d.\n",i);
        if (i == 0){
           mayor = cantidadCaracteresFile (configuracion.ARCHIVOS_SWAP_list[i]);

        }
        else if (mayor > cantidadCaracteresFile (configuracion.ARCHIVOS_SWAP_list[i])){
            mayor = cantidadCaracteresFile (configuracion.ARCHIVOS_SWAP_list[i]);
            indiceMayor = i;
        }
        // FUNCION STAT NO USADA
        /*int file = open (configuracion.ARCHIVOS_SWAP_list[i] , 1);
        struct stat statbuf;
        int size = fstat(file,&statbuf);
        close(file);
        int tamano = statbuf.st_size; 
        printf("El valor de truncate es %d\n",tamano);
        */
        
	    //char** list = string_split(bufer, "\n");
         /* if (ptr == NULL) {
                printf("es null \n");
            }*/
        //printf("El valor del archivo es %s\n",devuelve);
        i++;
    }
    printf("el mayor es %d\n",indiceMayor);
    return indiceMayor;
}
void crearCarpincho (int pid){
    int mejorArchivo = elegirMejorArchivo();
    FILE* archivo = fopen (configuracion.ARCHIVOS_SWAP_list[mejorArchivo] , "w+");
    int cant = sizeof("hoola");
     printf("La cantidad de hoola es %d\n",cant);
    //fwrite( pid, sizeof(int), sizeof(pid), archivo );
    fprintf(archivo, "%s","holaa|\n");
    fclose(archivo);
}
int cantidadCaracteresFile (char* path){
    FILE* archivo = fopen (path , "r");
    
    char bufer[configuracion.TAMANIO_SWAP];
    char* ptr = fgets(bufer, configuracion.TAMANIO_SWAP, archivo);
    int cant = string_length(ptr);
    //printf("El valor del archivo es %s\n",bufer);
    //printf("La cantidad es %d\n",cant);
    
    return cant;
}

// hacer funcion que escriba un carpincho una pagina
// hacer que lea una pagina
// busqueda de pagina por carpincho.


void terminar_programa(){
   config_destroy(config);
}

void iniciar_swamp(){
   server = iniciar_servidor(configuracion.IP, configuracion.PUERTO);
}

void atender_clientes(){ 

   // while feo
   while(1){ 
      int cliente = aceptar_cliente(server);
      printf("Se conecto alguien en el socket: %d\n", cliente);
   }
}
