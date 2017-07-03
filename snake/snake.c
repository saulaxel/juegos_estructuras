#include "lib/config.h"
#include "lib/recursos.h"

#include <unistd.h>

#define LAPSO_DORMIR 40
#define SALIR 0

#define IZQUIERDA 0
#define DERECHA   1
#define ARRIBA    2
#define ABAJO     4

struct coordenada {
    int16_t x;
    int16_t y;
};

struct mapaJuego {
    // Variables para lo lOgica
    int16_t alto;
    int16_t ancho;
    int_fast16_t tamanio_bloque;

    char ** ocupado;

    // Variables para lo grAfico
    BITMAP * imagen;
    BITMAP * img_muro;
    int32_t color_fondo;
};

struct serpiente {
    // El cuerpo estarA formado por una cola de coordenadas
    struct queue cuerpo;

    // La direcciOn es un entero 0 - 3
    int8_t direccion;

    // Imagenes partes serpiete
    BITMAP * img_cabeza;
    BITMAP * img_cuerpo;
    BITMAP * img_cola;
    BITMAP * img_giro;
    BITMAP * img_choque;
};

struct comida {
    // Booleanod para saber si hay comida en el mapa o hay que generarla
    // (a fin de cuentas solo aparece una a la vez)
    bool existe;

    // PosiciOn en que dibujar la comida
    struct coordenada posicion;

    // Imagen comida
    BITMAP * imagen;
};

struct juego {
    struct mapaJuego mapa;
    struct serpiente snake;
    struct comida    comida;

    // Cuenta el nUmero de veces que se ha repetido el ciclo de juego
    // desde su inicio
    uint_fast32_t contador_repeticiones;

    uint32_t puntuacion;

    // El juego sigue en pie o se ha perdido
    bool perdido;
};

void cargarRecursos(struct juego * j);


char ocupado[COLUMNAS][FILAS];


int color_fondo;

static long contadorTiempo;

void jugar(void);
    // Se divide en :
    void desplegarImagen(struct juego * j);
    void interaccionUsuario(struct juego * j);
    void calculos(struct juego * j);

bool direccion_opuesta(int dir1, int dir2);
bool misma_direccion(int dir1, int dir2);
bool direccion_opuesta(int32_t dir1, int32_t dir2);
bool misma_direccion(int32_t dir1, int32_t dir2);
bool terminoDescanso(void);
void pausa(void);
static inline bool salir(void);

int main(int argc, char ** argv) {

    jugar(malloc(sizeof(struct juego)));

    return 0;
}

void cargarRecursos(struct juego * j) {
    // Imagenes
    j->mapa.imagen = create_bitmap(COLUMNAS * TAM_BLOQUE, FILAS * TAM_BLOQUE);

    j->snake.img_cabeza = load_bitmap("img/snake_head.bmp", NULL);
    j->snake.img_cuerpo = load_bitmap("img/snake_body.bmp", NULL);
    j->snake.img_cola   = load_bitmap("img/snake_tail.bmp", NULL);
    j->snake.img_giro   = load_bitmap("img/snake_turn.bmp", NULL);
    j->snake.img_choque = load_bitmap("img/explosion.bmp", NULL);

    j->comida.imagen  = load_bitmap("img/manzana.bmp", NULL);
    j->mapa.img_muro  = load_bitmap("img/muro.bmp", NULL);
    // Audios
}


void prepararJuego(struct juego * j) {
    srand(time(NULL));

    j->mapa.color_fondo = 0x4B0D73;

    for(int x = 0; x < j->mapa.ancho; x++) {
        for(int y = 0; y < j->mapa.alto; y++) {
            j->mapa.ocupado[x][y] = (x % (j->mapa.ancho-1)
                    && y % (j->mapa.alto-1)) ?
                    '\0' : 'X';
#ifndef NDEBUG
            putchar(ocupado[i][j]? ocupado[i][j] : ' ');
#endif
        }
#ifndef NDEBUG
        putchar('\n');
#endif
    }

    crearSerpiente();

    inicializarAllegro();
    cargarRecursos(j);

    activarMusicaFondo();

}

void jugar(struct juego * j){
    // Preambulo
    prepararJuego();
    cargarAudios();

    desplegarImagen(j);

    textout_ex(screen, font, "Snake game: presiona una tecla para comenzar", 100, 100, 0x0, -1);

    readkey();

    // Juego
    while( !j->perdido && !salir) {

        desplegarImagen(j);

        interaccionUsuario(j);

        calculos(j);

        rest(10); // Descanso para el cpu
    }

    desplegarImagen();
    play_sample(muerte, 1000, 500, 1000, 0);
    dormir(500);

}

