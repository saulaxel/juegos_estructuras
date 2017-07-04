#include <stdio.h>
#include "init_allegro.h"

void init_allegro(const char * titulo,
        int ancho, int alto,
        int vol_digi, int vol_midi) {

    if (allegro_init() != 0) {
        fprintf(stderr, "Fallo al inicializar allegro\n");
        exit(EXIT_FAILURE);
    }

    if(install_keyboard() != 0) {
        fprintf(stderr, "Fallo al inicializar el teclado\n");
        exit(EXIT_FAILURE);
    }

    set_window_title(titulo);
    set_color_depth(32);

    set_gfx_mode(GFX_AUTODETECT_WINDOWED,
            ancho, alto, 0, 0); 

    if( install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) != 0) {
        allegro_message("Error al inicializar el sonido: %s\n", allegro_error);
    }

    set_volume(vol_digi, vol_midi);
    install_timer();
}
