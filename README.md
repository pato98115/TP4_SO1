# TP4_SO1
Trabajo practico nro 4 de la materia **sistemas operativos 1** de la FCEFyN-UNC
\
*Autores:* Agustinoy Jeremias y Viccini Patricio 

## Descripcion:
>Consiste en una programa escrito en C para GNU/Linux cuya finalidad es la implementacion de las funciones *malloc* y *free* que sirven para el manejo de la memoria.

---
## **Para el usuario:**
>Cada bloque de memoria asigado tiene direcciones de *memoria virtual*, las cuales son traducidas por la unidad de manejo de memoria (**MMU** por sus siglas en ingles), a las direcciones de *memoria fisica* o real.\
Estos bloques de memoria poseen un break, que consiste en una direccion limite que separa las direcciones de memoria utilizadas de las que no se utilizan. Este break puede desplazarse para asignar al bloque mas memoria.\
Para esta implementacion se genera una *lista doblemente enlazada* que lleva la informacion en modo de meta data de los bloques, a estos bloques de memoria a nivel codigo C, se lo plantea como una estructura que consiste de:

>##### Tamaño
>##### Bloque anterior
>##### Bloque siguiente
>##### Libre
>##### Data

>Con el tamaño del bloque hay diversas operaciones de asignacion de memoria, es decir, los bloques pueden estar libres o no, uno de los campos de cada bloque lo indicará.\
Que cada bloque indique si está o nó libre, es para la utilidad de división de bloque, es decir, si se requiere un bloque con un cierto tamaño y el bloque libre es de mayor tamaño que el deseado, entonces el bloque libre se divide y se utiliza solo el tamaño de bloque necesario.\
Un bloque puede liberarse, de esta manera puede fusionarse con otro bloque libre que se encuentre atras o adelante, de esta manera facilita que cuando se solicite un bloque de un cierto tamaño, se encuentre mas fácilmente.

### main() :
>Este programa contiene varias llamadas a *malloc* y a *free* a modo de test, mientras los resultados se presentan graficamente por la salida por consola, impriendo el estado de la lista de bloques de memoria asiganda.


## Para el desarrollador

* **void * malloc(size_t size)**: 
>Se encarga de aliear el tamaño recibido como parámetro al multiplo de 4 mas cercano, luego se controla que dicho tamaño sea positivo. Continuando, se controla la dirección base para verificar si es la primera vez que se llama a la funcion malloc, en caso de serlo, se extiende el heap(cuya utilidad se verá a continuación), se controla que si la extención se pudo realizar con éxito. Si se pudo realizar con éxito se cambia la dirección base a la del bloque creado. En caso de no ser la primera vez que se llama la función malloc, se busca si hay un bloque libre del tamaño requerido o mayor, para ello se recorre la lista desde el principio, en caso de no encontrarse un bloque, se extiende el heap, y en caso de encontrarse se verifica que el tamaño sea el necesario y divido el bloque con el espacio requerido, armando de esta manera otro bloque cuyo tamaño es la diferencia entre el tamaño del bloque libre y el tamaño del bloque requerido. Retorna un puntero que se encuentra delante del bloque creado.

* **void liberar_bloque(void *ptr)**: 
>Se controla que el puntero pasado como parámetro no sea NULL, en caso de no serlo, obtengo el puntero al bloque apuntado por ptr, y lo marco como libre. Se verifica si hay bloques libres anteriores o siguientes y se intenta fusionarlos.

* **void dividir_bloque(struct metadata_block * block, size_t size)**: 
>Guarda el tamaño del bloque actual, crea uno nuevo que comience al terminar la primer parte del bloque dividido. Si el bloque tenia un bloque siguiente, pone el siguiente del bloque nuevo, como siguiente del bloque anterior(entre medio) y el anterior del siguiente del bloque como el nuevo. Luego se establece el tamaño del bloque como el pasado como parámetro y el siguiente bloque es el nuevo. El tamaño del bloque nuevo es el tamaño total del bloque anterior menos el tamaño del bloque, y el tamaño de la memoria utilizada en la primera división. 
Se pone el anterior del nuevo como el original y se marca como free al bloque nuevo.

* **struct metadata_block * fusionar_bloques (struct metadata_block * block)**: 
>Se verifica si el bloque tiene un siguiente y esta libre, en caso de estarlo, se fusionan ambos bloques. Retorna el bloque recibido como parámetro, que ya fue fusionado.

* **struct metadata_block * encontrar_bloque_libre(struct metadata_block ** last, size_t size)**: 
>Consiste en recorrer la lista desde la base buscando un bloque que este libre y que su tamaño sea mayor o igual al tamaño necesitado, cuando lo encuentra retorna el bloque encontrado, o NULL en caso de no encontrar ninguno. 

* **struct metadata_block * extender_heap(struct metadata_block * last, size_t size)**: 
>Consiste en crear un nuevo bloque, obtener el break y colocar el bloque en dicho lugar. Luego se intenta aumentar el break en el tamaño requerido mas el tamaño del bloque de meta data, si no se puso aumentar el break, se retorna NULL. Si no es la primera vez, actualizo el siguiente del ultimo bloque.
Actualizo el tamaño, el bloque siguiente, el bloque anterior y el estado (libre) del nuevo bloque. Retorna el bloque recientemente alocado.

* **struct metadata_block *obtener_puntero_bloque(void *ptr)**:
>Consiste en retornar el puntero al bloque deseado, es decir, pasado como parametro.
