#include "lib/config.h"
#include "lib/recursos.h"

#include <unistd.h>

#define LAPSO_DORMIR 40
#define SALIR 0

char ocupado[COLUMNAS][FILAS];

void prepararJuego(void);

int color_fondo;

static long contadorTiempo;
static long contadorTtiempo;

void jugar(void);
    // Se divide en :
    void desplegarImagen(void);
    void interaccionUsuario(bool cambio);
    void calculos(void);
    void dormir(int milis);

bool direccion_opuesta(int dir1, int dir2);
bool misma_direccion(int dir1, int dir2);
bool terminoDescanso(void);
void pausa(void);
static inline bool salir(void);

int main(int argc, char ** argv) {

    jugar();

    return 0;
}

void prepararJuego(void) {
    // #4B0D73
    color_fondo = 0x4B << 16 | 0x9D << 8 | 0x73;

    srand(time(NULL));

    for(int i = 0; i < COLUMNAS; i++) {
        for(int j = 0; j < FILAS; j++) {
            ocupado[i][j] = !(i % (COLUMNAS-1) && j % (FILAS-1)) ?
                    'X' : '\0';
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
    cargarImagenes();
    cargarAudios();

    activarMusicaFondo();

}

void jugar(void){
    // Preambulo
    prepararJuego();

    desplegarImagen();

    textout_ex(screen, font, "Snake game: presiona una tecla para comenzar", 100, 100, 0x0, -1);

    readkey();

    // Juego
    contadorTiempo = clock();
    contadorTtiempo = time(NULL);

    while( !perdido && !salir()) {

        desplegarImagen();

        interaccionUsuario(true);

        calculos();

        dormir(2); // Descanso para el cpu
    }

    desplegarImagen();
    dormir(500);

}

void desplegarImagen(void) {
    SerpNodo * aux;
    Coor *actual, *ant, *sig;
    BITMAP * sprite = create_bitmap(40, 40);
    BITMAP * sprite2 = create_bitmap(40, 40);
    int x_lost, y_lost, angle_lost;

    int i, j;
    int rotate;
    clear_to_color(mapa, color_fondo);
    clear_to_color(sprite, color_fondo);

    for(j = 0; j < COLUMNAS; ++j) {
        draw_sprite(mapa, muro, j * TAM_BLOQUE, 0);
        draw_sprite(mapa, muro, j * TAM_BLOQUE, (FILAS - 1) * TAM_BLOQUE);
    }

    for(i = 1; i < FILAS - 1; ++i) {
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
        else if( actual->x == sig->x - 1 )
            rotate = 128;
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
    static int presionado = -1, ultimo = -1;

    if( !lista_direcciones ) lista_direcciones = new_queue();

    if     ( key[KEY_LEFT]  || key[KEY_A] ) presionado = 0;
    else if( key[KEY_RIGHT] || key[KEY_D] ) presionado = 1;
    else if( key[KEY_UP]    || key[KEY_W] ) presionado = 2;
    else if( key[KEY_DOWN]  || key[KEY_S] ) presionado = 3;
    else if( key[KEY_P] ) pausa();

    if( !misma_direccion(presionado, ultimo)
        && !direccion_opuesta(presionado, ultimo) ) {

        int * new_dir = (int *)malloc(sizeof(int));
        *new_dir = ultimo = presionado;
        en_queue(lista_direcciones, new_node((void *)new_dir));
    }

    if( terminoDescanso() && !queue_is_empty(lista_direcciones) ) {
        struct node * aux = de_queue(lista_direcciones);
        dir = *((int *)aux->data);

        free(aux->data); free(aux);
    }
}

void calculos(void) {
    static long tiempo;
    static long ttiempo;

    SerpNodo * movil;
    char lugar;
    Coor * aux = filtrar(serpiente->rear), * aux2;
    Coor nuevaCoord = *aux;

    if( terminoDescanso() ) {
        tiempo = contadorTiempo;
        ttiempo = contadorTtiempo;

        contadorTiempo = clock();
        contadorTtiempo = time(NULL);

        printf("Tiempo: %ld %ld\n", contadorTiempo - tiempo, contadorTtiempo - ttiempo);
    } else {
        return;
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

bool misma_direccion(int dir1, int dir2) {
    return dir1 == dir2;
}

bool direccion_opuesta(int dir1, int dir2) {
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
