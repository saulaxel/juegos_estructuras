#include "lib/config.h"
#include "lib/recursos.h"

void cargarRecursos(void);

void jugar(void);
    // Se divide en :
    void desplegarImagen(void);
    void interaccionUsuario();
    void calculos(void);

bool direccion_opuesta(int dir1, int dir2);
bool misma_direccion(int dir1, int dir2);
bool direccion_opuesta(int32_t dir1, int32_t dir2);
bool misma_direccion(int32_t dir1, int32_t dir2);
static int calcularRotacion(Coor * anterior, Coor * actual, Coor * siguiente);
void pausa(void);
static inline bool salir(void);

int main(int argc, char ** argv) {

    jugar();

    return 0;
}

void cargarRecursos(void) {
    cargarImagenes();
    cargarAudios();
}


void prepararJuego() {
    srand(time(NULL));

    for (int x = 0; x < FILAS; x++) {
        for (int y = 0; y < COLUMNAS; y++) {
            mapa.ocupado[x][y] = (x % (COLUMNAS - 1) && y % (FILAS - 1)) ?
                    '\0' : 'X';
            MESSAGE("%c", mapa.ocupado[x][y]? mapa.ocupado[x][y] : ' ');
        }
        MESSAGE("\n");
    }

    crearSerpiente();
    serpiente.proximas_direcciones = new_queue();

    inicializarAllegro();
    cargarRecursos();

    activarMusicaFondo();

}

void jugar() {
    // Preambulo
    prepararJuego();
    cargarAudios();

    desplegarImagen();

    textout_ex(screen, font, "Snake game: presiona una tecla para comenzar", 100, 100, 0x0, -1);

    readkey();

    // Juego
    while ( !juego.perdido && !salir()) {

        desplegarImagen();

        interaccionUsuario();

        if (!(juego.contador % ACTUALIZACIONES[juego.velocidad])) calculos();

        rest(10); // Descanso para el cpu
        juego.contador++;
    }

    desplegarImagen();
    play_sample(smp_muerte, 1000, 500, 1000, 0);
    rest(500);

}

void desplegarImagen(void) {
    SerpNodo *aux;
    Coor *actual, *ant, *sig;
    BITMAP *sprite = create_bitmap(40, 40);

    int32_t x, y;
    int32_t rotate;
    clear_to_color(bmp_mapa, COLOR_FONDO);
    clear_to_color(sprite, COLOR_FONDO);

    for (y = 0; y < COLUMNAS; ++y) {
        draw_sprite(bmp_mapa, bmp_muro, y * TAM_BLOQUE, 0);
        draw_sprite(bmp_mapa, bmp_muro, y * TAM_BLOQUE, (FILAS - 1) * TAM_BLOQUE);
    }

    for (x = 1; x < FILAS - 1; ++x) {
        draw_sprite(bmp_mapa, bmp_muro, 0, x * TAM_BLOQUE);
        draw_sprite(bmp_mapa, bmp_muro, (COLUMNAS - 1) * TAM_BLOQUE, x * TAM_BLOQUE);
    }

    if ( comida.existe ) {
        draw_sprite(bmp_mapa, bmp_comida,
                comida.posicion.x * TAM_BLOQUE,
                comida.posicion.y * TAM_BLOQUE);
    }

    aux = peek(true);

    if ( aux ) {
        actual = filtrar(aux);
        sig = filtrar(aux->next);

        MESSAGE("%d %d\n", sig->x, actual->x);

        rotate = calcularRotacion(NULL, actual, sig);

        rotate_sprite(sprite, bmp_serp_cola, 0, 0, itofix(rotate));

        draw_sprite(bmp_mapa, sprite,
                actual->x * TAM_BLOQUE, actual->y * TAM_BLOQUE);
    }

    while ( (aux = peek(false)) ) {
        clear_to_color(sprite, COLOR_FONDO);
        actual = filtrar(aux);
        ant = filtrar(aux->prev);

        if ( aux->next ) {
            sig = filtrar(aux->next);

            rotate = calcularRotacion(ant, actual, sig);

            rotate_sprite(sprite, rotate & 256 ? bmp_serp_giro : bmp_serp_cuerpo,
                    0, 0, itofix(rotate));
        } else {

            rotate = calcularRotacion(ant, actual, NULL);

            if (!juego.perdido) {
                rotate_sprite(sprite, bmp_serp_cabeza, 0, 0, itofix(rotate));
            } else {
                Coor * pos_muerte = filtrar(serpiente.cuerpo->rear);
                blit(bmp_mapa, sprite,
                        pos_muerte->x * TAM_BLOQUE, pos_muerte->y * TAM_BLOQUE,
                        0, 0, TAM_BLOQUE, TAM_BLOQUE);
                rotate_sprite(sprite, bmp_explosion, 0, 0, itofix(rotate));
            }
        }

        draw_sprite(bmp_mapa, sprite,
                actual->x * TAM_BLOQUE, actual->y * TAM_BLOQUE);
    }

    blit(bmp_mapa, screen, 0, 0, 0, 0, COLUMNAS * TAM_BLOQUE, FILAS * TAM_BLOQUE);
    draw_sprite(bmp_mapa, bmp_serp_cuerpo, actual->x, actual->y);

    destroy_bitmap(sprite);
}

