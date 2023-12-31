#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pares en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}

void insertMap(HashMap * map, char * key, void * value) {
    if (map == NULL || key == NULL) {
        // Verificación de entradas
        return;
    }

    // Calcular el índice utilizando la función hash
    long index = hash(key, map->capacity);

    // Buscar una casilla disponible para insertar el nuevo par
    while (map->buckets[index] != NULL) {
        // Si la casilla está ocupada, verifica si la clave ya existe
        if (is_equal(map->buckets[index]->key, key)) {
            // La clave ya existe, no se permiten claves repetidas
            return;
        }

        // Avanza al siguiente índice (sondeo lineal)
        index = (index + 1) % map->capacity;
    }

    // Crear un nuevo par con la clave y el valor
    Pair * newPair = createPair(strdup(key), value);

    if (newPair == NULL) {
        // Error de memoria insuficiente.
        return;
    }

    // Insertar el nuevo par en la casilla vacía encontrada
    map->buckets[index] = newPair;

    // Actualizar el tamaño del mapa
    map->size++;

    // Actualizar el índice current
    map->current = index;
}

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)
    if (map == NULL) {
        // Verificación de entrada
        return;
    }

    // Crear un arreglo auxiliar para mantener el antiguo arreglo de buckets
    Pair ** old_buckets = map->buckets;

    // Duplicar la capacidad
    map->capacity *= 2;

    // Asignar un nuevo arreglo con la nueva capacidad
    map->buckets = (Pair **)calloc(map->capacity, sizeof(Pair *));

    if (map->buckets == NULL) {
        // Error de memoria insuficiente.
        return;
    }

    // Reinicializar el tamaño a 0
    map->size = 0;

    // Insertar los elementos del antiguo arreglo en el nuevo mapa
    for (long i = 0; i < map->capacity / 2; i++) {
        if (old_buckets[i] != NULL && old_buckets[i]->key != NULL) {
            // Insertar el par en el nuevo mapa
            insertMap(map, old_buckets[i]->key, old_buckets[i]->value);
        }
    }

    // Liberar la memoria del antiguo arreglo (no es necesario liberar los pares individuales)
    free(old_buckets);
}


HashMap * createMap(long capacity) {
    HashMap * map = (HashMap *)malloc(sizeof(HashMap));
    if (map == NULL) {
        // Verificación de asignación de memoria.
        return NULL;
    }

    map->buckets = (Pair **)malloc(sizeof(Pair *) * capacity);
    if (map->buckets == NULL) {
        // Verificación de asignación de memoria.
        free(map);
        return NULL;
    }

    for (long i = 0; i < capacity; i++) {
        map->buckets[i] = NULL;
    }
  
    map->size = 0;
    map->capacity = capacity;
    map->current = -1;

    return map;
}


void eraseMap(HashMap * map,  char * key) {
    if (map == NULL || key == NULL) {
        // Verificación de entradas
        return;
    }

    // Calcular el índice utilizando la función hash
    long index = hash(key, map->capacity);

    // Buscar el par con la clave en el mapa
    while (1) {
        // Verificar si la casilla actual contiene un par
        if (map->buckets[index] != NULL) {
            // Comprobar si la clave del par actual coincide con la clave buscada
            if (is_equal(map->buckets[index]->key, key)) {
                // Marcar el par como no válido asignando NULL a la clave
                map->buckets[index]->key = NULL;
                // Actualizar el tamaño del mapa
                map->size--;
                break;
            }
        } else {
            // Hemos llegado a una casilla nula, por tanto la clave no está en el mapa
            break;
        }

        // Avanzar al siguiente índice (sondeo lineal)
        index = (index + 1) % map->capacity;
    }

    // No es necesario actualizar el índice current en este caso.
}

Pair * searchMap(HashMap * map,  char * key) {
    if (map == NULL || key == NULL) {
        // Verificación de entradas
        return NULL;
    }

    // Calcular el índice utilizando la función hash
    long index = hash(key, map->capacity);

    // Inicializar el resultado como NULL
    Pair * result = NULL;

    // Buscar el par con la clave en el mapa
    while (1) {
        // Verificar si la casilla actual contiene un par
        if (map->buckets[index] != NULL) {
            // Comprobar si la clave del par actual coincide con la clave buscada
            if (is_equal(map->buckets[index]->key, key)) {
                // Encontramos el par con la clave buscada
                result = map->buckets[index];
                break;
            }
        } else {
            // Hemos llegado a una casilla nula, la clave no está en el mapa
            break;
        }

        // Avanzar al siguiente índice (sondeo lineal)
        index = (index + 1) % map->capacity;
    }

    // Actualizar el índice current a la posición encontrada
    map->current = index;

    return result;
}

Pair * firstMap(HashMap * map) {
    if (map == NULL) {
        // Verificación de entrada
        return NULL;
    }

    // Inicializar el índice current en 0
    map->current = 0;

    // Encontrar el primer par válido y devolverlo
    while (map->current < map->capacity) {
        if (map->buckets[map->current] != NULL && map->buckets[map->current]->key != NULL) {
            return map->buckets[map->current];
        }
        map->current++;
    }

    return NULL; // No se encontraron pares válidos
}

Pair * nextMap(HashMap * map) {
    if (map == NULL) {
        // Verificación de entrada
        return NULL;
    }

    // Avanzar al siguiente índice desde current
    map->current++;

    // Encontrar el siguiente par válido y devolverlo
    while (map->current < map->capacity) {
        if (map->buckets[map->current] != NULL && map->buckets[map->current]->key != NULL) {
            return map->buckets[map->current];
        }
        map->current++;
    }

    return NULL; // No se encontraron más pares válidos
}