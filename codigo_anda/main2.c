#include <stdio.h>
#include <unistd.h>

/*! META_SIZE es el tamaño del bloque de meta-data */
#define META_SIZE sizeof(struct metadata_block)

/*! Para alinear punteros, se utilizan tamaños muliplos de 32 bits (4 bytes). Se usan operadores bitwise */
#define align4(x) (((((x)-1)>>2)<<2)+4)

/*! Declaración de la función malloc() */
void* malloc(size_t size);

/*! Declaración de la función free() */
void free(void *ptr);

/*! Declaración de funciones auxiliares */
struct metadata_block* encontrar_bloque_libre(struct metadata_block** last, size_t size);
struct metadata_block* extender_heap(struct metadata_block* last, size_t size);
struct metadata_block* obtener_puntero_bloque(void *ptr);
struct metadata_block* fusionar_bloques (struct metadata_block* block);
void split_block(struct metadata_block* block, size_t size);
void print_list(void);

/*! Estructura del bloque de meta-data */
struct metadata_block {
  size_t size;
  struct metadata_block *prev;
  struct metadata_block *next;
  int free;
  char data[1];
};

/* Inicio de la lista doblemente enlazada */
static void* base = NULL;

/* Metodo main() para testeo de las funciones */
int main(int argc, char *argv[]){

  printf("Test de la implementación de malloc() y free():\n");
  printf("-----------------------------------------------\n\n");

  /* Test */
  printf("Intento alocar los siguientes valores de memoria: 140, 200, 70, 80, 250.\n");

  void *ptr1 = malloc(140);
  void *ptr2 = malloc(200);
  void *ptr3 = malloc(70);
  void *ptr4 = malloc(80);
  void *ptr5 = malloc(250);

  printf("Luego de alocarlos, la lista que representa el heap queda asi:\n");
  print_list();

  printf("\n");
  printf("Libero el bloque de 200 con free(), la tabla queda como la siguiente:\n");
  free(ptr2);
  print_list();

  printf("\n");
  printf("Aloco memoria con malloc(80), se reutiliza el espacio vacio y se divide el bloque. \n");
  void *ptr6 = malloc(80);
  print_list();

  printf("\n");
  printf("Liberamos el bloque que habiamos alocado con malloc(70), y vemos que se va a unir con \n");
  printf("el bloque libre anterior, que surgió de alocar en un espacio libre mas grande. \n");
  free(ptr3);
  print_list();

  printf("\n");
  printf("Si necesitamos más espacio que el que hay libre en algún bloque, se agranda el break. \n");
  void *ptr7 = malloc(256);
  print_list();

  printf("\n");
  printf("Si libero todo lo ocupado.\n");
  free(ptr1);
  free(ptr4);
  free(ptr5);
  free(ptr6);
  free(ptr7);
  print_list();
  printf("\n");
  printf("Alloco de nuevo con malloc(90), se reutiliza lo anteriormente alocado, y se divide.\n");
  void *ptr8 = malloc(90);
  print_list();
    return 0;
}

/* Función malloc() */
void *malloc(size_t size) {
  struct metadata_block *block;

  /* Alineo el tamaño requerido al múltiplo de 4 más cercano */
  size = align4(size);

  /* Controlo que el tamaño sea positivo */
  if (size <= 0) {
    return NULL;
  }


  if (!base) {
    /* Si base es NULL, es la primera vez que se llama a malloc() */
    /* Como es la primera vez, llamo a extender_heap */
    block = extender_heap(NULL, size);
    if (!block) {
      /* Si ocurrio algún error, salgo */
      return NULL;
    }
    /* Coloco al nuevo bloque como la base de la lista */
    base = block;
  } else {
    /* Si no es la primera vez que se llama malloc() */

    /* Veo si hay un bloque libre del tamaño requerido o mayor */
    /* Para esto, recorro la lista desde el principio (First Fit) */
    struct metadata_block *last = base;
    block = encontrar_bloque_libre(&last, size);
    if (!block) {
      /* Si no se encontro nigún bloque, llamo a extender_heap */
      block = extender_heap(last, size);
      if (!block) {
        /* Si hubo un error, salgo de la función */
        return NULL;
      }
    } else {
        /* Si se encontro un bloque libre, me fijo que sea del tamaño necesario */
        if((block->size - size) >= META_SIZE + 4) {
          /* Y divido, tomo el espacio que necesito y armo otro bloque con el resto */
          split_block(block, size);

        }

        /* Pongo el bloque que estaba libre, como ocupado */
        block->free = 0;
    }
  }
  //printf("sale del malloc\n");
  return(block + 1);
}

