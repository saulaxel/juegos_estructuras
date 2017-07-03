#include "lib/config.h"
#include "lib/recursos.h"

#define LAPSO_DORMIR 10
#define SALIR 0

void jugar();
// Se divide en
void prepararJuego(void);

void desplegarImagen(void);
void interaccionUsuario(void);
void calculos(void);
void dormir(int milis);

bool rangoTorre(int numero_torre);
bool movimientoValido(int torre_origen, int torre_destino);
void separarImagenDisco(int val);
bool atajoTeclado(int torre);

void liberarEspacio();

void pausa();
bool salir();

int main(void) {
    jugar();
    return 0;
}

END_OF_MAIN()

void jugar(void) {

    prepararJuego();

    ganado = false;
    while( !ganado && !salir() ) {
        desplegarImagen();
        interaccionUsuario();
        calculos();
    }


    while(!salir()) {
        textout_ex(screen, font, "Congratulashion: has ganado (ESC para salIr)", 100, 100, 0x0, -1);
        rest(20);
        draw_sprite(screen, bmp_mouse, mouse_x, mouse_y);
    }

    liberarEspacio();
}

void prepararJuego() {
    int i;
    torre[0] = new_stack(); torre[1] = new_stack(); torre[2] = new_stack();

    for(i = NUM_DISCOS - 1; i >= 0; i--) {
        valor[i] = i;
        Disco * d = new_node(&valor[i]);
        push(torre[0], d);
    }

    inicializarAllegro();
    cargarImagenes();
    cargarAudios();

    activarMusicaFondo();
}

void desplegarImagen(void) {
    int i, j;
    int val;
    int posicion_x, posicion_y;

    Disco * aux, * aux2;

    clear_to_color(bmp_mapa, BLANCO);

    draw_sprite(bmp_mapa, bmp_base, 20, ALTO_PANTALLA - 220);

    for(i = 0; i < 3; i++) { // Se hace el proceso para las tres torres

        aux = stack_peek_reverse(torre[i], true);

        for(j = 0; j < torre[i]->elements - 1; j++) {

            posicion_x = 50 + (ANCHO_DISCO * 3 / 2 + 30) * i;
            posicion_y = ALTO_PANTALLA - 150 - j * (ALTO_DISCO - SOBRANTE);

            val = *((int *) aux->data);
            separarImagenDisco(val);

            draw_sprite(bmp_mapa, bmp_disco, posicion_x, posicion_y);

            aux = stack_peek_reverse(torre[i], false);

        }
        // Si la pila no esta vacIa
        if( aux ) {
            val = *((int *) aux->data);
            separarImagenDisco(val);

            if( disco_tomado != i ) {
                posicion_x = 50 + (ANCHO_DISCO * 3 / 2 + 30) * i;
                posicion_y = ALTO_PANTALLA - 150 - j* (ALTO_DISCO - SOBRANTE);
                draw_sprite(bmp_mapa, bmp_disco, posicion_x, posicion_y);
            } else {
                aux2 = aux;
            }

        }


        if( disco_tomado == i || stack_is_empty(torre[i]) ) {
            j = j - 1;
        }

        for(int l = 0; j < NUM_DISCOS; j++, l++) {
            if( l ) {
                blit(bmp_tubo, bmp_parte_tubo, 10, 0, 0, 0, ANCHO_TUBO, ALTO_TUBO);
            } else {
                blit(bmp_tubo, bmp_parte_tubo, 0, 0, 0, 0, ANCHO_TUBO, ALTO_TUBO);
            }
                posicion_x = 105 + (ANCHO_DISCO * 3 / 2 + 30) * i;
                posicion_y = ALTO_PANTALLA - 150 - j* (ALTO_DISCO - SOBRANTE);

            draw_sprite(bmp_mapa, bmp_parte_tubo, posicion_x, posicion_y);

#ifndef NDEBUG
            printf("He entrado %d, elem %d, j %d\n", l + 1, torre[i]->elements, j);
#endif
        }

        blit(bmp_tubo, bmp_parte_tubo, 20, 0, 0, 0, ANCHO_TUBO, ALTO_TUBO);

        posicion_x = 105 + (ANCHO_DISCO * 3 / 2 + 30) * i;
        posicion_y = ALTO_PANTALLA -160 - j * (ALTO_DISCO - SOBRANTE);

        draw_sprite(bmp_mapa, bmp_parte_tubo, posicion_x, posicion_y);
    }

    if( disco_tomado != -1 ) {
        val = *((int *) aux2->data);
        separarImagenDisco(val);

        posicion_x = mouse_x - ANCHO_DISCO / 2;
        posicion_y = mouse_y - ALTO_DISCO / 2;

        draw_sprite(bmp_mapa, bmp_disco, posicion_x, posicion_y);

    }

    draw_sprite(bmp_mapa, bmp_mouse, mouse_x, mouse_y);

    blit(bmp_mapa, screen, 0, 0, 0, 0, ANCHO_PANTALLA, ALTO_PANTALLA);

}

