#include "config.h"
#include "recursos.h"
#include "init_allegro.h"

/* Variables de juego */
struct Serpiente serpiente = { NULL, DERECHA, NULL };
struct Comida comida = { false, { -1, -1 } };
struct Juego juego = { 0, 0, 3, false, false };
struct Mapa mapa = { COLUMNAS * TAM_BLOQUE, FILAS * TAM_BLOQUE , FILAS * COLUMNAS };

struct Escenario escenario[NUM_ESCENARIOS];

const int8_t ACTUALIZACIONES[10] = { 30, 22, 16, 12, 10, 8, 6, 5, 4, 3 };
/* Funciones de utileria */

void inicializarAllegro(void (*funcion_cierre)()) {
    init_allegro("Snake game", mapa.ancho, mapa.alto, 70, 70);

    LOCK_FUNCTION(funcion_cierre);
    set_close_button_callback(funcion_cierre);
}

void cargarImagenes(void) {
    bmp_mapa = create_bitmap(mapa.ancho, mapa.alto);

    bmp_portada     = load_bitmap("img/portada_juego.bmp", NULL);
    bmp_serp_cabeza = load_bitmap("img/snake_head.bmp", NULL);
    bmp_serp_cuerpo = load_bitmap("img/snake_body.bmp", NULL);
    bmp_serp_cola   = load_bitmap("img/snake_tail.bmp", NULL);
    bmp_serp_giro   = load_bitmap("img/snake_turn.bmp", NULL);
    bmp_comida      = load_bitmap("img/manzana.bmp", NULL);
    bmp_muro        = load_bitmap("img/muro.bmp", NULL);
    bmp_explosion   = load_bitmap("img/explosion.bmp", NULL);
}

void destruirImagenes(void) {
    destroy_bitmap(bmp_mapa);

    destroy_bitmap(bmp_portada);
    destroy_bitmap(bmp_serp_cabeza);
    destroy_bitmap(bmp_serp_cuerpo);
    destroy_bitmap(bmp_serp_cola);
    destroy_bitmap(bmp_serp_giro);
    destroy_bitmap(bmp_comida);
    destroy_bitmap(bmp_muro);
    destroy_bitmap(bmp_explosion);
}

void cargarAudios(void) {
    smp_intro      = load_wav("sound/intro.wav");
    smp_muerte     = load_wav("sound/muerte.wav");
    smp_mordida    = load_wav("sound/croc_chomp_x.wav");
    smp_movimiento = load_wav("sound/rattlesnake3.wav");
}

void destruirAudios(void) {
    destroy_sample(smp_intro);
    destroy_sample(smp_muerte);
    destroy_sample(smp_mordida);
    destroy_sample(smp_movimiento);
}

void cargarEscenarios(void) {
    char buffer[COLUMNAS + 1];
    char nom_archivo[20];
    FILE *ap_archivo;

    MESSAGE("Preparando la carga de los escenarios\n");

    for (int i = 0 ; i < NUM_ESCENARIOS; i++) {
        sprintf(nom_archivo, "maps/map%d.dat", i);
        ap_archivo = fopen(nom_archivo, "r");
        escenario[i].lugares_disponibles = FILAS * COLUMNAS;

        for (int j = 0; j < FILAS; j++) {
            fscanf(ap_archivo, "%[^\n]\n", buffer);

            MESSAGE("j: %d %s\n", j, buffer);

            for (int k = 0; k < COLUMNAS; k++) {
                if (buffer[k] == 'X') {
                    escenario[i].mapa[k][j] = buffer[k] == 'X' ? 'X' : '\0';
                    escenario[i].lugares_disponibles--;
                }
            }
        }

        fclose(ap_archivo);
    }
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

    if (!mapa.ocupado[aux->x][aux->y])
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
