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

/*#########################################################
 * Library: dinamic_queue - Header -
 * Description: A stack implementation with structs in C.
 *#########################################################*/

#ifndef _DINAMIC_STACK_H
#define _DINAMIC_STACK_H

#include "config.h"
/*-*-*-*-*-*-*-*-*
 -  Dependencies -
 *-*-*-*-*-*-*-*-*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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

#ifndef _SIMPLE_NODE_DEFINED
#define _SIMPLE_NODE_DEFINED

struct node {
    void * data;

#ifdef STORE_TYPE
    char type;
#endif /* end STORE_TYPE */

    struct node * __restrict next;
#ifdef DOUBLE_LINKED
    struct node * __restrict prev;
#endif /* end DOUBLE_LINKED */

};

#endif // end _SIMPLE_NODE_DEFINED

struct stack {
    struct node * __restrict head;
    struct node * __restrict rear;
#ifdef STORE_ELEMENTS_NUM
    int32_t elements;
#endif /* STORE_ELEMENTS_NUM */
};

/*-*-*-*-*-*-*-*-*-*-*-*-*
 - Function declarations -
 *-*-*-*-*-*-*-*-*-*-*-*-*/

/*-* Allocate and free memory*-*/
extern struct stack * new_stack(void);
void free_stack(struct stack * __restrict s, bool free_storage);

#ifndef _NEW_NODE_DECLARED
#ifdef STORE_TYPE
struct node * new_node(void * __restrict data, const char type);
#else
struct node * new_node(void * __restrict data);
#endif // end STORE_TYPE
#endif /* _NEW_NODE_DECLARED */

/*-* Managing data *-*/
bool push(struct stack * __restrict s, struct node * __restrict n);
struct node * pop(struct stack * __restrict s);
struct node * stack_peek(const struct stack * __restrict s, bool from_start);
struct node * stack_peek_reverse(const struct stack * __restrict s, bool from_end);
/*-* Auxiliar functions*-*/
extern bool stack_is_empty(const struct stack * __restrict s);

#endif /* _DINAMIC_STACK_H */
