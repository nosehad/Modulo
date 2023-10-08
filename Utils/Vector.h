#ifndef VECTOR_H
#define VECTOR_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <malloc.h>

#define VECT_INIT_CAP 8

typedef struct _Vect_str Vect_str;
struct _Vect_str {
    char** _vect;
    unsigned int _size;
} 

/* vector type */
#define Vector(type)        \
    struct                  \
    {                       \
        type *_vect;        \
        unsigned int _size; \
    }

/* creates vector of <type> */
#define vect_create(type)                               \
    {                                                   \
        ._size = 0,                                     \
        ._vect = malloc(sizeof(type *) * VECT_INIT_CAP) \
    }

#define vect_set(vect, type) \
    vect._size = 0,          \
    vect._vect = malloc(sizeof(type *) * VECT_INIT_CAP)

/* adds <item> to <vect> */
#define vect_pushback(vect, item)                                             \
    if (vect._size == malloc_usable_size(vect._vect) / sizeof(item))          \
        vect._vect = realloc(vect._vect, malloc_usable_size(vect._vect) * 2); \
    *(vect._vect + (vect._size++)) = item;

/* frees vector */
#define vect_free(vect) \
    free(vect._vect)

/* returns item at given <index> */
#define vect_get(vect, index) \
    *(vect._vect + index)

#define vect_foreach(vect, type, iterator) \
    type *end = vect._vect + vect._size;   \
    for (type *iterator = vect._vect; iterator < end; ++iterator)

#ifdef __cplusplus
extern "C"
}
#endif
#endif