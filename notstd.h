/*
 * API:
 *  macro CONCAT(x, y);
 *  macro STRINGIFY(x);
 *  macro `move`;
 *  macro `mut`;
 *  
 *  int println(char* format, ...);
 *  int dbg    (char* format, ...);
 *
 *  type unit_t;
 *  unit_t unit;
 *
 *  typedef_Vec(type T, Ident name);
 *  void Vec_append(Vec(T) *self, T x);
 *
 *  typedef_Stack(type T, Ident name);
 *  void Stack_push(Stack(T) *self, T x);
 *  T    Stack_pop (Stack(T) *self);
 *
 *  type String;
 *
 *  typedef_List(type T, Ident name);
 *  List(T) List(T)_Cons(T head, List(T) tail);
 *  void    List(T)_free(List(T) self);
 *  void    List_match(
 *      List(T) to_match, 
 *      Block nil_case,
 *      Ident head, Ident tail, Block cons_case
 *  );
 *
 *  typedef_Queue(type T, Ident name);
 *  void         Queue(T)_push(Queue(T) *self, T x);
 *  Nullable(T*) Queue(T)_pop (Queue(T) *self);
 */
#ifndef _NOTSTD_H_INCLUDED
#define _NOTSTD_H_INCLUDED

// includes
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

// macros utils

#define __CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) __CONCAT_IMPL(a, b)

#define __STRINGIFY_IMPL(x) #x
#define STRINGIFY(x) __STRINGIFY_IMPL(x)

#define println(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define dbg(fmt, ...) printf("[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

// type annotations
#define move
#define mut
#define Nullable(T) T

// notstd config

#ifndef NOTSTD_MIN_CONTAINER_SIZE
#define NOTSTD_MIN_CONTAINER_SIZE 4
#endif

// types

/// UNIT

typedef struct {} unit_t;
#define unit (unit_t){};

/// VEC

#define typedef_Vec(T, name) typedef struct { T* items; size_t len; size_t cap; } name

#define Vec_append(/* Vec(T) */ self, /* T */ x)\
    do {\
        if ((self)->len == (self)->cap) {\
            (self)->cap = (self)->cap == 0 ? NOTSTD_MIN_CONTAINER_SIZE : (self)->cap * 2;\
            (self)->items = realloc((self)->items, sizeof((self)->items[0])*(self)->cap);\
        }\
        (self)->items[(self)->len++] = x;\
    } while (0)

// STACK

#define typedef_Stack(T, name) typedef_Vec(T, name)

#define Stack_push(self, x) Vec_append(self, x)
#define Stack_pop(self, x) (self)->items[--(self)->len]

// STRING

typedef_Vec(char, String);

// LIST

#define typedef_List(T, name)\
    typedef struct CONCAT(_nonptr_, name) {\
        T head;\
        struct CONCAT(_nonptr_, name)* tail;\
    } CONCAT(_nonptr_, name);\
    typedef struct CONCAT(_nonptr_, name)* name;\
    \
    name CONCAT(name, _Cons)(T head, name tail) {\
        name new = malloc(sizeof(name));\
        *new = (struct CONCAT(_nonptr_, name)) { .head = head, .tail = tail };\
        return new;\
    }\
    \
    void CONCAT(name, _free)(name self) {\
        while (self != NULL) {\
            name next = self->tail;\
            self->tail = NULL;\
            free(self);\
            self = next;\
        }\
    }\

#define List_match(lst, nil_case, h, t, cons_case)\
    do {\
        if ((lst) == NULL) {\
            nil_case\
        } else {\
            typeof(lst) t = (lst)->tail;\
            typeof((lst)->head) h = (lst)->head;\
            cons_case\
        }\
    } while (0);


// QUEUE

// |6|7|-|-|-|-|0|1|2|3|4|5|
//      ^end    ^start      ^cap
// len = 8
// cap = 12
// start = 6
// end = 2
#define typedef_Queue(T, name)\
    typedef struct {\
        T* items;\
        size_t head;\
        size_t len;\
        size_t cap;\
    } name;\
    \
    void CONCAT(name, _push)(name* self, T x) {\
        if (self->len == self->cap) {\
            size_t new_cap = self->cap == 0 ? NOTSTD_MIN_CONTAINER_SIZE : self->cap * 2;\
            T* new_items = (T*)malloc(sizeof(T)*new_cap);\
    \
            for (size_t i = 0; i < self->len; i++) {\
                size_t j = (i+self->head) % self->cap;\
                new_items[i] = self->items[j];\
            }\
    \
            free(self->items);\
    \
            self->items = new_items;\
            self->head = 0;\
            self->cap = new_cap;\
        }\
    \
        size_t end = (self->len + self->head) % self->cap;\
        self->items[end] = x;\
        self->len++;\
    }\
    \
    T* CONCAT(name, _pop)(name* self) {\
        if (self->len == 0) return NULL;\
    \
        T *x = &self->items[self->head];\
        self->head++;\
        self->len--;\
    \
        return x;\
    }

#endif // _NOTSTD_H_INCLUDED
