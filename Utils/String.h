#ifndef STRING_H /* just little different to bs tree */
#define STRING_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <malloc.h>
#include <stdlib.h>

#include "null.h"
    ;
    typedef struct _sstr XString;

    ;
    struct _sstr
    {
        char *_str;
        int _size;
    };

#define sstrcreate()                   \
    {                                  \
        ._str = malloc(24), ._size = 0 \
    }

#define sstrcreate()                   \
    {                                  \
        ._str = malloc(24), ._size = 0 \
    }

#define sstrcreateft(t) ({           \
    XString str = sstrcreate();      \
    sstrappends((XString *)&str, t); \
    str;                             \
})

#define sstrappend(str, ch)                                                 \
    {                                                                       \
        if ((str._size) == malloc_usable_size(str._str))                    \
            str._str = realloc(str._str, malloc_usable_size(str._str) * 2); \
        *(str._str + (str._size++)) = ch;                                   \
    }

#define sstrserialize(str) ({sstrappend(str, '\00'); str._str; })

#define sstrgrow(str)                                \
    if ((str._size) == malloc_usable_size(str._str)) \
        str._str = realloc(str._str, malloc_usable_size(str._str) * 2);

    __attribute__((__nonnull__(1, 2))) extern void sstrappends(XString *str, char *stra);
    extern char *strcreaterandomlowercase(int lengtha, int lengthb);
    extern char *strcreaterandom(int lengtha, int lengthb);
    __attribute__((__nonnull__(1, 2))) extern int strstartswith(char *str, char *start);
    __attribute__((__nonnull__(1))) extern int strend(char *str);
    __attribute__((__nonnull__(1))) extern unsigned long long strhash(char *str);
    __attribute__((__nonnull__(1, 2))) extern int strendswith(char *str, char *end);
    __attribute__((__nonnull__(1))) extern signed int strequalsmo(char *str, char opts, ...);
    __attribute__((__nonnull__(1, 2))) extern int strequals(char *str1, char *str2);
    __attribute__((__nonnull__(1, 2))) extern char *strremoveAdStart(char *str, char *start);
    /// @brief concats <str1> and <str2>
    /// @return result, allocated using malloc
    __attribute__((__nonnull__(1, 2))) extern char *strccat(char *str1, char *str2);
    __attribute__((__nonnull__(1, 3))) extern char *strccatph(char *str1, char ph, char *str2);
    /// @brief returns a string splited before <i> strsubbf("hello.world", "."); will return "hello"
    /// @return result, allocated using malloc
    __attribute__((__nonnull__(1, 2))) extern char *strsubbf(char *str1, char *i);
    __attribute__((__nonnull__(1))) extern char *strremovecAdStart(char *str, char ch);
    /// @brief searches for character sequence <i> in <str1>
    /// returns substring abfter the searched sequence or null if there was no occurance
    __attribute__((__nonnull__(1, 2))) extern char *strsearch(char *str1, char *i);
    __attribute__((__nonnull__(1, 2))) extern char *strsearchbf(char *str1, char *i);
#ifdef __cplusplus
}
#endif
#endif