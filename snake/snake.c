#include "lib/config.h"
#include "lib/recursos.h"

void jugar(void);

// Preambulo y fin
void intro(void);
void prepararJuego(void);
void cargarRecursos(void);
void liberarMemoria(void);

// Imagen
void desplegarImagen(void);
void pintarEscenario(void);
void pintarSerpiente(void);
void pintarFruta(void);

// Manejo de eventos
void interaccionUsuario();
static inline void pausa(const char * mensaje);
static inline bool salir(void);

// LOgica del juego
void calculos(void);
void actualizarDireccion();
void generarComida(void);
void moverCoord(Coor * coordenada);
void avanceSerpiente(char lugar, Coor * nuevaCoord);

static inline bool misma_direccion(int32_t dir1, int32_t dir2);
static inline bool direccion_opuesta(int32_t dir1, int32_t dir2);
static int32_t calcularRotacion(Coor * anterior, Coor * actual, Coor * siguiente);

static void tache_presionado(void) {
    liberarMemoria();
    exit(EXIT_SUCCESS);
}

// Principal
int main(int argc, char ** argv) {

    inicializarAllegro(tache_presionado);
    cargarRecursos();

    jugar();

    return 0;

} END_OF_MAIN()

void jugar(void) {
    // Preambulo

    prepararJuego();
    cargarAudios();

    intro();

    desplegarImagen();
    pausa("Presiona C para comenzar");

    // Juego
    while (!juego.perdido && !juego.ganado && !salir()) {

        desplegarImagen();

        interaccionUsuario();

        if (!(juego.contador % ACTUALIZACIONES[juego.velocidad])) calculos();

        rest(10); // Descanso para el cpu
        juego.contador++;
    }

    desplegarImagen();

    if (juego.perdido)
        play_sample(smp_muerte, 1000, 500, 1000, 0);

    rest(500);
}

void intro(void) {
    bool continuar = false;
    int32_t contador = 0;

    clear_to_color(screen, 0x7C9757);
    BITMAP * portada = create_bitmap(mapa.ancho, mapa.alto);
    stretch_blit(bmp_portada, portada, 0, 0, 600, 600, 0, 0, 640, 600);

    play_sample(smp_intro, 200, 100, 1000, 1);
    play_sample(smp_movimiento, 200, 100, 1000, 1);

    while (!continuar) {
        draw_sprite(screen, portada, 0, 0);

        textout_ex(screen, font, "Snake game: Presione C para continuar", 170, 220, 0x0, -1);

        if (contador > 140) {
            for (int32_t i = 0; i < 5; i++)
                draw_sprite(screen, bmp_comida, rand() % mapa.ancho, rand() % mapa.alto);
        }

        for (int32_t i = 0; i < 10; i++) {
            if (key[KEY_C]) continuar = true;
            rest(30);
            contador++;

            if (contador == 140)
                stop_sample(smp_movimiento);

        }
    }
    stop_sample(smp_intro);
    stop_sample(smp_movimiento);

    destroy_bitmap(portada);
}

// Preambulo y fin
void prepararJuego(void) {
    srand(time(NULL));

    cargarEscenarios();
    mapa.ocupado = escenario[1].mapa;
    mapa.lugares_disponibles = escenario[0].lugares_disponibles;

    crearSerpiente();
    serpiente.proximas_direcciones = new_queue();

    activarMusicaFondo();
}

void cargarRecursos(void) {
    cargarImagenes();
    cargarAudios();
}

void liberarMemoria(void) {
    struct node * aux;

    while ((aux = de_queue(serpiente.cuerpo))) {
        free(aux);
    }

    while ((aux = de_queue(serpiente.proximas_direcciones))) {
        free(aux);
    }

    free(serpiente.cuerpo);
    free(serpiente.proximas_direcciones);

    destruirImagenes();
    destruirAudios();

    release_screen();
    allegro_exit();
}

// Manejo de la imagen
void desplegarImagen(void) {

    clear_to_color(bmp_mapa, COLOR_FONDO);

    pintarEscenario();
    pintarFruta();
    pintarSerpiente();

    // Despliega todo lo pintado sobre la pantalla
    draw_sprite(screen, bmp_mapa, 0, 0);
}

void pintarEscenario(void) {
    for (int x = 0; x < COLUMNAS; x++)
        for (int y = 0; y < FILAS; y++)
            if (mapa.ocupado[x][y] == 'X')
                draw_sprite(bmp_mapa, bmp_muro, x * TAM_BLOQUE, y * TAM_BLOQUE);

}

void pintarFruta(void) {
    if (comida.existe) {
        draw_sprite(bmp_mapa, bmp_comida,
                comida.posicion.x * TAM_BLOQUE,
                comida.posicion.y * TAM_BLOQUE);
    }
}

