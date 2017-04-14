#include "allegro.h"
#include "config.h"
#include <stdlib.h>
#include <stdbool.h>

#define TAM_BLOQUE 30

#ifdef __unix__
    #include "libs/cargar_mapa.h"
    #include "libs/init_allegro.h"
#else
    #include "libs\cargar_mapa.h"
    #include "libs\init_allegro.h"
#endif

uint16_t columnas, filas; // Tamanio del mapa
uint16_t comida;

typedef struct monigote {
    char * nombre_imagen;
    int32_t dir;
    int32_t x;
    int32_t y;
    int32_t pas_x;
    int32_t pas_y;
    BITMAP * bmp_img;
}Monigote;

void jugar(void);

uint16_t dibujar_mapa(char ** map, BITMAP * map_buff, uint16_t rows, uint16_t cols);
void display(BITMAP * map_buff);
void direccion_jugador(char ** map, Monigote * jugador, BITMAP *pacbmp,bool turno, bool par);
void intentar_comer(char ** map, const Monigote * pac);
void direccion_fantasma(char ** map, Monigote * ghost);
void mover_monigote(Monigote * m);
void dibujar_monigote(BITMAP * map_buff, const Monigote * m);
void morir(char ** mapa, BITMAP * map_buff, Monigote * pac);
bool colision(Monigote * m1, Monigote * m2);
bool se_puede_mover(char ** map, Monigote * m);
bool direccion_opuesta(int32_t dir1, int32_t dir2);

int main(int argc, char ** argv) {
    jugar();
    return 0;
}

END_OF_MAIN();

void jugar(void){
    char ** mapa = cargar_mapa("pac_map.dat", &columnas, &filas);
    BITMAP * map_buffer;
    MIDI * musica;
    uint16_t comida = 0;
    uint16_t vidas  = 4;
    bool ganado = false;
    bool perdido = false;
    uint32_t contador;
    uint8_t i;

    Monigote pacman = { "img" DIAG "pacman.bmp", 4, 1, 1 , 1, 1, NULL};
    Monigote fantasma[5] = {
            { "img" DIAG "enemigo.bmp", 4, 11, 11 },
            { "img" DIAG "enemigo.bmp", 4, 11, 11 },
            { "img" DIAG "enemigo.bmp", 4, 11, 11 },
            { "img" DIAG "enemigo.bmp", 4, 11, 11 },
            { "img" DIAG "enemigo.bmp", 4, 11, 11 }
    };

    inicializar_allegro();

    map_buffer = create_bitmap(970, 600);

    BITMAP * pacbmp = load_bitmap(pacman.nombre_imagen, NULL);
    pacman.bmp_img = create_bitmap(33, 33);

    musica = load_midi("sound" DIAG "mario.mid");

    fantasma[0].bmp_img = load_bitmap(fantasma[0].nombre_imagen, NULL);
    for(int f = 1; f < 5; f++)
        fantasma[f].bmp_img = fantasma[0].bmp_img;

#ifndef NDEBUG
    /*getchar();*/
    printf("%d %d \n", filas, columnas);
    printf("%d\n", vidas);
#endif

    play_midi(musica, 1);
    while( !key[KEY_ESC] && !ganado && !perdido ) {
        contador++;
        clear(map_buffer);

        for(i = 0; i < 5; i++)
            direccion_fantasma(mapa, fantasma + i);

        comida = dibujar_mapa(mapa, map_buffer, filas, columnas);

        for(i = 0; i < 7; i++) {
            direccion_jugador(mapa, &pacman, pacbmp, !i, contador % 2);
#ifdef  _WIN32
            rest(20); // Usar rest es lo adecuado, sin embargo no me funciona
                       // bien en linux por alguna razOn
#else
            system("sleep .02");
#endif
        }

        dibujar_monigote(map_buffer, &pacman);
        for(i = 0; i < 5; i++)
            dibujar_monigote(map_buffer, fantasma + i);

        intentar_comer(mapa, &pacman);

        display(map_buffer);

#ifndef NDEBUG
        printf("pacman\n");
        printf("\t%d\t%d\t%d\t%d\n", pacman.x, pacman.y,
                pacman.pas_x, pacman.pas_y);
        printf("fantasma\n");
        for(i = 0; i < 5; i++) {
            printf("\t%d\t%d\t%d\t%d\n", fantasma[i].x, fantasma[i].y,
                fantasma[i].pas_x, fantasma[i].pas_y);
        }
#endif
        if( contador > 5 ) {
            for(i = 0; i < 7; i++)
                if( colision(&pacman, fantasma + i) ) {
                    vidas--;
                    contador = 0;
                    morir(mapa, map_buffer, &pacman);
            }
        }

        perdido = (vidas == 0);
        ganado = (comida == 0);
    }

    liberar_mapa(mapa, filas);
}

uint16_t dibujar_mapa(char ** map, BITMAP * map_buff, uint16_t rows, uint16_t cols) {
    int_fast16_t i, j;
    static BITMAP * roca_bmp = NULL;
    static BITMAP * comida_bmp = NULL;
    int16_t comida = 0;

    if( !roca_bmp ) roca_bmp = load_bitmap("img" DIAG "roca.bmp", NULL);
    if( !comida_bmp ) comida_bmp = load_bitmap("img" DIAG "Comida.bmp", NULL);

    for(i = 0; i < rows; i++) {
        for(j = 0; j < cols; j++) {
            if( map[i][j] == 'X' ) {
                draw_sprite(map_buff, roca_bmp, j * TAM_BLOQUE, i * TAM_BLOQUE);
            } else if( map[i][j] == 'o' ) {
                draw_sprite(map_buff, comida_bmp, j * TAM_BLOQUE, i * TAM_BLOQUE);
                comida++;
            }
        }
    }

    return comida;
}

