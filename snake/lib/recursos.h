#ifndef _RECURSOS_H
#define _RECURSOS_H

#include <stdio.h>
//#include <time.h>
#include "dinamic_queue.h"
#include "config.h"
#include "allegro.h"

typedef struct node SerpNodo;
typedef struct queue Serp;

/* Espacio de juego */
#define TAM_BLOQUE 40

#define COLUMNAS 15
#define FILAS    15

typedef struct {
    int x;
    int y;
} Coor;

/* Escenario */
BITMAP * mapa;

/* Partes serpiete */
BITMAP * serp_cabeza;
BITMAP * serp_cuerpo;
BITMAP * serp_cola;
BITMAP * serp_giro;

BITMAP * comida;
BITMAP * muro;

/* Otros */
BITMAP * comida;
BITMAP * muro;
BITMAP * explosion;

/* Estado del juego */

extern bool perdido;

extern int dir;
extern char ocupado[COLUMNAS][FILAS];
extern Serp * serpiente;
extern bool hayComida;
extern Coor coordenadaComida;

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
