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
->Tamaño;
->Bloque anterior;
->Bloque siguiente;
->Libre;
->Data;

Con el tamaño del bloque hay diversas operaciones de asignacion de memoria, es decir, los bloques pueden estar libres o no, uno de los campos de cada bloque lo indicará.
Que cada bloque indique si está o nó libre, es para la utilidad de división de bloque, es decir, si se requiere un bloque con un cierto tamaño y el bloque libre es de mayor tamaño que el deseado, entonces el bloque libre se divide y se utiliza solo el tamaño de bloque necesario.
