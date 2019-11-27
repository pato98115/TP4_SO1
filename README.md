# TP4_SO1
Trabajo practico nro 4 de la materia sistemas operativos 1 de la FCEFyN-UNC
\
*Autores:* Agustinoy Jeremias y Viccini Patricio 

## Descripcion:
>Consiste en una programa escrito en C para GNU/Linux cuya finalidad es la implementacion de las funciones malloc y free* 

---
## **Para el usuario:**

Cada bloque de memoria tiene direcciones virtuales, las cuales aquellas utilizadas son las que existen como direcciones fisicas.
Estos bloques de memoria poseen un break, que consiste en una direccion limite que separa las direcciones de memoria utilizadas de las que no se utilizan. Este break puede desplazarse para asignar al bloque mas memoria.
Estos bloques de memoria a nivel codigo C, se lo plantea como una estructura que consiste de:

##### Tamaño
##### Bloque anterior
##### Bloque siguiente
##### Libre
##### Data

Con el tamaño del bloque hay diversas operaciones de asignacion de memoria, es decir, los bloques pueden estar libres o no, uno de los campos de cada bloque lo indicará.
Que cada bloque indique si está o nó libre, es para la utilidad de división de bloque, es decir, si se requiere un bloque con un cierto tamaño y el bloque libre es de mayor tamaño que el deseado, entonces el bloque libre se divide y se utiliza solo el tamaño de bloque necesario.
Un bloque puede liberarse, de esta manera puede fusionarse con otro bloque libre que se encuentre atras o adelante, de esta manera facilita que cuando se solicite un bloque de un cierto tamaño, se encuentre mas fácilmente.

El desarollo del programa comienza con llamados a la funcion malloc 5 veces para asignar bloques de distintos tamaños.


## **Para el desarrollador

* void* malloc(size_t size): Se encarga de aliear el tamaño recibido como parámetro al multiplo de 4 mas cercano, luego se controla que dicho tamaño sea positivo. Luego se controla la dirección base para verificar si es la primera vez que se llama a la funcion malloc, en casi de serlo, se extiende el heap(cuya utilidad se verá a continuación), se controla que si la extención se pudo realizar con éxito. Si se pudo realizar con éxito se cambia la dirección base a la del bloque creado. En caso de no ser la primera vez que se llama la función malloc, se busca si hay un bloque libre del tamaño requerido o mayor, para ello se recorre la lista desde el principio, en caso de no encontrarse un bloque, se extiende el heap, y en caso de encontrarse se verifica que el tamaño sea el necesario y divido el bloque con el espacio requerido, armando de esta manera otro bloque cuyo tamaño es la diferencia entre el tamaño del bloque libre y el tamaño del bloque requerido. Retorna un puntero que se encuentra delante del bloque creado.

* void liberar_bloque(void *ptr): Se controla que el puntero pasado como parámetro no sea NULL, en caso de no serlo, obtengo el puntero al bloque apuntado por ptr, y lo marco como libre 

  /* Obtengo el puntero al bloque apuntado por "ptr" */
  struct metadata_block* block = obtener_puntero_bloque(ptr);

  /* Marco el bloque como libre */
  block->free = 1;

  /* Veo si hay bloques libres antes o después, intento fusionarlos */
  if(block->prev && block->prev->free) {
    block = fusionar_bloques(block->prev);
  }

  if(block->next) {
    block = fusionar_bloques(block);
  } else {
    /* Se llego al final de la lista */
    if (block->prev) {
      /* Pongo al último bloque como libre */
      block->free = 1;
    }
  }
}
