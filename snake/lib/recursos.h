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

/* Espacio de juego */
#define TAM_BLOQUE 40

#define COLUMNAS 16
#define FILAS    15

#define COLOR_FONDO 0x4B9D73

#define NUM_ESCENARIOS 7

typedef enum { IZQUIERDA, DERECHA, ARRIBA, ABAJO } Dir;

typedef struct {
    int16_t x;
    int16_t y;
} Coor;

struct Serpiente {
    struct queue * cuerpo;
    Dir direccion;

    // La lista de las direcciones que tendrA la respuesta a futuro
    struct queue * proximas_direcciones;
};

struct Comida {
    bool existe;
    Coor posicion;
};

struct Juego {
    uint_fast32_t contador;
    uint32_t puntuacion;
    uint32_t velocidad;

    bool perdido;
    bool ganado;
};

struct Mapa {
    uint16_t ancho;
    uint16_t alto;
    uint32_t lugares_disponibles;

    char (*ocupado)[FILAS];
};

struct Escenario {
    uint32_t lugares_disponibles;
    char mapa[COLUMNAS][FILAS];
};

#define new_dir(d) new_node(int_to_ptr(d))

/* Escenario */
BITMAP * bmp_mapa;
BITMAP * bmp_portada;

/* Partes serpiete */
BITMAP * bmp_serp_cabeza;
BITMAP * bmp_serp_cuerpo;
BITMAP * bmp_serp_cola;
BITMAP * bmp_serp_giro;

/* Otras imagenes */
BITMAP * bmp_comida;
BITMAP * bmp_muro;
BITMAP * bmp_explosion;

/* Somidos */
SAMPLE * smp_intro;
SAMPLE * smp_muerte;
SAMPLE * smp_mordida;
SAMPLE * smp_movimiento;

/* Estado del juego */

extern struct Serpiente serpiente;
extern struct Juego juego;
extern struct Comida comida;
extern struct Mapa mapa;
extern struct Escenario escenario[NUM_ESCENARIOS];
const int8_t ACTUALIZACIONES[10];

/* Funciones */
void inicializarAllegro(void (*funcion_cierre)());

void cargarImagenes(void);
void cargarAudios(void);
void cargarEscenarios(void);

void destruirImagenes(void);
void destruirAudios(void);

void activarMusicaFondo(void);

void crearSerpiente(void);
bool insertar(SerpNodo * n);
SerpNodo * nuevoNodo(Coor * p);
SerpNodo * sacar(void);
SerpNodo * peek(bool reiniciar);
Coor * filtrar(SerpNodo * n);

#endif /* _RECURSOS_H */
