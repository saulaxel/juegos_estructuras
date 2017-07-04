# No tengo la menor idea de como hacer un Makefile :S

# Compilador
CC = gcc
# Banderas
CFLAGS = -Wall -std=c11 -Ilib `allegro-config --cflags --libs`

snake: lib/dinamic_queue.c lib/dinamic_queue.h lib/recursos.c lib/recursos.h snake.c lib/config.h lib/init_allegro.c lib/init_allegro.h
	$(CC) -o snake snake.c lib/dinamic_queue.c lib/recursos.c lib/init_allegro.c $(CFLAGS)

