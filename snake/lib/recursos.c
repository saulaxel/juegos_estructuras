#include "config.h"
#include "recursos.h"
#include "init_allegro.h"

/* Variables de juego */
struct Serpiente serpiente = { NULL, DERECHA, NULL };
struct Comida comida = { false, { -1, -1 } };
struct Juego juego = { 0, 0, 4, false };
struct Mapa mapa = { FILAS * TAM_BLOQUE, COLUMNAS * TAM_BLOQUE };

const int8_t ACTUALIZACIONES[10] = { 30, 22, 16, 12, 10, 8, 6, 5, 4, 3 };
/* Funciones de utileria */

void inicializarAllegro(void) {
    init_allegro("Snake game", mapa.ancho, mapa.alto, 70, 70);
}

void cargarImagenes(void) {
    bmp_mapa = create_bitmap(COLUMNAS * TAM_BLOQUE, FILAS * TAM_BLOQUE);

    bmp_serp_cabeza = load_bitmap("img/snake_head.bmp", NULL);
    bmp_serp_cuerpo = load_bitmap("img/snake_body.bmp", NULL);
    bmp_serp_cola   = load_bitmap("img/snake_tail.bmp", NULL);
    bmp_serp_giro   = load_bitmap("img/snake_turn.bmp", NULL);
    bmp_comida      = load_bitmap("img/manzana.bmp", NULL);
    bmp_muro        = load_bitmap("img/muro.bmp", NULL);
    bmp_explosion   = load_bitmap("img/explosion.bmp", NULL);

}

void cargarAudios(void) {
    smp_muerte = load_wav("sound/muerte.wav");
    smp_mordida = load_wav("sound/croc_chomp_x.wav");
    smp_movimiento = load_wav("sound/rattlesnake3.wav");
}

void activarMusicaFondo(void) {

}

void crearSerpiente() {
    Coor * aux;

    serpiente.cuerpo = new_queue();

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

    MESSAGE("Ingreso %d %d\n", aux->x, aux->y);

    mapa.ocupado[aux->x][aux->y] = 'S';
    return en_queue(serpiente.cuerpo, n);
}

SerpNodo * sacar(void) {
    SerpNodo * aux = de_queue(serpiente.cuerpo);

    if( aux ) {
        Coor * p = ((Coor *)aux->data);
        mapa.ocupado[p->x][p->y] = '\0';
    }

    return aux;
}

SerpNodo * peek(bool reiniciar) {
    return queue_peek(serpiente.cuerpo, reiniciar);
}

Coor * filtrar(SerpNodo * n) {
    return ((Coor *) n->data);
}

SerpNodo * nuevoNodo(Coor * p) {
    return new_node((void *)p);
}
