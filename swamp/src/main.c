#include "main.h"
#include <mensajes/mensajes.h>

#define VALIDO 0
#define INVALIDO -1


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
    
    asignacionFija = 1;
    lista_carpinchos= list_create();
    lista_marcos= list_create();
    marcos_libres_fija = list_create();
    crearArchivos();
    

    if ( asignacionFija){
        marcosLIbresFija();
        if(quedaEspacioEnArchivoDOS()){
           printf("hay espacio\n");
        }
        //printf("La cantidad del archivo 0 es: %d\n",cantidadBarra0File (configuracion.ARCHIVOS_SWAP_list[0]));
        //printf("La cantidad del archivo 1 es: %d\n",cantidadBarra0File (configuracion.ARCHIVOS_SWAP_list[1]));
      
        crearCarpinchoFijaDOS(1);
        crearCarpinchoFijaDOS(2);
        crearCarpinchoFijaDOS(3);
        crearCarpinchoFijaDOS(4);
        crearCarpinchoFijaDOS(5);
        //mostrarCarpinchos();
        char* basura = string_repeat('n',configuracion.TAMANIO_PAGINA);
        int pudo=remplazoPaginaFija(1,2,basura);
        
        basura = string_repeat('k',configuracion.TAMANIO_PAGINA);
        pudo=remplazoPaginaFija(2,2,basura);
        
        basura = string_repeat('p',configuracion.TAMANIO_PAGINA);
        pudo=remplazoPaginaFija(4,1,basura);
        basura = string_repeat('A',configuracion.TAMANIO_PAGINA);
        pudo=remplazoPaginaFija(4,3,basura);
        //mostrarCarpinchos();
        pudo=remplazoPaginaFija(8,1,basura);
        //mostrarCarpinchos();
        char* pal = buscarPaginaFija(2,1);
        printf("pagina 1:%s\n",pal);
        int error = borrarCarpinchoFija(2);
        pal = buscarPaginaFija(4,1);
        if (strcmp (pal,"")){
            printf("pagina 1:%s\n",pal);
        }
        error = borrarCarpinchoFija(5);
        pal = buscarPaginaFija(4,3);
        printf("pagina 1:%s\n",pal);
        crearCarpinchoFijaDOS(6);
        crearCarpinchoFijaDOS(7);
        crearCarpinchoFijaDOS(8);
        crearCarpinchoFijaDOS(9);
        crearCarpinchoFijaDOS(10);
        crearCarpinchoFijaDOS(11);
        crearCarpinchoFijaDOS(12);
        crearCarpinchoFijaDOS(13);
        crearCarpinchoFijaDOS(14);
        crearCarpinchoFijaDOS(15);
        mostrarCarpinchos();


    }
    else{
       /*char* basura = string_repeat('n',configuracion.TAMANIO_PAGINA);
       int algo;
       algo = agregarPaginaDinamica(6, 6, basura);
       basura = string_repeat('p',configuracion.TAMANIO_PAGINA);
       algo = agregarPaginaDinamica(8, 7, basura);
       basura = string_repeat('9',configuracion.TAMANIO_PAGINA);
       algo = agregarPaginaDinamica(9, 1, basura);
       char* pagina = buscarPagina(9, 1);
       printf ("La pagina encontrada es %s\n", pagina);
       /*basura = string_repeat('J',configuracion.TAMANIO_PAGINA);
       algo = remplazarPagina(6,6, basura);
       basura = string_repeat('Z',configuracion.TAMANIO_PAGINA);
       algo = remplazarPagina(9,1, basura);*/

    }
   

    // CONEXIONES
   /*iniciar_swamp();
   terminar_programa();*/
   /*iniciar_swamp();
   atender_clientes();*/
   return 0;
}

