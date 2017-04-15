#ifndef RECURSOS_H_INCLUDED
#define RECURSOS_H_INCLUDED

// Dependencias
#include "config.h"
#include "dinamic_stack.h"
#include "allegro.h"
#include "stdio.h"
#include "stdbool.h"

// Definiciones
#define ALTO_PANTALLA  480
#define ANCHO_PANTALLA 640

#define ALTO_DISCO  30
#define ANCHO_DISCO 120
#define SOBRANTE    10

#define NUM_DISCOS  5

#define ALTO_TUBO  30
#define ANCHO_TUBO 10

#define BLANCO 0xFFFFFF

typedef struct stack Torre;
typedef struct node  Disco;

// Mapas de bits
BITMAP * bmp_mapa;

BITMAP * bmp_base;
BITMAP * bmp_torre;
BITMAP * bmp_disco;

BITMAP * bmp_tubo;
BITMAP * bmp_parte_tubo;

BITMAP * bmp_mouse;

// Sonidos

// Variables del juego
extern bool ganado;
extern Torre * torre[3];
extern int valor[NUM_DISCOS];
extern int disco_tomado;
extern int disco_dejado;

void inicializarAllegro();
void cargarImagenes();
void cargarAudios();
void activarMusicaFondo();

#endif // RECURSOS_H_INCLUDED
