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
    long size; //cantidad de datos/pairs en la tabla
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
        // Manejar casos de error aquí si es necesario.
        return;
    }

    // Calcular el índice utilizando la función hash
    long index = hash(key, map->capacity);

    // Buscar una casilla disponible para insertar el nuevo par
    while (map->buckets[index] != NULL) {
        // Si la casilla está ocupada, verifica si la clave ya existe
        if (is_equal(map->buckets[index]->key, key)) {
            // La clave ya existe, no se permite claves repetidas
            return;
        }

        // Avanza al siguiente índice (sondeo lineal)
        index = (index + 1) % map->capacity;
    }

    // Crear un nuevo par con la clave y el valor
    Pair * newPair = createPair(strdup(key), value);

    if (newPair == NULL) {
        // Manejar el error de memoria insuficiente aquí si es necesario.
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


}

Pair * searchMap(HashMap * map,  char * key) {
    if (map == NULL || key == NULL) {
        // Manejar casos de error aquí si es necesario.
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

    return NULL;
}

Pair * nextMap(HashMap * map) {

    return NULL;
}
