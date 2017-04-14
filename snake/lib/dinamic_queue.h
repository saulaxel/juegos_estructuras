/*
 * (C) Copyright 2017 Martinez Ortiz Saul UNAM
 *
 * This program is free software: you can redistribute it and/or modify
 * Tt under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _DINAMIC_QUEUE_H
#define _DINAMIC_QUEUE_H

/*-*-*-*-*-*-*-*-*-*-*
 - Data definitions  -
 *-*-*-*-*-*-*-*-*-*-*/
//#define STORE_TYPE
//#define STORE_ELEMENTS_NUM
//#define DOUBLE_LINKED

#ifndef _SIMPLE_NODE_DEFINED
#define _SIMPLE_NODE_DEFINED

struct node {
    void * __restrict data;

#ifdef STORE_TYPE
    char type;
#endif /* STORE_TYPE */

    struct node * __restrict next;
#ifdef DOUBLE_LINKED
    struct node * __restrict prev;
#endif
};

#endif // _SIMPLE_NODE_DEFINED

struct queue {
    struct node * __restrict head;
    struct node * __restrict rear;

#ifdef STORE_ELEMENTS_NUM
    int16_t elements;
#endif /* STORE_ELEMENTS_NUM */
};


#include "config.h"

/*-*-*-*-*-*-*-*-*
 -  Dependencies -
 *-*-*-*-*-*-*-*-*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef STORE_TYPE
#include <stdint.h>
#endif /* STORE_TYPE */

/*-*-*-*-*-*-*-*-*-*-*
 - Data definitions  -
 *-*-*-*-*-*-*-*-*-*-*/
//#define STORE_TYPE
//#define STORE_ELEMENTS_NUM
//#define DOUBLE_LINKED

/*-*-*-*-*-*-*-*-*-*-*-*-*
 - Function declarations -
 *-*-*-*-*-*-*-*-*-*-*-*-*/

/* Allocate and free memory*/
extern struct queue * new_queue(void);
void free_queue(struct queue * __restrict q);

#ifdef STORE_TYPE
struct node * new_node(void * __restrict data, const char type);
#else
struct node * new_node(void * __restrict data);
#endif

/* Managing data */
bool en_queue(struct queue * __restrict s, struct node * __restrict n);
struct node * de_queue(struct queue * __restrict s);
void * queue_peek(const struct queue * __restrict q, bool from_start);

extern bool queue_is_empty(const struct queue * __restrict q);

#endif /* _DINAMIC_QUEUE_H */
