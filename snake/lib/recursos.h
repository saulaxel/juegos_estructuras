#ifndef _RECURSOS_H
#define _RECURSOS_H

#include "dinamic_queue.h"
#include "config.h"
#include "allegro.h"

/* Espacio de juego */
#define TAM_BLOQUE 40

#define COLUMNAS 15
#define FILAS    15

typedef struct {
    int x;
    int y;
}Parte;

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

extern bool ganado, perdido;

extern int dir;
extern char ocupado[COLUMNAS][FILAS];
extern struct queue * serpiente;
extern bool hayComida;
extern Parte coordenadaComida;

void inicializarAllegro(void);

void cargarImagenes(void);
void cargarAudios(void);

void activarMusicaFondo(void);

void crearSerpiente(void);
bool insertar(struct node * n);
struct node * nuevoNodo(Parte * p);
struct node * sacar(void);
struct node * peek(bool reiniciar);
Parte * filtrar(struct node * n);

#endif /* _RECURSOS_H */