/* Función free() */
void free(void *ptr) {
  /* Controlo que el puntero pasado no sea NULL */
  if (!ptr) {
    return;
  }

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

/* Función split_block() */
void split_block(struct metadata_block* block, size_t size) {
  /*Guardo el tamaño total del bloque actual*/
  size_t total_size = block->size;

  /* Creo un bloque nuevo, que comience despues que
     termine la primera parte del bloque dividido */
  struct metadata_block* nuevo = (struct metadata_block* ) block->data + size;

  /* Si el bloque tenia un bloque siguiente */
  if(block->next) {
    /* Pongo el siguiente del nuevo, como el siguiente del anterior(lo pongo entre medio) */
    nuevo->next = block->next;
    /* Y el anterior del siguiente del bloque como el nuevo (el bloque de adelante tiene como anterior al bloque nuevo) */
    nuevo->next->prev = nuevo;
  }

  /* El nuevo tamaño del bloque "block" es size */
  block->size = size;
  /* Y el siguiente bloque es el "nuevo" */
  block->next = nuevo;

  /* El tamaño del nuevo es el tamaño total del anterior menos el tamaño
     del bloque, y el tamaño de la memoria utilizada en la primera division */
  nuevo->size = total_size - size - META_SIZE;

  /* Pongo al anterior del nuevo, como el original */
  nuevo->prev = block;
  /* Y lo marco como libre */
  nuevo->free = 1;
}

/* Función fusionar_bloques() */
struct metadata_block* fusionar_bloques (struct metadata_block* block) {
  /* Me fijo si el bloque tiene un siguiente y está libre*/
  if (block->next && block->next ->free ){
    /* Fusiono ambos bloques */
    block->size += META_SIZE + block->next ->size;
    block->next = block->next->next;
    if (block->next) {
      /* Pongo al anterior del siguiente como el actual */
      block->next->prev = block;
    }
  }

  return (block);
}

/* Función encontrar_bloque_libre() */
struct metadata_block *encontrar_bloque_libre(struct metadata_block **last, size_t size) {
  /* Recorro la lista desde la base buscando un bloque que este libre
     y que su tamaño sea mayor o igual al tamaño necesitado  */
  struct metadata_block *current = base;
  while (current && !(current->free && current->size >= size)) {
    *last = current;
    current = current->next;
  }

  /* Retorno un bloque si lo encontré o NULL */
  return current;
}

/* Función extender_heap() */
struct metadata_block *extender_heap(struct metadata_block* last, size_t size) {
  /* Creo un nuevo bloque */
  struct metadata_block *block;

  /* Obtengo el break y coloco el bloque ahí */
  block = sbrk(0);

  /* Intento aumentar el break en el tamaño requerido,
     mas el tamaño del bloque de meta-data */
  void *request = sbrk(size + META_SIZE);

  /* Si no se pudo aumentar el break, retorno NULL, hubo un error */
  if (request == (void*) -1) {
    return NULL;
  }

  /* Si no es la primera vez, actualizo el next del último */
  if (last) {
    last->next = block;
  }

  /* Actualizo el tamaño, el siguiente, el anterior y el estado (free) del nuevo bloque */
  block->size = size;
  block->next = NULL;
  block->prev = last;
  block->free = 0;
  /* Retorno el bloque recientemente alocado */
  return block;
}

/* Función obtener_puntero_bloque() */
struct metadata_block *obtener_puntero_bloque(void *ptr) {
  /* Retorno el puntero al bloque desado */
  return (struct metadata_block*) ptr - 1;
}

/* Función print_list() */
/* Sólo para debugging, imprime la lista doblemente enlazada
   de una forma que sea sencilla de ver e interpretar */
void print_list() {
    struct metadata_block *b = base;

    /* Si la lista esta vacia no hago nada */
    if(b == NULL) return;

    /* Si no esta vacia, la recorro e imprimo los datos en forma de tabla */
    int index = 1;

    printf("+----------+----------+----------+-----------------------+---------------------+-------------------+\n");
    printf("| %8s | %8s | %8s | %20s | %20s | %20s |\n", "Nº Bloque", "Tamaño", "Libre", "Anterior", "Actual", "Siguiente");
    printf("+----------+----------+----------+-----------------------+---------------------+-------------------+\n");

    do {
        printf("| %8d | %8zu | %8s | %20p | %20p | %20p |\n", index, b->size,  (b->free) ? "Si": "No", b->prev, b, b->next);
        index++;
        b = b->next;
    } while(b != NULL);

    printf("+----------+----------+----------+-----------------------+---------------------+--------------------+\n");

}