void separarImagenDisco(int val) {
    blit(bmp_torre, bmp_disco, 0, (ALTO_DISCO) * val, 0, 0, ANCHO_DISCO, ALTO_DISCO);
}

void interaccionUsuario() {
    int i;

    for(i = 0; i < 3; i++) {
        if( (rangoTorre(i) && mouse_b == 1) || atajoTeclado(i) ) {
                if( disco_tomado == -1 && !stack_is_empty(torre[i]) ) {
                    disco_tomado = i;
                } else if( disco_tomado != -1){
                    disco_dejado = i;
                }

                rest(150);
            }
    }
}

void calculos(void) {
    int val_origen, val_destino;

    // Si hay torre de origen y destino seleccionadas
    if( disco_tomado != -1 && disco_dejado != -1) {
        val_origen = *( (int *) stack_peek(torre[disco_tomado], true)->data );

        if( !stack_is_empty(torre[disco_dejado]) ) {
            val_destino = *( (int *) stack_peek(torre[disco_dejado], true)->data);
        } else {
            val_destino = NUM_DISCOS;
        }

#ifndef NDEBUG
        printf("val_origen %d, val_destino %d\n", val_origen, val_destino);
#endif

        if( val_origen <= val_destino ) {
            push(torre[disco_dejado], pop(torre[disco_tomado]));
        }

        disco_tomado = disco_dejado = -1;
    }

    ganado = (torre[2]->elements == NUM_DISCOS);
}

bool rangoTorre(int numero_torre) {
    int ymax, ymin;
    int xmax, xmin;

    ymin = ALTO_PANTALLA - NUM_DISCOS * (ALTO_DISCO - SOBRANTE) - 120;
    ymax = ALTO_PANTALLA - 120;

#ifndef NDEBUG
    printf("ymin %d ymax %d\n", ymin, ymax);
#endif

    if( mouse_y >= ymin && mouse_y <= ymax ) {

        switch(numero_torre) {
            case 0:
                xmin = 50;
                xmax = 170;
                break;
            case 1:
                xmin = 260;
                xmax = 380;
                break;

            case 2:
                xmin = 470;
                xmax = 590;
                break;
        }

        if( mouse_x >= xmin && mouse_x <= xmax ) {
            rest(100);
            return true;
        }
    }

    return false;
}

bool atajoTeclado(int torre) {
    if( torre == 0 ) {
        return key[KEY_1] || key[KEY_A];
    } else if( torre == 1 ) {
        return key[KEY_2] || key[KEY_S];
    } else if( torre == 2 ) {
        return key[KEY_3] || key[KEY_D];
    }

    return false;
}

void liberarEspacio() {
    destroy_bitmap(bmp_mapa);
    destroy_bitmap(bmp_torre);
    destroy_bitmap(bmp_disco);
    destroy_bitmap(bmp_base);
    destroy_bitmap(bmp_tubo);
    destroy_bitmap(bmp_parte_tubo);

    free_stack(torre[0], false);
    free_stack(torre[1], false);
    free_stack(torre[2], false);
}

void dormir(int milis) {
    rest(milis);
}

bool salir(void) {
    rest(2);
    return key[KEY_ESC];
}
