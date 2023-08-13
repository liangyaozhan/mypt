/*
 * File: myptm_queue.h
 * Author: Liang YaoZhan <262666882@qq.com>
 * Brief:  timer
 *
 * Copyright (c) 2022 - 2023 Liang YaoZhan <262666882@qq.com>
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * History:
 * ================================================================
 * 2023-08-05 Liang YaoZhan <262666882@qq.com> created
 *
 */

#ifndef myptm_QUEUE_H
#define myptm_QUEUE_H

#include <assert.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct myptm_queue_s myptm_queue_t;

    struct myptm_queue_s
    {
        myptm_queue_t *prev;
        myptm_queue_t *next;
    };

#define myptm_queue_init(q) \
    do                      \
    {                       \
        (q)->prev = q;      \
        (q)->next = q;      \
    } while (0)

#define myptm_queue_empty(h) \
    (h == (h)->prev)

#define myptm_queue_insert_head(h, x) \
    do                                \
    {                                 \
        (x)->next = (h)->next;        \
        (x)->next->prev = x;          \
        (x)->prev = h;                \
        (h)->next = x;                \
    } while (0)

#define myptm_queue_insert_after myptm_queue_insert_head

#define myptm_queue_insert_tail(h, x)                                                     \
    do                                                                                    \
    {                                                                                     \
        assert((x)->next == (x)->prev && (x)->next == (x) && "insert twice or not init"); \
        (x)->prev = (h)->prev;                                                            \
        (x)->prev->next = x;                                                              \
        (x)->next = h;                                                                    \
        (h)->prev = x;                                                                    \
    } while (0)

#define myptm_queue_head(h) \
    (h)->next

#define myptm_queue_last(h) \
    (h)->prev

#define myptm_queue_next(q) \
    (q)->next

#define myptm_queue_prev(q) \
    (q)->prev

#define myptm_queue_remove(x)              \
    do                                     \
    {                                      \
        myptm_queue_t *p_prev = (x)->prev; \
        myptm_queue_t *p_next = (x)->next; \
        p_next->prev = p_prev;             \
        p_prev->next = p_next;             \
        myptm_queue_init(x);               \
    } while (0)

/**
 * origin:      h ==A=== q  ====B=== h
 * after list 1:      h ==A=== h
 * after list 2:      n q==B=== n
 */
#define myptm_queue_split(h, q, n) \
    (n)->prev = (h)->prev;         \
    (n)->prev->next = n;           \
    (n)->next = q;                 \
    (h)->prev = (q)->prev;         \
    (h)->prev->next = h;           \
    (q)->prev = n;

#define myptm_queue_add(h, n)    \
    (h)->prev->next = (n)->next; \
    (n)->next->prev = (h)->prev; \
    (h)->prev = (n)->prev;       \
    (h)->prev->next = h;

#define myptm_queue_move(h, _new)         \
    do                                    \
    {                                     \
        myptm_queue_insert_tail(h, _new); \
        myptm_queue_remove(h);            \
    } while (0)

#ifndef myptm_container_of
#define myptm_container_of(p, t, m) \
    ((t *)((char *)p - (char *)(&(((t *)0)->m))))
#endif

#define myptm_queue_container(q, type, m) myptm_container_of((q), type, m)

#define myptm_queue_for_each_container(pos, head, t, member)   \
    for (pos = myptm_queue_container((head)->next, t, member); \
         &pos->member != (head);                               \
         pos = myptm_queue_container(pos->member.next, t, member))

#define myptm_queue_for_each_container_safe(pos, n, head, t, member) \
    for (pos = myptm_queue_container((head)->next, t, member),       \
        n = myptm_queue_container(pos->member.next, t, member);      \
         &pos->member != (head);                                     \
         pos = n, n = myptm_queue_container(n->member.next, t, member))

#ifdef __cplusplus
}
#endif

#endif
