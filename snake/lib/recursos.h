#ifndef _RECURSOS_H
#define _RECURSOS_H

#include <stdio.h>
#include <stdlib.h>

#include "dinamic_queue.h"
#include "config.h"
#include "init_allegro.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#if INTPTR_MAX == INT32_MAX
    #define int_to_ptr(i) (void *)(int32_t)(i + 1)
    #define ptr_to_int(i) (int32_t)(i - 1)
#else
    #define int_to_ptr(i) (void *)(int64_t)(i + 1)
    #define ptr_to_int(i) (int32_t)(int64_t)(i - 1)
#endif

typedef struct node SerpNodo;
typedef struct queue Serp;

#define MILI CLOCKS_PER_SEC / 1000

/* Espacio de juego */
#define TAM_BLOQUE 40

#define COLUMNAS 15
#define FILAS    15

#define new_dir() new_node(malloc(sizeof(int)));

typedef struct {
    int x;
    int y;
} Coor;

#define new_dir(d) new_node(int_to_ptr(d))

/* Escenario */
BITMAP * mapa;

/* Partes serpiete */
BITMAP * serp_cabeza;
BITMAP * serp_cuerpo;
BITMAP * serp_cola;
BITMAP * serp_giro;

/* Otras imagenes */
BITMAP * comida;
BITMAP * muro;
BITMAP * explosion;

/* Somidos */
SAMPLE * muerte;
SAMPLE * mordida;
SAMPLE * movimiento;

/* Estado del juego */

void inicializarAllegro(void);

void cargarImagenes(void);
void cargarAudios(void);

void activarMusicaFondo(void);

void crearSerpiente(void);
bool insertar(SerpNodo * n);
SerpNodo * nuevoNodo(Coor * p);
SerpNodo * sacar(void);
SerpNodo * peek(bool reiniciar);
Coor * filtrar(SerpNodo * n);

#endif /* _RECURSOS_H */