void desplegarImagen(struct juego * j) {
    struct nodo * aux;
    struct coordenada *actual, *ant, *sig;
    BITMAP * sprite = create_bitmap(40, 40);
    BITMAP * sprite2 = create_bitmap(40, 40);

    int x, y;
    int rotate;
    clear_to_color(j->mapa.imagen, j->mapa.color_fondo);
    clear_to_color(sprite, j->mapa.color_fondo);

    int32_t i, j;
    int32_t rotate;
    clear_to_color(mapa, color_fondo);
    clear_to_color(sprite, color_fondo);

    for(y = 0; y < j->mapa.ancho; ++y) {
        draw_sprite(mapa, muro, y * TAM_BLOQUE, 0);
        draw_sprite(mapa, muro, y * TAM_BLOQUE, (FILAS - 1) * TAM_BLOQUE);
    }

    for(i = 1; i < j->mapa.alto - 1; ++i) {
        draw_sprite(mapa, muro, 0, i * TAM_BLOQUE);
        draw_sprite(mapa, muro, (COLUMNAS - 1) * TAM_BLOQUE, i * TAM_BLOQUE);
    }

    if( hayComida ) {
        draw_sprite(mapa, comida,
                coordenadaComida.x * TAM_BLOQUE,
                coordenadaComida.y * TAM_BLOQUE);
    }

    aux = peek(true);

    if( aux ) {
        actual = filtrar(aux);
        sig = filtrar(aux->next);


        if( actual->x == sig->x + 1 )
            rotate = 0;
        else if( actual->y == sig->y + 1 )
            rotate = 64;
        else
            rotate = 192;

#ifndef NDEBUG
        printf("%d %d\n", sig->x, actual->x);
#endif

        rotate_sprite(sprite, serp_cola, 0, 0, itofix(rotate));

        draw_sprite(mapa, sprite,
                actual->x * TAM_BLOQUE, actual->y * TAM_BLOQUE);
    }

    while( (aux = peek(false)) ) {
        clear_to_color(sprite, color_fondo);
        actual = filtrar(aux);
        ant = filtrar(aux->prev);

        if( aux->next ) {
            sig = filtrar(aux->next);

            if( ant->x == sig->x ) {
                rotate = 64;
                rotate_sprite(sprite, serp_cuerpo, 0, 0, itofix(rotate));
            } else if( ant->y == sig->y ) {
                rotate = 0;
                rotate_sprite(sprite, serp_cuerpo, 0, 0, itofix(rotate));
            } else {
                if( ant->y == actual->y ) {
                    if( ant->x > actual->x && actual->y > sig->y )
                        rotate = 192;
                    else if( ant->x > actual->x && actual->y < sig->y )
                        rotate = 0;
                    else if( actual->y > sig->y )
                        rotate = 128;
                    else
                        rotate = 64;
                } else {
                    if( ant->y > actual->y && actual->x > sig->x )
                        rotate = 64;
                    else if( ant->y > actual->y && actual->x < sig->x )
                        rotate = 0;
                    else if( actual->x > sig->x )
                        rotate = 128;
                    else
                        rotate = 192;
                }
                rotate_sprite(sprite, serp_giro, 0, 0, itofix(rotate));
            }
        } else {
            if( perdido ) {
                Coor * aux = filtrar(serpiente->rear);
                blit(mapa, sprite2, aux->x * TAM_BLOQUE, aux->y * TAM_BLOQUE, 0, 0, TAM_BLOQUE, TAM_BLOQUE);
                x_lost = aux->x * TAM_BLOQUE;
                y_lost = aux->y * TAM_BLOQUE;
            }

            if( ant->x == actual->x - 1 )
                rotate = 128;
            else if( ant->x == actual->x + 1 )
                rotate = 0;
            else if( ant->y  == actual->y - 1 )
                rotate = 192;
            else
                rotate = 64;

            if( perdido ) angle_lost = rotate;
            rotate_sprite(sprite, serp_cabeza, 0, 0, itofix(rotate));
        }
        draw_sprite(mapa, sprite,
                actual->x * TAM_BLOQUE, actual->y * TAM_BLOQUE);
    }

    if( perdido ) {
        draw_sprite(mapa, sprite2, x_lost, y_lost);
        rotate_sprite(sprite2, explosion, 0, 0, itofix(angle_lost));
        draw_sprite(mapa, sprite2, x_lost, y_lost);
    }

    blit(mapa, screen, 0, 0, 0, 0, COLUMNAS * TAM_BLOQUE, FILAS * TAM_BLOQUE);
    draw_sprite(mapa, serp_cuerpo, actual->x, actual->y);

    destroy_bitmap(sprite);
    destroy_bitmap(sprite2);
}