void pintarSerpiente(void) {
    SerpNodo *aux;
    Coor *actual, *ant, *sig;
    int32_t angulo;

    BITMAP *sprite = create_bitmap(40, 40);
    clear_to_color(sprite, COLOR_FONDO);

    aux = peek(true);

    if (aux) {
        actual = filtrar(aux);
        sig = filtrar(aux->next);

        MESSAGE("%d %d\n", sig->x, actual->x);

        angulo = calcularRotacion(NULL, actual, sig);

        rotate_sprite(sprite, bmp_serp_cola, 0, 0, itofix(angulo));

        draw_sprite(bmp_mapa, sprite,
                actual->x * TAM_BLOQUE, actual->y * TAM_BLOQUE);
    }

    while ((aux = peek(false))) {
        clear_to_color(sprite, COLOR_FONDO);
        actual = filtrar(aux);
        ant = filtrar(aux->prev);

        if (aux->next) {
            sig = filtrar(aux->next);

            angulo = calcularRotacion(ant, actual, sig);

            rotate_sprite(sprite, angulo & 256 ? bmp_serp_giro : bmp_serp_cuerpo,
                    0, 0, itofix(angulo));
        } else {

            angulo = calcularRotacion(ant, actual, NULL);

            if (!juego.perdido) {
                rotate_sprite(sprite, bmp_serp_cabeza, 0, 0, itofix(angulo));
            } else {
                blit(bmp_mapa, sprite,
                        actual->x * TAM_BLOQUE, actual->y * TAM_BLOQUE,
                        0, 0, TAM_BLOQUE, TAM_BLOQUE);
                rotate_sprite(sprite, bmp_explosion, 0, 0, itofix(angulo));
            }
        }

        draw_sprite(bmp_mapa, sprite,
                actual->x * TAM_BLOQUE, actual->y * TAM_BLOQUE);
    }

    destroy_bitmap(sprite);
}

// Manejo de eventos
void interaccionUsuario(void) {
    static int32_t presionado = 1, ultimo = 1;

    // Revisa si estan presionadas las teclas de movimiento
    if      (key[KEY_LEFT]  || key[KEY_A]) presionado = IZQUIERDA;
    else if (key[KEY_RIGHT] || key[KEY_D]) presionado = DERECHA;
    else if (key[KEY_UP]    || key[KEY_W]) presionado = ARRIBA;
    else if (key[KEY_DOWN]  || key[KEY_S]) presionado = ABAJO;
    else if (key[KEY_P]) pausa("Presiona C para continuar");

    if (!misma_direccion(presionado, ultimo) &&
        !direccion_opuesta(presionado, ultimo)) {
        int32_t direction = ultimo = presionado;

        en_queue(serpiente.proximas_direcciones, new_dir(direction));
    }

}

static inline void pausa(const char * mensaje) {
    textout_ex(screen, font, mensaje, 100, 100, 0x000000, -1);
    while (!key[KEY_C]) rest(10);
}

static inline bool salir(void) { return key[KEY_ESC]; }

// LOgica del juego
void calculos(void) {
    char lugar;
    Coor nuevaCoord = *(filtrar(serpiente.cuerpo->rear));

    if (rand() % 50 == 0)
        play_sample(smp_movimiento, 200, 150, 1000, 0);

    actualizarDireccion();

    generarComida();

    moverCoord(&nuevaCoord);

    lugar = mapa.ocupado[nuevaCoord.x][nuevaCoord.y];

    avanceSerpiente(lugar, &nuevaCoord);

    MESSAGE("Restantes: %d %d %d\n", mapa.lugares_disponibles, serpiente.cuerpo->elements, mapa.lugares_disponibles - serpiente.cuerpo->elements);

    juego.ganado = (mapa.lugares_disponibles - serpiente.cuerpo->elements) < 10;

}

void actualizarDireccion(void) {
    if (!queue_is_empty(serpiente.proximas_direcciones)) {
        struct node * aux = de_queue(serpiente.proximas_direcciones);
        serpiente.direccion = ptr_to_int(aux->data);
        free(aux);
    }
}

void generarComida(void) {
    if (!comida.existe) {
        do {
            comida.posicion.x = 1 + rand() % (COLUMNAS - 2);
            comida.posicion.y = 1 + rand() % (FILAS - 2);
        } while( mapa.ocupado[comida.posicion.x][comida.posicion.y] );

        mapa.ocupado[comida.posicion.x][comida.posicion.y] = 'o';
        comida.existe = true;
    }
}

