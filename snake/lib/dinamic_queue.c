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

#include "config.h"
#include "dinamic_queue.h"

/*-*-*-*-*-*-*-*-*-*-*-*-*
 - Function definitions  -
 *-*-*-*-*-*-*-*-*-*-*-*-*/

inline struct queue * new_queue(void) {
    return (struct queue *) calloc(1, sizeof(struct queue));
}

void free_queue(struct queue * __restrict q) {
    while( !queue_is_empty(q) )
        free(de_queue(q));

    free(q);
}

#ifdef STORE_TYPE
struct node * new_node(void * __restrict data, const char type) {
#else /* not defined STORE_TYPE */
struct node * new_node(void * data) {
#endif /* end STORE_TYPE */

    if( !data ) return NULL;

    struct node * nq =
        (struct node *) calloc(1, sizeof(struct node));

    nq->data = data;

#ifdef STORE_TYPE
    nq->type = type;
#endif /* end STORE_TYPE */

    return nq;
}

bool en_queue(struct queue  * __restrict q, struct node * __restrict n) {
    if( n ) {

        if( !queue_is_empty(q) ) {
            q->rear->next = n;

#ifdef DOUBLE_LINKED
            n->prev = q->rear;
#endif /*end DOUBLE_LINKED*/
        } else {
            q->head = n;
        }

        q->rear = n;

#ifdef STORE_ELEMENTS_NUM
        q->elements++;
#endif // end STORE_ELEMENTS_NUM

        return true;
    }

    return false;
}

struct node * de_queue(struct queue * __restrict q) {
    struct node * tmp;

    tmp = q->head;

    if( !queue_is_empty(q) ) {
         q->head = q->head->next;
#ifdef DOUBLE_LINKED
         if( q->head ) q->head->prev = NULL;
#endif /*end DOUBLE_LINKED*/

#ifdef STORE_ELEMENTS_NUM
         q->elements--;
#endif // STORE_ELEMENTS_NUM

         tmp->next = NULL;
#ifdef DOUBLE_LINKED
         tmp->prev = NULL;
#endif /*end DOUBLE_LINKED*/
    }

    return tmp;
}

inline bool queue_is_empty(const struct queue * __restrict q) {
    return q->head == NULL;
}

void * queue_peek(const struct queue * __restrict q, bool from_start) {
    static struct node * apt;

    if( from_start ) { apt = q->head;   }
    else if( apt )   { apt = apt->next; }

    return apt;
}
