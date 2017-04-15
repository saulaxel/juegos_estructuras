#include "config.h"
#include "recursos.h"


int valor[NUM_DISCOS];
bool ganado;
Torre * torre[3];
int disco_tomado = -1;
int disco_dejado = -1;

void inicializarAllegro(void) {
    // InicializaciOn general
    allegro_init();

    set_window_title("Torres de Hanoi");

    // Graficos
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, ANCHO_PANTALLA, ALTO_PANTALLA, 0, 0);

    // Sonido
    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
    set_volume(70, 70);

    // Contador de tiempo
    install_timer();

    install_keyboard();

    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);

    install_mouse();
}

void cargarImagenes(void) {
    bmp_mapa = create_bitmap(ANCHO_PANTALLA, ALTO_PANTALLA);

    bmp_base = load_bitmap("img/base3.bmp", NULL);
    bmp_torre = load_bitmap("img/torre.bmp", NULL);

    bmp_disco = create_bitmap(ANCHO_DISCO, ALTO_DISCO);

    bmp_tubo = load_bitmap("img/tubo2.bmp", NULL);
    bmp_parte_tubo = create_bitmap(ANCHO_TUBO, ALTO_TUBO);

    bmp_mouse = load_bitmap("img/cursor.bmp", NULL);
}

void cargarAudios() {
}

void activarMusicaFondo() {
}
