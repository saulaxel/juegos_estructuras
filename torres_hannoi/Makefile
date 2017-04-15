# No tengo la menor idea de como hacer un Makefile :S

# Compilador
CC = gcc
# Banderas
CFLAGS = -Wall -std=c11 -Ilib `allegro-config --cflags --libs`

torres_hannoi: lib/dinamic_stack.c lib/recursos.c torres_hannoi.c lib/recursos.h lib/dinamic_stack.h lib/config.h
	$(CC) -o torres_hannoi torres_hannoi.c lib/dinamic_stack.c lib/recursos.c $(CFLAGS)

