#ifndef SHASHMAP_H
#define SHASHMAP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "String.h"

#include <malloc.h>
#include <string.h>

    typedef struct _shashmap SHashmap;
    typedef struct _pair Pair;
    typedef unsigned long long hasht;
    typedef char *K;
    typedef void *V;

    struct _shashmap
    {
        unsigned long long size;
        unsigned long long capacity;
        struct _hashnode **table;
        hasht (*_hash)(K);
    };

    struct _hashnode
    {
        void *key;
        void *value;
        hasht hash;
        struct _hashnode *next;
    };

    struct _pair
    {
        void *key;
        void *value;
    };

    /// @brief Creates new SHashmap
    /// @return new SHashmap
    extern struct _shashmap *shm_create(void);

    /// @brief Creates new SHashmap on buffer <hm>
    extern void shm_createob(SHashmap *hm);

    /// @brief Puts a new Pair into the hashmap (even if the SHashmap already contains the Pair)
    /// @param hm used SHashmap
    /// @param p used Pair
    extern void shm_putp(struct _shashmap *hm, Pair *p);

    /// @brief Direcly put a pair into the hashmap by using Key and Value (even if the SHashmap already contains the Pair)
    /// @param hm used SHashmap
    /// @param key used Key
    /// @param value associated with Key
    extern void shm_put(SHashmap *hm, K key, V value);

    /// @brief Sets the value corresponding to <key>, if a Node already exists with <key> it overrides its value with <value>
    /// @param hm used SHashmap
    /// @param key used Key
    /// @param value associated with Key
    extern void shm_set(SHashmap *hm, K key, V value);

    /// @brief Removes the Node associated with a Key from the SHashmap
    /// @param hm used SHashmap
    /// @param key used Key
    extern void shm_remove(SHashmap *hm, K key);

    /// @brief Doubles the Amount of Buckets in the SHashmap (automatically gets called when map is full)
    /// @param hm used SHashmap
    extern void shm_resize(SHashmap *hm);

    /// @brief Checks if a SHashmap contains a given Key
    /// @param hm used SHashmap
    /// @param key that is being checked
    /// @return 1 if <hm> contains <key> 0 if not
    extern int shm_contains(SHashmap *hm, K key);

    /// @brief gets Value associated with a Key
    /// @param hm used SHashmap
    /// @param key used Key
    /// @return Value associated with <key>
    extern V shm_get(struct _shashmap *hm, K key);

    /// @brief Iterates over a SHashmap
    /// @param hm used SHashmap
    /// @param _iterator used Iterator
    extern void shm_iterate(SHashmap *hm, void(_iterator)(Pair *p));

    /// @brief Delets the entire SHashmap using free function
    /// @param hm SHashmap that is being deleted
    extern void shm_delete(struct _shashmap *hm);

    /// @brief Copies a given SHashmap
    extern struct _shashmap *shm_copy(struct _shashmap *hm);

    /// @brief simple method used for testing
    extern void shm_printBuckets(SHashmap *hm);

#define _shm_fr_0(buf)
#define _shm_fr_1(buf) free(buf)

    /// @brief Simple utility macro that allows to free an Hashmap
    /// @param hm used SHashmap
    /// @param kf if set to 0 dont automatically free key to 1 if it should be freed
    /// @param vf same as <kf> just for the value pointer
#define _shm_delete_fbm(hm, kf, vf)                                         \
    {                                                                       \
        struct _hashnode **cap = (hm.table + hm.capacity);                  \
        for (struct _hashnode **bucket = hm.table; bucket != cap; ++bucket) \
        {                                                                   \
            struct _hashnode *node = *bucket;                               \
            if (node != null)                                               \
                for (struct _hashnode * next; node != null; node = next)    \
                {                                                           \
                    next = node->next;                                      \
                    _shm_fr_##kf(node->key);                                \
                    _shm_fr_##vf(node->value);                              \
                    free(node);                                             \
                }                                                           \
        }                                                                   \
        free(hm.table);                                                     \
    }

#ifdef __cplusplus
}
#endif

#endif