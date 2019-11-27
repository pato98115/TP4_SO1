#include <stdio.h>
#include <unistd.h>

/*! Estructura del bloque de meta-data */
struct s_block {
    size_t    size;
    struct    s_block *prev;
    struct    s_block *next;
    int       free;
    void      *ptr;
    char      data[1];
};
typedef struct s_block *t_block ;

/* Define the block size since the sizeof will be wrong*/
# define BLOCK_SIZE sizeof(struct s_block) //20 //sizeof(struct s_block) - 4

/*! Para alinear punteros, se utilizan tamaños muliplos de 32 bits (4 bytes). Se usan operadores bitwise */
#define align4(x) (((((x)-1)>>2)<<2)+4)

/*! Declaración de la función malloc() */
void* malloc(size_t size);

/*! Declaración de la función free() */
void free(void *ptr);

/*! Declaración de funciones auxiliares */
t_block get_block (void *p);
int valid_addr (void *p);
void split_block ( t_block b, size_t s);
t_block fusion (t_block b);
t_block find_block(t_block *last , size_t size );
t_block extend_heap ( t_block last , size_t s);

void print_list(void);

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
  printf("Alloco de nuevo con malloc(100), se reutiliza lo anteriormente alocado, y se divide.\n");
  void *ptr8 = malloc(100);
  print_list();

    return 0;
}

/* Función malloc() */
void *malloc(size_t size) {
    t_block bloque,last;
    size_t s;
    s = align4 (size);

    if (base != NULL) {

       // printf("malloc1 \n"); //************************************

        /* First find a block */
        last = base;
        bloque = find_block(&last ,s);
        if (bloque) {

           // printf("malloc2 \n"); //************************************

        /* can we split */
            if ((bloque->size - s) >= ( BLOCK_SIZE + 4)){

                //printf("malloc3 \n"); //************************************

                split_block (bloque,s);
            }
            bloque->free =0;
        } 
        else {

            //printf("malloc4 \n"); //************************************

            /* No fitting block , extend the heap */
            bloque = extend_heap (last ,s);
            if (!bloque){

                //printf("malloc5 \n"); //************************************

                return (NULL);
            }
        }
    } 
    else {

       // printf("malloc6 \n"); //************************************

        /* first time */
        bloque = extend_heap (NULL ,s);

        if (!bloque){
            return (NULL);
        }
        base = bloque;
        //printf("holaaaaa\n");
    }
    return (bloque->data);
}


/* Función free() */
void free(void *p){

    //printf("\n %10p \n\n", p);//************************!!!!!!

    t_block b;
    if (valid_addr(p)){

        //printf("aca1 \n");//************************!!!!!!

        b = get_block(p);
        b->free = 1;
        /* fusion with previous if possible */
        if(b->prev && b->prev->free){

            //printf("aca2 \n");//************************!!!!!!

            b = fusion (b->prev );
        }
        /* then fusion with next */
        if (b->next){
        fusion (b);
        }
        else{
            /* free the end of the heap */

            //printf("aca3 \n");//************************!!!!!!

            if (b->prev){
                b->prev->next = NULL;

               // printf("aca4 \n");//************************!!!!!!

            
            }
            else{
                /* No more block !*/

                //printf("aca5 \n");//************************!!!!!!

            
                base = NULL;
            }
            brk(b);
        }
    }
    else{

        //printf("aca6 \n");//************************!!!!!!

    }
}
/* Get the block from and addr */
t_block get_block (void *p) {
    char *tmp;
    tmp = p;
    tmp = tmp - BLOCK_SIZE;
    //t_block t = (t_block) tmp;
    return (t_block)tmp;
    }

/* Valid addr for free*/
int valid_addr (void *p){
    if (base){

            //printf("aca7 \n");//************************!!!!!!

            
        if ( p>base && p< sbrk(0)){


            //printf("aca8 \n");//************************!!!!!!


            return (p == ( get_block(p))->ptr );
        }
    }

            //printf("aca9 \n");//************************!!!!!!

            
    return (0);
}

/* Función split_block() */
/* Split block according to size.*/
/* The b block must exist */
void split_block ( t_block b, size_t s){
    t_block new;
    new = ( t_block )(b->data + s);
    new->size = b->size - s - BLOCK_SIZE ;
    new->next = b->next;
    new->prev = b;
    new->free = 1;
    new->ptr = new->data;
    b->size = s;
    b->next = new;
    if (new->next){
        new->next->prev = new;
    }
    return;
}

/* Función fusionar_bloques() */
t_block fusion (t_block b){
    if (b->next && b->next ->free ){
        b->size += BLOCK_SIZE + b->next->size;
        b->next = b->next->next;
        if (b->next){
            b->next ->prev = b;
        }
    }
    return (b);
}

t_block find_block(t_block *last , size_t size ){
    t_block b=base;
    while (b && !(b->free && b->size >= size )) {
        *last = b;
        b = b->next;
    }
    return (b);
}

/* Función extender_heap() */
/* Add a new block at the of heap*/
/* return NULL if things go wrong*/
t_block extend_heap ( t_block last , size_t s){
    
    //printf("aca extender \n");//****%ld   %10p    %10ld ********************!!!!!!
    
    void* sb;
    t_block b;
    b = sbrk(0);
    sb = sbrk(BLOCK_SIZE + s);

    //printf("    \n\n");//************************!!!!!!
    if (sb == (void*)-1){

            //printf("aca10 \n");//************************!!!!!!

            
        return (NULL);
    }
    b->size = s;
    b->next = NULL;
    b->prev = last;
    b->ptr = b->data;
    if (last){
        last->next = b;
    }
    b->free = 0;
    return (b);
}


/* Función print_list() */
/* Sólo para debugging, imprime la lista doblemente enlazada
   de una forma que sea sencilla de ver e interpretar */
void print_list() {
    t_block b = base;

    /* Si la lista esta vacia no hago nada */
    if(b == NULL){
        return;

        printf("aca \n");//************************!!!!!!

    }

    /* Si no esta vacia, la recorro e imprimo los datos en forma de tabla */
    int index = 1;

    printf("+----------+----------+----------+------------+------------+------------+\n");
    printf("| %8s | %8s | %8s | %10s | %10s | %10s |\n", "Nº Bloque", "Tamaño", "Libre", "Anterior", "Actual", "Siguiente");
    printf("+----------+----------+----------+------------+------------+------------+\n");

    do {
        printf("| %8d | %8zu | %8s | %10p | %10p | %10p |\n",index, b->size,  (b->free) ? "Si": "No", b->prev, b, b->next);
        index++;
        b = b->next;
    } while(b != NULL);

    printf("+----------+----------+----------+------------+------------+------------+\n");

}