static int calcularRotacion(Coor * anterior, Coor * actual, Coor * siguiente) {
    int angulo_rotacion; // 0-255

    if (!anterior) {
        if ( actual->x == (siguiente->x + 1) % COLUMNAS )
            angulo_rotacion= 0;
        else if ( actual->x == (siguiente->x - 1) % COLUMNAS )
            angulo_rotacion = 128;
        else if ( actual->y == (siguiente->y + 1) % FILAS )
            angulo_rotacion = 64;
        else
            angulo_rotacion = 192;
    } else if (siguiente) {
        int delta_x[2] = { 0 };
        int delta_y[2] = { 0 };

        if ( anterior->x == (actual->x + 1) % COLUMNAS )
            delta_x[0] = 1;
        else if ( anterior->x == (actual->x - 1 + COLUMNAS) % COLUMNAS )
            delta_x[0] = -1;
        else if ( actual->x == (siguiente->x + 1) % COLUMNAS )
            delta_x[1] = 1;
        else if ( actual->x == (siguiente->x - 1 + COLUMNAS) % COLUMNAS )
            delta_x[1] = -1;

        if ( anterior->y == (actual->y + 1) % FILAS )
            delta_y[0] = 1;
        else if ( anterior->y == (actual->y - 1 + FILAS) % FILAS)
            delta_y[0] = -1;
        else if ( actual->y == (siguiente->y + 1) % FILAS )
            delta_y[1] = 1;
        else if ( actual->y == (siguiente->y - 1 + FILAS) % FILAS)
            delta_y[1] = -1;

        if ( !delta_x[0] && !delta_x[1] ) {
            angulo_rotacion = 64;
        } else if ( !delta_y[0] && !delta_y[1] ) {
            angulo_rotacion = 0;
        } else {
            if ( !delta_y[0] ) {
                if ( delta_x[0] == 1 && delta_y[1] == 1 )
                    angulo_rotacion = 192;
                else if ( delta_x[0] == 1 && delta_y[1] == -1 )
                    angulo_rotacion = 0;
                else if ( delta_y[1] == 1 )
                    angulo_rotacion = 128;
                else
                    angulo_rotacion = 64;

            } else {
                if ( delta_y[0] == 1 && delta_x[1] == 1 )
                    angulo_rotacion = 64;
                else if ( delta_y[0] == 1 && delta_x[1] == -1 )
                    angulo_rotacion = 0;
                else if ( delta_x[1] == 1 )
                    angulo_rotacion = 128;
                else
                    angulo_rotacion = 192;
            }

            angulo_rotacion += 256;
        }
    } else {
        if ( anterior->x == (actual->x - 1 + COLUMNAS) % COLUMNAS )
            angulo_rotacion = 128;
        else if ( anterior->x == (actual->x + 1) % COLUMNAS )
            angulo_rotacion = 0;
        else if ( anterior->y  == (actual->y - 1 + FILAS) % FILAS )
            angulo_rotacion = 192;
        else
            angulo_rotacion = 64;
    }


    return angulo_rotacion;
}

