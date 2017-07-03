#include "config.h"
#include "recursos.h"
#include "init_allegro.h"

#define FILAS       15
#define COLUMNAS    15


void inicializarAllegro(void) {
    init_allegro("Snake game",
            COLUMNAS * TAM_BLOQUE, FILAS * TAM_BLOQUE,
            70, 70);
}

void cargarImagenes(void) {
    mapa = create_bitmap(COLUMNAS * TAM_BLOQUE, FILAS * TAM_BLOQUE);

    serp_cabeza = load_bitmap("img/snake_head.bmp", NULL);
    serp_cuerpo = load_bitmap("img/snake_body.bmp", NULL);
    serp_cola   = load_bitmap("img/snake_tail.bmp", NULL);
    serp_giro   = load_bitmap("img/snake_turn.bmp", NULL);
    comida      = load_bitmap("img/manzana.bmp", NULL);
    muro        = load_bitmap("img/muro.bmp", NULL);
    explosion   = load_bitmap("img/explosion.bmp", NULL);

}

void cargarAudios(void) {
    muerte = load_wav("sound/muerte.wav");
    mordida = load_wav("sound/croc_chomp_x.wav");
    movimiento = load_wav("sound/rattlesnake3.wav");
}

void activarMusicaFondo(void) {

}

void crearSerpiente() {
    Coor * aux;

    serpiente = new_queue();

    aux = (Coor *) malloc(3 * sizeof(Coor));

    aux[0].x = 1; aux[0].y = 1;
    aux[1].x = 2; aux[1].y = 1;
    aux[2].x = 3; aux[2].y = 1;


    insertar(new_node(&aux[0]));
    insertar(new_node(&aux[1]));
    insertar(new_node(&aux[2]));
}

bool insertar(SerpNodo * n) {
    Coor * aux = ((Coor *)n->data);

#ifndef NDEBUG
    printf("Ingreso %d %d\n", aux->x, aux->y);
#endif

    ocupado[aux->x][aux->y] = 'S';
    return en_queue(serpiente, n);
}

SerpNodo * sacar(void) {
    SerpNodo * aux = de_queue(serpiente);

    if( aux ) {
        Coor * p = ((Coor *)aux->data);
        ocupado[p->x][p->y] = '\0';
    }

    return aux;
}

SerpNodo * peek(bool reiniciar) {
    return queue_peek(serpiente, reiniciar);
}

Coor * filtrar(SerpNodo * n) {
    return ((Coor *) n->data);
}

SerpNodo * nuevoNodo(Coor * p) {
    return new_node((void *)p);
}