void moverCoord(Coor * coordenada) {
    if (serpiente.direccion == IZQUIERDA)
        coordenada->x = (coordenada->x - 1 + COLUMNAS) % COLUMNAS;
    else if (serpiente.direccion == DERECHA)
        coordenada->x = (coordenada->x + 1) % COLUMNAS;
    else if (serpiente.direccion == ARRIBA)
        coordenada->y = (coordenada->y - 1 + FILAS) % FILAS;
    else
        coordenada->y = (coordenada->y + 1) % FILAS;
}

void avanceSerpiente(char lugar, Coor * nuevaCoord) {
    SerpNodo * movil;
    Coor * aux;

    if (lugar != '\0' && lugar != 'o') {
        aux = filtrar(peek(true));
        if ((aux->x != nuevaCoord->x) || (aux->y != nuevaCoord->y)) {

            MESSAGE("HE perdido\n");

            juego.perdido = true;
        }

        movil = sacar();
    } else if (lugar == 'o') {
        mapa.ocupado[nuevaCoord->x][nuevaCoord->y] = '\0';
        comida.existe = false;

        MESSAGE("He comido\n");

        movil = new_node(malloc(sizeof(Coor)));
        play_sample(smp_mordida, 50, 250, 1000, 0);
    } else {

        MESSAGE("Me he movido\n");

        movil = sacar();
    }

    aux = filtrar(movil);
    *aux = *nuevaCoord;

    insertar(movil);
}

static int calcularRotacion(Coor * anterior, Coor * actual, Coor * siguiente) {
    int angulo_rotacion; // 0-255

    // WTFFFFF??? En serio esto no se puede simplificar?
    if (!anterior) {
        if (actual->x == (siguiente->x + 1) % COLUMNAS)
            angulo_rotacion= 0;
        else if (actual->x == (siguiente->x - 1 + COLUMNAS) % COLUMNAS)
            angulo_rotacion = 128;
        else if (actual->y == (siguiente->y + 1) % FILAS)
            angulo_rotacion = 64;
        else
            angulo_rotacion = 192;
    } else if (siguiente) {
        int delta_x[2] = { 0 };
        int delta_y[2] = { 0 };

        if (anterior->x == (actual->x + 1) % COLUMNAS)
            delta_x[0] = 1;
        else if (anterior->x == (actual->x - 1 + COLUMNAS) % COLUMNAS)
            delta_x[0] = -1;
        else if (actual->x == (siguiente->x + 1) % COLUMNAS)
            delta_x[1] = 1;
        else if (actual->x == (siguiente->x - 1 + COLUMNAS) % COLUMNAS)
            delta_x[1] = -1;

        if (anterior->y == (actual->y + 1) % FILAS)
            delta_y[0] = 1;
        else if (anterior->y == (actual->y - 1 + FILAS) % FILAS)
            delta_y[0] = -1;
        else if (actual->y == (siguiente->y + 1) % FILAS)
            delta_y[1] = 1;
        else if (actual->y == (siguiente->y - 1 + FILAS) % FILAS)
            delta_y[1] = -1;

        if (!delta_x[0] && !delta_x[1]) {
            angulo_rotacion = 64;
        } else if (!delta_y[0] && !delta_y[1]) {
            angulo_rotacion = 0;
        } else {
            if (!delta_y[0]) {
                if (delta_x[0] == 1 && delta_y[1] == 1)
                    angulo_rotacion = 192;
                else if (delta_x[0] == 1 && delta_y[1] == -1)
                    angulo_rotacion = 0;
                else if (delta_y[1] == 1)
                    angulo_rotacion = 128;
                else
                    angulo_rotacion = 64;

            } else {
                if (delta_y[0] == 1 && delta_x[1] == 1)
                    angulo_rotacion = 64;
                else if (delta_y[0] == 1 && delta_x[1] == -1)
                    angulo_rotacion = 0;
                else if (delta_x[1] == 1)
                    angulo_rotacion = 128;
                else
                    angulo_rotacion = 192;
            }

            angulo_rotacion += 256;
        }
    } else {
        if (anterior->x == (actual->x - 1 + COLUMNAS) % COLUMNAS)
            angulo_rotacion = 128;
        else if (anterior->x == (actual->x + 1) % COLUMNAS)
            angulo_rotacion = 0;
        else if (anterior->y  == (actual->y - 1 + FILAS) % FILAS)
            angulo_rotacion = 192;
        else
            angulo_rotacion = 64;
    }


    return angulo_rotacion;
}

static inline bool misma_direccion(int32_t dir1, int32_t dir2) {
    return dir1 == dir2;
}

static inline bool direccion_opuesta(int32_t dir1, int32_t dir2) {
    static const int8_t opuesta[4] = { 1, 0, 3, 2 };

    if (dir1 >= 0 && dir1 < 4)
        return opuesta[dir1] == dir2;

    return false;
}

