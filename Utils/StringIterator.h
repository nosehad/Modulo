#ifndef STRING_ITERATOR_H
#define STRING_ITERATOR_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "../Utils/String.h"
#include "../Utils/null.h"

    ;
    struct _StringIterator
    {
        char *init;

        char *bound;
        char *str;
        unsigned int line;
    };
    typedef struct _StringIterator StringIterator;

#define ITR_MATCH 1
#define ITR_NO_MATCH 0

#define itr_create(_str) \
    (struct _StringIterator){.bound = _str, .str = _str, .line = 1, .init = _str};

    /* returns current column of <iterator> */
    extern unsigned int itr_getcolumn(StringIterator *iterator);
    /* returns current line of <iterator> */
    extern unsigned int itr_getline(StringIterator *iterator);
    /* returns the text of the current line of <iterator> */
    extern char *itr_getlinestr(StringIterator *iterator);

    /* returns string pointer where the iterator currently is at */
    extern char *itr_state(struct _StringIterator *iterator);
    /* returns ITR_MATCH if the text was successfully parsed */
    extern char *itr_gettext(struct _StringIterator *iterator);
    /* returns ITR_MATCH if the string was successfully parsed */
    extern char *itr_getstr(struct _StringIterator *iterator);
    /* returns ITR_MATCH if the integer was successfully parsed */
    extern int itr_getveryshort(struct _StringIterator *iterator, signed char *itr_buff);
    /* returns ITR_MATCH if the integer was successfully parsed */
    extern int itr_getshort(struct _StringIterator *iterator, signed short *itr_buff);
    /* returns ITR_MATCH if the integer was successfully parsed */
    extern int itr_getint(struct _StringIterator *iterator, signed int *itr_buff);
    /* returns ITR_MATCH if the integer was successfully parsed */
    extern int itr_getlong(struct _StringIterator *iterator, signed long long *itr_buff);

    /* returns ITR_MATCH if the integer was successfully parsed */
    extern int itr_getuveryshort(struct _StringIterator *iterator, unsigned char *itr_buff);
    /* returns ITR_MATCH if the integer was successfully parsed */
    extern int itr_getushort(struct _StringIterator *iterator, unsigned short *itr_buff);
    /* returns ITR_MATCH if the integer was successfully parsed */
    extern int itr_getuint(struct _StringIterator *iterator, unsigned int *itr_buff);
    /* returns ITR_MATCH if the integer was successfully parsed */
    extern int itr_getulong(struct _StringIterator *iterator, unsigned long long *itr_buff);
    /* returns the error message */
    extern char *itr_geterr();
    /* returns ITR_MATCH if the double was successfully parsed */
    extern int itr_getdouble(struct _StringIterator *iterator, double *itr_buff);
    /* returns ITR_MATCH if <ch> matches the next relevant char of the iterated string */
    extern int itr_char(StringIterator *iterator, char ch);
    /* calls the function for the matching function */
    extern void itr_text(struct _StringIterator *iterator, void *itr_nomatch, unsigned int options, ...);
    /* returns the matching lable address */
    extern void *itr_jtext(struct _StringIterator *iterator, void *itr_nomatch, unsigned int options, ...);
    /* searches for a character sequence <section> in not yet iterated text */
    extern int itr_search(StringIterator *iterator, char *section);
    /* searches for a character sequence <section> in not yet iterated text */
    extern int itr_searchbf(StringIterator *iterator, char *section);

#ifdef __cplusplus
}
#endif
#endif