void interaccionUsuario(bool cambio) {
    static struct queue * lista_direcciones;
    static int32_t presionado = 1, ultimo = 1;

    if( !lista_direcciones ) lista_direcciones = new_queue();

    if     ( key[KEY_LEFT]  || key[KEY_A] ) presionado = 0;
    else if( key[KEY_RIGHT] || key[KEY_D] ) presionado = 1;
    else if( key[KEY_UP]    || key[KEY_W] ) presionado = 2;
    else if( key[KEY_DOWN]  || key[KEY_S] ) presionado = 3;
    else if( key[KEY_P] ) pausa();

    if( !misma_direccion(presionado, ultimo)
        && !direccion_opuesta(presionado, ultimo) ) {
        int32_t direction = ultimo = presionado;

        en_queue(lista_direcciones, new_dir(direction));
    }

    if( terminoDescanso() && !queue_is_empty(lista_direcciones) ) {
        struct node * aux = de_queue(lista_direcciones);
        dir = ptr_to_int(aux->data);
    }
}

void calculos(void) {

    SerpNodo * movil;
    char lugar;
    Coor * aux = filtrar(serpiente->rear), * aux2;
    Coor nuevaCoord = *aux;

    if( terminoDescanso() ) {
        contadorTiempo = clock();
    } else {
        return;
    }

    if( rand() % 10 == 0 ) {
        play_sample(movimiento, 1000, 150, 1000, 0);
    }


    if( !hayComida ) {
        do {
            coordenadaComida.x = 1 + rand() % (COLUMNAS - 2);
            coordenadaComida.y = 1 + rand() % (FILAS - 2);
        } while( ocupado[coordenadaComida.x][coordenadaComida.y] );

        ocupado[coordenadaComida.x][coordenadaComida.y] = 'o';
        hayComida = true;
    }

    if( dir == 0 ) {
        nuevaCoord.x--;
    } else if( dir == 1 ) {
        nuevaCoord.x++;
    } else if( dir == 2 ) {
        nuevaCoord.y--;
    } else {
        nuevaCoord.y++;
    }

#ifndef NDEBUG
    printf("%d %dA\n", nuevaCoord.x, nuevaCoord.y);
    printf("%d %dA\n", aux->x, aux->y);
    /*getchar();*/
#endif

    lugar = ocupado[nuevaCoord.x][nuevaCoord.y];

#ifndef NDEBUG
    printf("lugar %c %d\n", lugar, lugar);
#endif

    if( lugar != '\0' && lugar != 'o') {
        aux2 = filtrar(peek(true));
        if( (aux2->x != nuevaCoord.x) || (aux2->y != nuevaCoord.y) ) {
#ifndef NDEBUG
            printf("HE perdido\n");
#endif
            perdido = true;
        }

        movil = sacar();
    } else if( lugar == 'o') {
        ocupado[nuevaCoord.x][nuevaCoord.y] = '\0';
        hayComida = false;
#ifndef NDEBUG
        printf("He comido\n");
#endif
        movil = new_node(malloc(sizeof(Coor)));
        play_sample(mordida, 50, 250, 1000, 0);
    } else {
#ifndef NDEBUG
        printf("He movido\n");
#endif
        movil = sacar();
    }

    aux2 = filtrar(movil);
    aux2->x = nuevaCoord.x;
    aux2->y = nuevaCoord.y;

    insertar(movil);
}

void dormir(int milis) {
    rest(milis);
}

bool misma_direccion(int32_t dir1, int32_t dir2) {
    return dir1 == dir2;
}

bool direccion_opuesta(int32_t dir1, int32_t dir2) {
    int8_t opuesta[4] = { 1, 0, 3, 2 };

    if( dir1 >= 0 && dir1 < 4 ) {
        return opuesta[dir1] == dir2;
    }

    return false;
}

bool terminoDescanso(void) {
    return clock() >= contadorTiempo + LAPSO_DORMIR * MILI;
}

void pausa(void) {
    textout_ex(screen, font, "Se ha pausado el juego, presiona C para continuar", 100, 100, 0x000000, -1);
    while( !key[KEY_C] ) dormir(10);
}

static inline bool salir(void) { return key[KEY_ESC]; }

void liberarMemoria(void) {
    struct node * aux;

    while( (aux = de_queue(serpiente)) ) {
        free(aux);
    }
    free(serpiente);

    destroy_bitmap(mapa);
    destroy_bitmap(serp_cabeza);
    destroy_bitmap(serp_cuerpo);
    destroy_bitmap(serp_cola);
    destroy_bitmap(serp_giro);
    destroy_bitmap(comida);
    destroy_bitmap(muro);
    destroy_bitmap(explosion);
    destroy_bitmap(screen);

    release_screen();
}
