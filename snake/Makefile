# No tengo la menor idea de como hacer un Makefile :S

# Compilador
CC = gcc
# Banderas
CFLAGS = -Wall -std=c11 -Ilib `allegro-config --cflags --libs`

snake: lib/dinamic_queue.c lib/recursos.c snake.c lib/config.h
	$(CC) -o snake snake.c lib/dinamic_queue.c lib/recursos.c $(CFLAGS)