void interaccionUsuario() {
    static int32_t presionado = 1, ultimo = 1;

    if      ( key[KEY_LEFT]  || key[KEY_A] ) presionado = 0;
    else if ( key[KEY_RIGHT] || key[KEY_D] ) presionado = 1;
    else if ( key[KEY_UP]    || key[KEY_W] ) presionado = 2;
    else if ( key[KEY_DOWN]  || key[KEY_S] ) presionado = 3;
    else if ( key[KEY_P] ) pausa();

    if ( !misma_direccion(presionado, ultimo) &&
        !direccion_opuesta(presionado, ultimo) ) {
        int32_t direction = ultimo = presionado;

        en_queue(serpiente.proximas_direcciones, new_dir(direction));
    }

}

void calculos(void) {

    SerpNodo * movil;
    char lugar;
    Coor * aux = filtrar(serpiente.cuerpo->rear), * aux2;
    Coor nuevaCoord = *aux;

    if ( !queue_is_empty(serpiente.proximas_direcciones) ) {
        struct node * aux = de_queue(serpiente.proximas_direcciones);
        serpiente.direccion = ptr_to_int(aux->data);
        free(aux);
    }

    if ( rand() % 50 == 0 ) {
        play_sample(smp_movimiento, 1000, 150, 1000, 0);
    }

    if ( !comida.existe ) {
        do {
            comida.posicion.x = 1 + rand() % (COLUMNAS - 2);
            comida.posicion.y = 1 + rand() % (FILAS - 2);
        } while( mapa.ocupado[comida.posicion.x][comida.posicion.y] );

        mapa.ocupado[comida.posicion.x][comida.posicion.y] = 'o';
        comida.existe = true;
    }

    if ( serpiente.direccion == IZQUIERDA )
        nuevaCoord.x--;
    else if ( serpiente.direccion == DERECHA )
        nuevaCoord.x++;
    else if ( serpiente.direccion == ARRIBA )
        nuevaCoord.y--;
    else
        nuevaCoord.y++;

    MESSAGE("%d %dA\n", nuevaCoord.x, nuevaCoord.y);
    MESSAGE("%d %dA\n", aux->x, aux->y);

    lugar = mapa.ocupado[nuevaCoord.x][nuevaCoord.y];

    MESSAGE("lugar %c %d\n", lugar, lugar);

    if ( lugar != '\0' && lugar != 'o') {
        aux2 = filtrar(peek(true));
        if ( (aux2->x != nuevaCoord.x) || (aux2->y != nuevaCoord.y) ) {

            MESSAGE("HE perdido\n");

            juego.perdido = true;
        }

        movil = sacar();
    } else if ( lugar == 'o') {
        mapa.ocupado[nuevaCoord.x][nuevaCoord.y] = '\0';
        comida.existe = false;

        MESSAGE("He comido\n");

        movil = new_node(malloc(sizeof(Coor)));
        play_sample(smp_mordida, 50, 250, 1000, 0);
    } else {

        MESSAGE("Me he movido\n");

        movil = sacar();
    }

    aux2 = filtrar(movil);
    *aux2 = nuevaCoord;

    insertar(movil);

}

bool misma_direccion(int32_t dir1, int32_t dir2) {
    return dir1 == dir2;
}

bool direccion_opuesta(int32_t dir1, int32_t dir2) {
    int8_t opuesta[4] = { 1, 0, 3, 2 };

    if ( dir1 >= 0 && dir1 < 4 ) {
        return opuesta[dir1] == dir2;
    }

    return false;
}

void pausa(void) {
    textout_ex(screen, font, "Se ha pausado el juego, presiona C para continuar", 100, 100, 0x000000, -1);
    while ( !key[KEY_C] ) rest(10);
}

static inline bool salir(void) { return key[KEY_ESC]; }

void liberarMemoria(void) {
    struct node * aux;

    while ( (aux = de_queue(serpiente.cuerpo)) ) {
        free(aux);
    }

    while ( (aux = de_queue(serpiente.proximas_direcciones)) ) {
        free(aux);
    }

    free(serpiente.cuerpo);
    free(serpiente.proximas_direcciones);

    destroy_bitmap(bmp_mapa);
    destroy_bitmap(bmp_serp_cabeza);
    destroy_bitmap(bmp_serp_cuerpo);
    destroy_bitmap(bmp_serp_cola);
    destroy_bitmap(bmp_serp_giro);
    destroy_bitmap(bmp_comida);
    destroy_bitmap(bmp_muro);
    destroy_bitmap(bmp_explosion);

    release_screen();
}
