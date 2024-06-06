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
#define dbg(fmt, ...) printf("[%s:%d] %s = " fmt "\n", __FILE__, __LINE__, STRINGIFY(__VA_ARGS__), ##__VA_ARGS__)

#define swap(x, y)\
    do {\
        typeof(x) _swap_tmp_ = x;\
        x = y;\
        y = _swap_tmp_;\
    } while (0)

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
#define unit (unit_t){}

/// VEC

#define typedef_Vec(T, name) typedef struct { T* items; size_t len; size_t cap; } name

#define Vec_append(/* Vec(T) */ self, /* T */ x) ({\
    if ((self)->len == (self)->cap) {\
        (self)->cap = (self)->cap == 0 ? NOTSTD_MIN_CONTAINER_SIZE : (self)->cap * 2;\
        (self)->items = realloc((self)->items, sizeof((self)->items[0])*(self)->cap);\
    }\
    (self)->items[(self)->len++] = x;\
})

#define Vec_print(self, fmt)\
  do {\
      printf("[ ");\
      for (size_t i = 0; i < (self)->len; i++) {\
          printf(fmt " ", (self)->items[i]);\
      }\
      printf("]");\
  } while (0)

#define Vec_clone(self) ({\
    typeof(*self) to = { .items = NULL, .len = (self)->len, .cap = (self)->len };\
    to.items = malloc(sizeof((self)->items[0])*to.cap);\
    memcpy(to.items, (self)->items, sizeof((self)->items[0])*to.cap);\
    to;\
})

#define Vec_split(self, at, left, right) do {\
    (left)->items = (self)->items;\
    (left)->len = (at);\
    (left)->cap = (at);\
    (right)->items = (self)->items + (at);\
    (right)->len = (self)->len - (at);\
    (right)->cap = (self)->len - (at);\
} while(0);

// STACK

#define typedef_Stack(T, name) typedef_Vec(T, name)

#define Stack_push(self, x) Vec_append(self, x)
#define Stack_pop(self, x) ((self)->items[--(self)->len])

// STRING

typedef_Vec(char, String);

#define S(str) ({\
    String string = {0};\
    size_t len = strlen(str) + 1;\
    for (int i = 0; i < len; i++) {\
        Vec_append(&string, (str[i]));\
    }\
    string;\
})

// LIST

#define typedef_List(T, name)\
typedef struct CONCAT(_nonptr_, name) {\
    T head;\
    struct CONCAT(_nonptr_, name)* tail;\
} CONCAT(_nonptr_, name);\
typedef struct CONCAT(_nonptr_, name)* name;

#define List_Cons(head_, tail_) ({\
    typeof(tail_) new = malloc(sizeof(*(tail_)));\
    *new = (typeof(*(tail_))){ .head = head_, .tail = tail_ };\
    new;\
})
    
#define List_free(self)\ ({\
    typeof(self) current = (self);\
    while (current != NULL) {\
        name next = current->tail;\
        current->tail = NULL;\
        free(current);\
        current = next;\
    }\
})

#define List_match(lst, nil_case, h, t, cons_case) \
if ((lst) == NULL) {\
    nil_case\
} else {\
    typeof(lst) t = (lst)->tail;\
    typeof((lst)->head) h = (lst)->head;\
    cons_case\
}

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
    } name;

#define Queue_push(self_, x_) ({\
    typeof(self_) self = self_;\
    typeof(x_) x = x_;\
    if (self->len == self->cap) {\
        size_t new_cap = self->cap == 0 ? NOTSTD_MIN_CONTAINER_SIZE : self->cap * 2;\
        typeof(self->items) new_items = (typeof(self->items))malloc(sizeof(*self->items)*new_cap);\
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
    unit;\
})


#define Queue_pop(self_) ({\
    typeof(self_) self = self_;\
    typeof(self->items) x;\
    if (self->len == 0) {\
        x = NULL;\
    } else {\
        x = &self->items[self->head];\
        self->head++;\
        self->len--;\
    }\
    x;\
})

// HASHMAP

#endif // _NOTSTD_H_INCLUDED