// GENERAL
void crearArchivos(){
    
    int i = 0;
    while(configuracion.ARCHIVOS_SWAP_list[i]){
        // intenta abrirlo apra escritura, si no existe lo crea
        
        if (fopen (configuracion.ARCHIVOS_SWAP_list[i],"r") == -1){
            printf("EL archivo no existe. \n");
        }
        FILE* archivo = fopen (configuracion.ARCHIVOS_SWAP_list[i],"w");

        for ( int j = 0; j< configuracion.TAMANIO_SWAP;j++){
            fputs("\0",archivo);
        }
        fclose(archivo);
        /*int file =  open(configuracion.ARCHIVOS_SWAP_list[i], O_RDWR,S_IRUSR|S_IWUSR, O_APPEND,O_CREAT);
        for ( int j = 0; j< configuracion.TAMANIO_SWAP;j++){
            write(file,'\0',1);
        } */

        // abre el archivo en modo escritura
         int file = open (configuracion.ARCHIVOS_SWAP_list[i] , 2);
        // le trunca (coloca) un espacio fijo. puede que tenga datos y lo borre
        int trun2 = ftruncate(file,configuracion.TAMANIO_SWAP);
        //struct stat statbuf;
        //int size = fstat(file,&statbuf);


        close(file);
        //int tamano = statbuf.st_size;
        
        //printf("El valor de truncate es %d\n",tamano);
         //printf("el valor de i es %d.\n",i);
        i++;
       
    }
     //printf("FIN aapertura archivo y truncate\n");
        
}
int elegirMejorArchivo(){
    // busca el archivo con el que mas espacio tenga. 
    int i = 0;
    int menor = cantidadBarra0File (configuracion.ARCHIVOS_SWAP_list[i]);
    int indiceMenor = 0;
   
    //int menor2 = cantidadCaracteresFile (configuracion.ARCHIVOS_SWAP_list[1]);
    //printf("valor menor %d\n",menor);
    //printf("valor menor2 %d\n",menor2);
    while(configuracion.ARCHIVOS_SWAP_list[i]){
       //printf("el valor de i es %d.\n",i);
        if (i == 0){
           menor = cantidadBarra0File (configuracion.ARCHIVOS_SWAP_list[i]);
        }
        else if (menor < cantidadBarra0File (configuracion.ARCHIVOS_SWAP_list[i])){
            menor = cantidadBarra0File (configuracion.ARCHIVOS_SWAP_list[i]);
            indiceMenor = i;
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
    //printf("el indice menor es %d.\n",indiceMenor);
    return indiceMenor;
}
Carpincho_Swamp* buscarCarpincho(int pidd){
    //printf("BUSCANDO NUMERO CARPINCHO. %d\n",pidd);
    int max = list_size(lista_carpinchos);
    Carpincho_Swamp* car = malloc(sizeof(Carpincho_Swamp));
    //printf("Cantidad de carpinchos en lista es %d\n",max);
    for (int i = 0;i<max;i++){
    
        car = (Carpincho_Swamp*) list_get(lista_carpinchos,i);
        /*printf("ITERACION %d\n",i);
        printf("EL pid es %d \n",car->pid);
        printf("EL num archivo es %d \n",car->numeroArchivo);*/
        if(car->pid == pidd){
            return car;
        }
    }
    car = malloc(sizeof(Carpincho_Swamp));
    car->pid = -1;
    return car;
}
int cantidadCaracteresFile (char* path){
    FILE* archivo = fopen (path , "r");
    
    int i = 0;
    while(configuracion.ARCHIVOS_SWAP_list[i]){
        // intenta abrirlo apra escritura, si no existe lo crea
        
        if (fopen (configuracion.ARCHIVOS_SWAP_list[i],"r") == -1){
            printf("EL archivo no existe. \n");
        }
        FILE* archivo = fopen (configuracion.ARCHIVOS_SWAP_list[i],"w");

        for ( int j = 0; j< configuracion.TAMANIO_SWAP;j++){
            fputs("\0",archivo);
        }
        fclose(archivo);    
    char bufer[configuracion.TAMANIO_SWAP];
    char* ptr = fgets(bufer, configuracion.TAMANIO_SWAP, archivo);
    int cant = string_length(ptr);
    //printf("El valor del archivo es %s\n",bufer);
    //printf("La cantidad es %d\n",cant);
    fclose(archivo);
    return cant;
    }
}
int cantidadBarra0File (char* path){
    int  file =  open(path, O_RDWR, S_IRUSR|S_IWUSR);
    char * file_in_memory = mmap(NULL,configuracion.TAMANIO_SWAP,PROT_READ |PROT_WRITE ,MAP_SHARED, file,0);
    int k = 0;
    for(int i = 0; i< configuracion.TAMANIO_SWAP;i++ ){
        //printf("entro\n");
        //printf("%c",file_in_memory[i]);
        if (file_in_memory[i]== '\0'){
            //printf("entroHOLI\n");
            k++;
        }
    }
    //printf("El valor del archivo es %s\n",bufer);
    //printf("La cantidad es %d\n",cant);
    close(file);
    return k;
}
bool quedaEspacioEnArchivo(){
    int i = 0;
    while(configuracion.ARCHIVOS_SWAP_list[i]){
        if( cantidadCaracteresFile (configuracion.ARCHIVOS_SWAP_list[i])< configuracion.TAMANIO_SWAP-1){
            return true;
        }
        i++;
    }
    return false;
}
bool quedaEspacioEnArchivoDOS(){
    int i = 0;

    while(configuracion.ARCHIVOS_SWAP_list[i]){
        // PUEDE SALTAR ERROR O QUE SEA TAMAÑO PAGINA -1
        if( cantidadBarra0File (configuracion.ARCHIVOS_SWAP_list[i]) > configuracion.TAMANIO_PAGINA){
            return true;
        }
        i++;
    }
    return false;
}

//DINAMICA
int agregarPaginaDinamica(int pid, int pagina, char* contenido){
    Carpincho_Swamp* car = buscarCarpincho(pid);
    int num_archivo;
    if (car->pid == -1){
        num_archivo = elegirMejorArchivo();
        Carpincho_Swamp* carpincho;
        carpincho = malloc(sizeof(Carpincho_Swamp));
        carpincho->pid = pid;
        carpincho->numeroArchivo = num_archivo;
        car = carpincho;
        list_add(lista_carpinchos,car);
        printf("paso1\n");
    }
    else{
        num_archivo = car->numeroArchivo;
        printf("paso2\n");
    }
    
    printf("paso3\n");
    Marcos_x_pagina* marco = malloc(sizeof(Marcos_x_pagina));
    marco->pid = car->pid;
    marco->pagina = pagina;
    marco->numero_archivo = num_archivo;
    int cant_caracteres = cantidadCaracteresFile (configuracion.ARCHIVOS_SWAP_list[num_archivo]);
    int file =  open(configuracion.ARCHIVOS_SWAP_list[num_archivo], O_RDWR,S_IRUSR|S_IWUSR, O_APPEND);
    if (cant_caracteres == 2){
        marco->marco = 0;
    }
    else{
        lseek (file , cantidadCaracteresFile(configuracion.ARCHIVOS_SWAP_list[num_archivo]), 0);
        marco->marco = cant_caracteres/configuracion.TAMANIO_PAGINA;
    }
    
    list_add(lista_marcos, marco);
    
    write(file,contenido,configuracion.TAMANIO_PAGINA);
    close(file);
    return 1;
}
char* buscarPagina(int pid, int pagina){
    Marcos_x_pagina* marco =  buscarMarco( pid, pagina);
    if(marco->pid != -1){

        int  file =  open(configuracion.ARCHIVOS_SWAP_list[marco->numero_archivo], O_RDWR, S_IRUSR|S_IWUSR);
        int base = 0;
        if (marco->marco != 0){
            base = (marco->marco)*configuracion.MARCOS_MAXIMOS*configuracion.TAMANIO_PAGINA;
        }

        char * file_in_memory = mmap(NULL,cantidadCaracteresFile(configuracion.ARCHIVOS_SWAP_list[marco->numero_archivo]),PROT_READ |PROT_WRITE ,MAP_SHARED, file,0);
        // supone que las paginas cuentan desde 0. por eso desde la pagina.. hasta pagina +1..
        char* pagina_devolver = string_substring(file_in_memory, base ,configuracion.TAMANIO_PAGINA);
        close(file);
        return pagina_devolver;
    }
    printf("Pagina no encontrada\n");
    return "";
}
Marcos_x_pagina* buscarMarco(int pid, int pagina){
    int max = list_size(lista_marcos);
    Marcos_x_pagina* marco = malloc(sizeof(Marcos_x_pagina));
    //printf("Cantidad de carpinchos en lista es %d\n",max);
    for (int i = 0;i<max;i++){
    
        marco = (Marcos_x_pagina*) list_get(lista_marcos,i);
        if(marco->pid == pid && marco->pagina == pagina){
            return marco;
        }
    }
    marco->pid = -1;
    return marco;

}
int remplazarPagina(int pid, int pagina, char* contenido){
    Marcos_x_pagina* marco =  buscarMarco( pid, pagina);
    if(marco->pid != -1){
        int  file =  open(configuracion.ARCHIVOS_SWAP_list[marco->numero_archivo], O_RDWR, S_IRUSR|S_IWUSR);
        int base = 0;
        if (marco->marco != 0){
            base = (marco->marco)*configuracion.MARCOS_MAXIMOS*configuracion.TAMANIO_PAGINA;
        }

        char * file_in_memory = mmap(NULL,cantidadCaracteresFile(configuracion.ARCHIVOS_SWAP_list[marco->numero_archivo]),PROT_READ |PROT_WRITE ,MAP_SHARED, file,0);
        for (int i = base; i<configuracion.TAMANIO_PAGINA+base;i++){
            
            file_in_memory[i] = contenido[i];
        }
        munmap(file_in_memory,cantidadCaracteresFile(configuracion.ARCHIVOS_SWAP_list[marco->numero_archivo]));
        
        close(file);
       
        return 1;
    }
    else{
        printf("NO SE ENCONTRO LA PAGINA ni el carpincho\n");
        return -1;
    }
    
}

// ASIGNACION FIJA
void crearCarpinchoFijaDOS(int pidd){
    printf("Creando carpincho %d\n",pidd);
    Carpincho_Swamp* car = buscarCarpincho(pidd);
    
    if (elegirMejorArchivoDOS (pidd) != -1 && car->pid == -1 ){
        //printf("ENTRO %d\n",pidd);
        int mejorArchivo = elegirMejorArchivoDOS();
        int file =  open(configuracion.ARCHIVOS_SWAP_list[mejorArchivo], O_RDWR,S_IRUSR|S_IWUSR, O_APPEND);
        Carpincho_Swamp* carpincho;
        carpincho = malloc(sizeof(Carpincho_Swamp));
        carpincho->pid = pidd;
        carpincho->numeroArchivo = mejorArchivo;
        carpincho->base = elegirBaseCarpincho(mejorArchivo);
        list_add(lista_carpinchos,carpincho);
        close(file);
    }
    else{
        printf("CARPINCHO YA EXISTENTE o no hay paginas\n");
    }
}
int elegirMejorArchivoDOS(){
    int i = 0;
    int mayorBloque = 0;
    int contador;
    int archivo_mayor = -1;
    while(configuracion.ARCHIVOS_SWAP_list[i]){
        contador = 0;
        marcos_libre* mar = malloc (sizeof(marcos_libre));
        int max = list_size(marcos_libres_fija);
        if (max != 0){
            for (int j = 0;j<max;j++){
                mar = ( marcos_libre*) list_get(marcos_libres_fija,j);
                
                if(mar->numero_archivo == i){
                    //printf("%d-",mar->numero_archivo);
                    contador++;
                }
            }
            printf("\n");
            if (mayorBloque == 0){
                mayorBloque = contador;
                archivo_mayor = i;
            }
            else if(contador>mayorBloque){
                archivo_mayor = i;
            }
        }
        
        i++;
    }
    return archivo_mayor;
}
void marcosLIbresFija(){
    int i = 0;
     while(configuracion.ARCHIVOS_SWAP_list[i]){    
        for(int j = 0; j<configuracion.TAMANIO_SWAP-configuracion.TAMANIO_PAGINA*configuracion.MARCOS_MAXIMOS; j = j+ configuracion.TAMANIO_PAGINA*configuracion.MARCOS_MAXIMOS)
            {
                marcos_libre* mar = malloc (sizeof(marcos_libre));
                //printf("%d - ",j);
                mar->numero_archivo = i;
                mar->base = j;
                list_add(marcos_libres_fija,mar);
                

            }
             //printf("\n");
         i++;
     }
     //printf("Marcos libres %d\n",list_size(marcos_libres_fija));

}
int elegirBaseCarpincho(int num_archivo){
    //printf("ENtro elegir base\n");
    marcos_libre* mar = malloc (sizeof(marcos_libre));
    int max = list_size(marcos_libres_fija);
    //printf("max:%d\n",max);
    for(int j = 0; j< max; j++)
    {
        mar = ( marcos_libre*) list_get(marcos_libres_fija,j);
        //printf("num archivo: %d, num base: %d\n",mar->numero_archivo,mar->base);
        if (mar->numero_archivo == num_archivo){
            //printf("base%d\n",mar->base);
            int devolver = mar->base;
            list_remove(marcos_libres_fija, j);
            return devolver;
        }
        
    }
}
void mostrarCarpinchos(){
    printf("Mostrando carpincho\n");
    int max = list_size(lista_carpinchos);
    Carpincho_Swamp* car;
    printf("CANT CARPINCHO %d\n",max);
    for (int i = 0;i<max;i++){
    
        car = (Carpincho_Swamp*) list_get(lista_carpinchos,i);
        printf("ITERACION %d\n",i);
        printf("EL pid es %d \n",car->pid);
        printf("EL num archivo es %d \n",car->numeroArchivo);
        printf("La BASE es %d \n",car->base);

    }
}

int remplazoPaginaFija(int pid, int pagina, char*contenido){

    Carpincho_Swamp* car = buscarCarpincho(pid);
    if(  car->pid != -1 ){
        int  file =  open(configuracion.ARCHIVOS_SWAP_list[car->numeroArchivo], O_RDWR, S_IRUSR|S_IWUSR);
        //printf(" base %d carp: %d.\n",base,pid);
        char * file_in_memory = mmap(NULL,configuracion.TAMANIO_SWAP,PROT_READ |PROT_WRITE ,MAP_SHARED, file,0);
       // supone que las paginas cuentan desde 0. por eso desde la pagina.. hasta pagina +1..
        int j = 0;
       
        for (int i = car->base+(pagina)*configuracion.TAMANIO_PAGINA; i < car->base+(pagina+1)*configuracion.TAMANIO_PAGINA ;i++){
            //printf("r-%d\n",i);
            file_in_memory[i]=contenido[j];
            j++;
        }
        //printf("\n");
        //munmap(file_in_memory,cantidadCaracteresFile(configuracion.ARCHIVOS_SWAP_list[car->numeroArchivo]));
        
        close(file);
       
        return 1;
    }
    else{
        printf("NO SE ENCONTRO LA PAGINA\n");
        return -1;
    }
    
}

char* buscarPaginaFija(int pid, int pagina){
    Carpincho_Swamp* car = buscarCarpincho(pid);
    if(  car->pid != -1 ){
        int  file =  open(configuracion.ARCHIVOS_SWAP_list[car->numeroArchivo], O_RDWR, S_IRUSR|S_IWUSR);
        
         //printf(" base %d carp: %d.\n",base,pid);
        char * file_in_memory = mmap(NULL,configuracion.TAMANIO_SWAP,PROT_READ |PROT_WRITE ,MAP_SHARED, file,0);
       // supone que las paginas cuentan desde 0. por eso desde la pagina.. hasta pagina +1..
       
        char* pagina_devolver = string_substring(file_in_memory, car->base + pagina*configuracion.TAMANIO_PAGINA,configuracion.TAMANIO_PAGINA);
        
       /* for (int i = base + pagina*configuracion.TAMANIO_PAGINA; i < tope ;i++){
            //printf("%d\n",j);
          string_append 
          
        }*/
        //printf("\n");
        //munmap(file_in_memory,cantidadCaracteresFile(configuracion.ARCHIVOS_SWAP_list[car->numeroArchivo]));
        
        close(file);
        //printf("Devuelve: %s.\n",pagina_devolver);
        //printf("CANT CARACTERES: %d.\n",string_length(pagina_devolver));
        return pagina_devolver;
    }
    printf("pagina no encontrada\n");
    return "";
    
}
int borrarCarpinchoFija(int pid){
    Carpincho_Swamp* car = buscarCarpincho(pid);
    if(  car->pid != -1 ){
        int  file =  open(configuracion.ARCHIVOS_SWAP_list[car->numeroArchivo], O_RDWR, S_IRUSR|S_IWUSR);
      
        char * file_in_memory = mmap(NULL,configuracion.TAMANIO_SWAP,PROT_READ |PROT_WRITE ,MAP_SHARED, file,0);
        for (int i = car->base; i < car->base+configuracion.MARCOS_MAXIMOS*configuracion.TAMANIO_PAGINA;i++){
            //printf("r-%d\n",i);
            file_in_memory[i]='\0';
           
        }
        marcos_libre* mar = malloc (sizeof(marcos_libre));
        mar->numero_archivo = car->numeroArchivo;
        mar->base = car->base;
        list_add(marcos_libres_fija,mar);
        printf("entro1\n");
        sacarCarpinchoDeLista(car->pid);
        free(car);

        close(file);
    }
    return -1;
}
void sacarCarpinchoDeLista( int pidd){
    int max = list_size(lista_carpinchos);
    Carpincho_Swamp* car = malloc(sizeof(Carpincho_Swamp));
    //printf("Cantidad de carpinchos en lista es %d\n",max);
  
    for (int i = 0;i<max;i++){
        
        car = (Carpincho_Swamp*) list_get(lista_carpinchos,i);
        if(car->pid == pidd){
            list_remove(lista_carpinchos, i);
            max = max-1;
            i = max;
        }
    }
  
}

// CONEXIONES
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
      //printf("Se conecto alguien en el socket: %d\n", cliente);
       int operacion = recibir_operacion(cliente);
      int estado;
      switch (operacion)
      {
      case SOLICITUD_INICIO:
      // estado = funcion que determina si puede iniciar un nuevo proceso (0 si puede, -1 si no puede)
         estado = -1;
         resolver_estado(estado, cliente);
         break;


      //faltan las demas operaciones
      default:
         break;
      }
   }
}

int resolver_estado(int estado, int cliente){

   if (estado == INVALIDO)
   {
      enviar_int(cliente, INVALIDO);
      return INVALIDO;
   }

   if (estado == VALIDO)
   {
      enviar_int(cliente, VALIDO);
      return recibir_int(cliente);
   }

}