void direccion_jugador(char ** map, Monigote * jugador, BITMAP *pacbmp, bool turno, bool par) {
    static int32_t new_dir, old_dir;
    static SAMPLE *sonido_movimiento;
    Monigote aux = *jugador;

    if( !sonido_movimiento ) sonido_movimiento = load_wav("sound" DIAG "jump.wav");

    if     ( key[KEY_LEFT]  ) new_dir = 0;
    else if( key[KEY_RIGHT] ) new_dir = 1;
    else if( key[KEY_UP]    ) new_dir = 2;
    else if( key[KEY_DOWN]  ) new_dir = 3;

    old_dir = aux.dir;
    aux.dir = new_dir;

    if( se_puede_mover(map, &aux) )
        jugador->dir = new_dir;

    if( turno && se_puede_mover(map, jugador) )
        if( !direccion_opuesta(new_dir, old_dir) ) {
            play_sample(sonido_movimiento, 100, 150, 1000, 0);
            mover_monigote(jugador);
        }

    blit(pacbmp, jugador->bmp_img, 33 * jugador->dir + 132 * par, 0, 0, 0, 33, 33);

}

void direccion_fantasma(char ** map, Monigote * ghost) {

    int32_t dir_ant = ghost->dir;

    do{
        ghost->dir = rand() % 4;

    }while( !se_puede_mover(map, ghost) ||
            direccion_opuesta(dir_ant, ghost->dir) );
    /*printf("Dir %d\n", ghost->dir);*/

    mover_monigote(ghost);
}

void intentar_comer(char ** map, const Monigote * pac) {
    static SAMPLE * sonido_comer = NULL;
    if( !sonido_comer ) sonido_comer = load_wav("sound" DIAG "coin.wav");

    if( map[pac->y][pac->x] == 'o' ) {
        map[pac->y][pac->x] = ' ';
        play_sample(sonido_comer, 300, 150, 1000, 0);
    }
}

void dibujar_monigote(BITMAP * map_buff, const Monigote * m) {
    draw_sprite(map_buff, m->bmp_img,
            m->x * TAM_BLOQUE, m->y * TAM_BLOQUE);
}

void display(BITMAP * map_buff) {
    blit(map_buff, screen, 0, 0, 0, 0, 870, 600);
}

void mover_monigote(Monigote * m) {
    if( m->dir >= 0 && m->dir < 4 ) {
        m->pas_x = m->x;
        m->pas_y = m->y;
    }
    switch( m->dir ) {
        case 0: // Izquierda
            m->x--;

            if( m->x < 0 ) m->x = columnas - 1;
            break;

        case 1: // Derecha
            m->x++;

            if( m->x >= columnas ) m->x = 0;
            break;

        case 2: // Arriba
            m->y--;

            if( m->y < 0 ) m->y = filas - 1;
            break;

        case 3: //Abajo
            m->y++;

            if( m->y >= filas ) m->y = 0;
            break;

    }
}

void morir(char ** map, BITMAP *map_buff, Monigote * pac) {
    BITMAP * muertebmp = load_bitmap("img" DIAG "muerte.bmp", NULL);
    int8_t i;
    static SAMPLE *sonido_muerte;

    if( !sonido_muerte ) sonido_muerte = load_wav("sound" DIAG "Muerte.wav");

    play_sample(sonido_muerte, 100, 150, 1000, 0);
    for(i = 0; i <= 6; i++) {
        clear(pac->bmp_img);
        clear(map_buff);
        dibujar_mapa(map, map_buff, filas, columnas);

        blit(muertebmp, pac->bmp_img, i * 33, 0, 0, 0, 33, 33);
        draw_sprite(map_buff, pac->bmp_img,
                pac->x * TAM_BLOQUE, pac->y * TAM_BLOQUE);
#ifdef _WIN32
        rest(80);
#else
        system("sleep 0.08");
#endif
        display(map_buff);
    }

    pac->x = pac->y = 1;
}

bool colision(Monigote * m1, Monigote * m2) {
    if( m1->x == m2->x && m1->y == m2->y )
        return true;
    else if( m2->x == m1->pas_x &&
             m2->y == m1->pas_y &&
             m1->x == m2->pas_x &&
             m1->y == m2->pas_y )
        return true;

    return false;
}

bool se_puede_mover(char ** map, Monigote * m) {
    switch( m->dir ) {
        case 0: // Izquierda
            if(m->x <= 0) return true;

            if( map[m->y][m->x - 1] == 'X' ) {
                return false;
            } else {
                return true;
            }

            break;

        case 1: // Derecha
            if(m->x >= columnas - 1) return true;

            if( map[m->y][m->x + 1] == 'X' ) {
                return false;
            } else {
                return true;
            }

            break;

        case 2: // Arriba
            if(m->y <= 0) return true;

            if( map[m->y - 1][m->x] == 'X' ) {
                return false;
            } else {
                return true;
            }

            break;

        case 3: //Abajo
            if(m->y >= filas - 1) return true;

            if( map[m->y + 1][m->x] == 'X' ) {
                return false;
            } else {
                return true;
            }

            break;

    }
    return false;
}

bool direccion_opuesta(int32_t dir1, int32_t dir2) {
    int8_t opuesta[4] = { 1, 0, 3, 2 };

    if( dir1 >= 0 && dir1 < 4 ) {
        return opuesta[dir1] == dir2;
    }

    return false;
}
