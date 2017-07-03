#ifndef _RECURSOS_H
#define _RECURSOS_H

#include <stdio.h>
//#include <time.h>
#include "dinamic_queue.h"
#include "config.h"
#include "allegro.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

/* Espacio de juego */
#define TAM_BLOQUE 40

#define COLUMNAS 15
#define FILAS    15

#define new_dir() new_node(malloc(sizeof(int)));

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
