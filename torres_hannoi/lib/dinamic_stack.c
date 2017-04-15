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
 * Library: dinamic_queue - Functions -
 * Description: A stack implementation with structs in C.
 *#########################################################*/
#include "config.h"
#include "dinamic_stack.h"

/*-*-*-*-*-*-*-*-*-*-*-*-*
 - Function definitions  -
 *-*-*-*-*-*-*-*-*-*-*-*-*/

/*
 * Function    : new_stack
 * Description : Allocates memory for struct stech.
 *
 * Returns     : The direction of allocated memory if any,
 *               NULL otherwise.
 */
inline struct stack * new_stack(void) {
    return (struct stack *) calloc(1, sizeof(struct stack));
}

void free_stack(struct stack * __restrict s, bool free_storage) {
    struct node * aux;

    while( !stack_is_empty(s) ) {
        aux = pop(s);

        if( free_storage ) free(aux->data);

        free(aux);
    }

    free(s);
}

#ifdef STORE_TYPE
struct node * new_node(void * __restrict data, const char type) {
#else /* not defined STORE_TYPE */
struct node * new_node(void * __restrict data) {
#endif /* end STORE_TYPE */

    if( !data ) return NULL;

    struct node * new =
        (struct node *) calloc(1, sizeof(struct node));

    new->data = data;

#ifdef STORE_TYPE
    new->type = type;
#endif /* end STORE_TYPE */

    return new;
}


bool push(struct stack  * __restrict s, struct node * __restrict n) {
    if( n ) {

        if( !stack_is_empty(s) ) {
            n->next = s->head;
#ifdef DOUBLE_LINKED
            s->head->prev = n;
#endif /* DOUBLE_LINKED */
        } else {
            s->rear = n;
        }

        s->head = n;

#ifdef STORE_ELEMENTS_NUM
        s->elements++;
#endif // end _STORE_ELEMENTS_NUM

        return true;
    }

    return false;
}

struct node * pop(struct stack * __restrict s) {
    struct node * tmp;

    tmp = s->head;

    if( !stack_is_empty(s) ) {
         s->head = tmp->next;

#ifdef STORE_ELEMENTS_NUM
         s->elements--;
#endif // end _STORE_ELEMENTS_NUM

#ifdef DOUBLE_LINKED
         if (tmp->next) tmp->next->prev = NULL;
#endif
         tmp->next = NULL;
    }

    if( stack_is_empty(s) ) {
        s->rear = NULL;
    }

    return tmp;
}

struct node * stack_peek(const struct stack * __restrict s, bool from_start) {
    static struct node * apt;

    if( from_start ) {
        apt = s->head;
    } else if( apt ) {
        apt = apt->next;
    }

    return apt;
}

#ifdef DOUBLE_LINKED
struct node * stack_peek_reverse(const struct stack * __restrict s, bool from_end) {
    static struct node * apt;

    if( from_end ) {
        apt = s->rear;
    } else if( apt ) {
        apt = apt->prev;
    }

    return apt;
}
#endif /* DOUBLE_LINKED */

inline bool stack_is_empty(const struct stack * __restrict s) {
    return s->head == NULL;
}

