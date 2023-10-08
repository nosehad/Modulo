#include "StringIterator.h"

// TODO: Optimize __skipcharactersandcomments__

/* define integer constants */
#define DEC_CONST_UCHAR 0x18                  /* math.floor(2^8 / 10) - 1  */
#define DEC_CONST_USHORT 0x1998               /* math.floor(2^16 / 10) - 1 */
#define DEC_CONST_UINT 0x19999998             /* math.floor(2^32 / 10) - 1 */
#define DEC_CONST_ULONG 0x1999999999999998ULL /* math.floor(2^64 / 10) - 1 */

#define DEC_CONST_CHAR 0xB                 /* math.floor(2^7 / 10) - 1  */
#define DEC_CONST_SHORT 0xCCB              /* math.floor(2^15 / 10) - 1 */
#define DEC_CONST_INT 0xCCCCCCB            /* math.floor(2^31 / 10) - 1 */
#define DEC_CONST_LONG 0xCCCCCCCCCCCCCCBLL /* math.floor(2^63 / 10) - 1 */

struct
{
    char *err;
    /* used to store the ptr of where the error was detected */
    char *err_source_end;
} INFO;

char *itr_geterr()
{
    return INFO.err;
}

/* define as inline and not as macro, since performance is not that important when getting error messages */
unsigned int itr_getcolumn(StringIterator *iterator)
{
    char *str = iterator->bound;
    unsigned int column = 1;
    for (; str != iterator->init && *(str - 1) != '\n'; ++column, --str)
        ;
    return column;
}

unsigned int itr_getline(StringIterator *iterator)
{
    return iterator->line;
}

#define __str__ (iterator->str)
#define __line__ (iterator->line)

static inline __attribute__((__always_inline__)) void itr_skipcomments(struct _StringIterator *iterator)
{
    for (;; ++__str__)
    {
        switch (*__str__)
        {
        case ' ':
            break;
        case '\t':
            break;
        case '\n':
            ++__line__;
            break;
        case ';':
            if (*(__str__++ + 1) == ';')
            {
                for (; *__str__ != '\00'; ++__str__)
                    if (*__str__ == ';' && *(__str__ + 1) == ';')
                        break;
                    else if (*__str__ == '\n')
                    {
                        ++__line__;
                    }
            }
            else
            {
                for (++__line__; *__str__ != '\n' && *__str__ != '\00'; ++__str__)
                    ;
            }
            break;
        default:
            return;
        }
    }
}

char *itr_state(struct _StringIterator *iterator)
{
    return iterator->str;
}

/* if an request doesn't match the state is being poped to the bound */
#define popandreturn(errmsg)                 \
    {                                        \
        INFO.err = errmsg;                   \
        INFO.err_source_end = iterator->str; \
        iterator->str = iterator->bound;     \
        return ITR_NO_MATCH;                 \
    }

/* if an request matches the bound is being pushed to iterator->str*/
#define pushandreturn(_str_)             \
    {                                    \
        iterator->bound = iterator->str; \
        itrstrappend(_str_, '\00');      \
        return _str_._str;               \
    }

#define itrstr      \
    struct          \
    {               \
        char *_str; \
        int _size;  \
    }

#define itrstrcreate()                 \
    {                                  \
        ._str = malloc(24), ._size = 0 \
    }

    #define itrstrappend(str, ch)                                               \
    {                                                                       \
        if ((str._size) == malloc_usable_size(str._str))                    \
            str._str = realloc(str._str, malloc_usable_size(str._str) * 2); \
        *(str._str + (str._size++)) = ch;                                   \
    }

#define itrstrappends(str, stra) \
    for(;*stra != '\00'; ++stra) \
        itrstrappend(str, *stra) 


#define itrstrgrow(str)                              \
    if ((str._size) == malloc_usable_size(str._str)) \
        str._str = realloc(str._str, malloc_usable_size(str._str) * 2);

/* ignore unused result of realloc */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"

char *itr_getlinestr(StringIterator *iterator)
{
    char *str = iterator->bound;
    for (; str != iterator->init && *(str - 1) != '\n'; --str)
        ;
    itrstr line = itrstrcreate();
    for (; *str != '\n' && *str != '\00'; ++str)
        itrstrappend(line, *str);
    itrstrappend(line, '\00');
    return line._str;
}

/* deprecated character skip */
/*
skip:
    for (; *__str__ == ' ' || *__str__ == '\t'; ++__str__)
        ;
    if (*__str__ == '\n')
    {
        ++__line__, ++__str__;
        goto skip;
    }
*/

char *itr_gettext(struct _StringIterator *iterator)
{
/* skip useless characters */
skip:
    for (; *__str__ == ' ' || *__str__ == '\t'; ++__str__)
        ;
    if (*__str__ == '\n')
    {
        ++__line__, ++__str__;
        goto skip;
    }
    /* check if string is valid */
    if (!((96 < *__str__ && *__str__ < 123) || (64 < *__str__ && *__str__ < 91) || (*__str__ == '_'))) /* cannot start with a number */
        popandreturn("invalid literal");
    /* copy string to new str buff */
    itrstr str = itrstrcreate();
    for (;
         (96 < *__str__ && *__str__ < 123) || (64 < *__str__ && *__str__ < 91) /* letters */
         || (*__str__ == '_')                                                  /* underscores */
         || (*__str__ > ('0' - 1) && *__str__ < ('9' + 1))                     /* digits */
         ;
         ++__str__)
    {
        if (*__str__ == null)
            pushandreturn(str); /* doesn't need an enclosure */
        itrstrappend(str, *__str__);
    }
    pushandreturn(str);
}

char * itr_getstr(struct _StringIterator *iterator)
{
/* skip useless characters */
skip:
    for (; *__str__ == ' ' || *__str__ == '\t'; ++__str__)
        ;
    if (*__str__ == '\n')
    {
        ++__line__, ++__str__;
        goto skip;
    }
    /* check if string is valid */
    if (*(__str__++) != '"')
        popandreturn("invalid string");
    /* copy string to new str buff */
    itrstr str = itrstrcreate();
    for (;;)
    {
        switch (*__str__)
        {
        case '\000':
            popandreturn("expected string enclosure (\")");
            break;
        case '"':
            __str__++;
            pushandreturn(str);
        case '\\':
            if (*((__str__++) + 1) == '\\')
            {
                ++__str__;
                itrstrappend(str, '\\');
                break;
            };
            itrstrgrow(str);
            if(!itr_getuveryshort(iterator, (str._str + (str._size++))))
            {
                popandreturn("invalid character format code");
            }
            break;
        default:
            itrstrappend(str, *__str__);
            ++__str__;
        }
    }
}

#pragma GCC diagnostic pop

#undef itrstr
#undef itrstrcreate
#undef itrstrappend
#undef pushandreturn

#define pushandreturn()                  \
    {                                    \
        iterator->bound = iterator->str; \
        return ITR_MATCH;                \
    }

int itr_getveryshort(struct _StringIterator *iterator, signed char *itr_buff)
{
/* skip useless characters */
skip:
    for (; *__str__ == ' ' || *__str__ == '\t'; ++__str__)
        ;
    if (*__str__ == '\n')
    {
        ++__line__, ++__str__;
        goto skip;
    }
    /* check if short is negative */
    char signed n = 1;
    if (*__str__ == '-')
        n = -1, ++__str__;
    register signed char s = 0;
    /* check if string contains valid decimal characters */
    if (*__str__ < '0' || *__str__ > '9')
        popandreturn("not an valid numer"); /* if not -> return 0 */
    for (; *__str__ > ('0' - 1) && *__str__ < ('9' + 1); ++__str__)
    {
        if (s > DEC_CONST_CHAR)
            popandreturn("very short constant to large");
        s = s * 10 + *__str__ - '0';
    }
    *itr_buff = s * n;
    pushandreturn();
}

int itr_getshort(struct _StringIterator *iterator, signed short *itr_buff)
{
/* skip useless characters */
skip:
    for (; *__str__ == ' ' || *__str__ == '\t'; ++__str__)
        ;
    if (*__str__ == '\n')
    {
        ++__line__, ++__str__;
        goto skip;
    }
    /* check if short is negative */
    signed short n = 1;
    if (*__str__ == '-')
        n = -1, ++__str__;
    register signed short s = 0;
    /* check if string contains valid decimal characters */
    if (*__str__ < '0' || *__str__ > '9')
        popandreturn("not an valid numer"); /* if not -> return 0 */
    for (; *__str__ > ('0' - 1) && *__str__ < ('9' + 1); ++__str__)
    {
        if (s > DEC_CONST_SHORT)
            popandreturn("short constant to large");
        s = s * 10 + *__str__ - '0';
    }
    *itr_buff = s * n;
    pushandreturn();
}

int itr_getint(struct _StringIterator *iterator, signed int *itr_buff)
{
/* skip useless characters */
skip:
    for (; *__str__ == ' ' || *__str__ == '\t'; ++__str__)
        ;
    if (*__str__ == '\n')
    {
        ++__line__, ++__str__;
        goto skip;
    }
    /* check if int is negative */
    signed int n = 1;
    if (*__str__ == '-')
        n = -1, ++__str__;
    register signed int i = 0;
    /* check if string contains valid decimal characters */
    if (*__str__ < '0' || *__str__ > '9')
        popandreturn("not a valid number"); /* if not -> return 0 */
    for (; *__str__ > ('0' - 1) && *__str__ < ('9' + 1); ++__str__)
    {
        if (i > DEC_CONST_INT)
            popandreturn("integer constant to large");
        i = i * 10 + *__str__ - '0';
    }
    *itr_buff = i * n;
    pushandreturn();
}

int itr_getlong(struct _StringIterator *iterator, signed long long *itr_buff)
{
/* skip useless characters */
skip:
    for (; *__str__ == ' ' || *__str__ == '\t'; ++__str__)
        ;
    if (*__str__ == '\n')
    {
        ++__line__, ++__str__;
        goto skip;
    }
    /* check if long is negative */
    signed long long n = 1;
    if (*__str__ == '-')
        n = -1, ++__str__;
    register signed long long l = 0;
    /* check if string contains valid decimal characters */
    if (*__str__ < '0' || *__str__ > '9')
        popandreturn("not a valid number"); /* if not -> return 0 */
    for (; *__str__ > ('0' - 1) && *__str__ < ('9' + 1); ++__str__)
    {
        if (l > DEC_CONST_LONG)
            popandreturn("long constant to large");
        l = l * 10 + *__str__ - '0';
        /* check for '_' if long is seperated like 1_234_567L */
        if (*(__str__ + 1) == '_')
            __str__++;
    }
    /* check for suffix */
    if (*__str__ != 'l' && *__str__ != 'L')
        popandreturn("missing suffix for 64 Bit integers ('L')");
    *itr_buff = l * n;
    pushandreturn();
}

int itr_getuveryshort(struct _StringIterator *iterator, unsigned char *itr_buff)
{
/* skip useless characters */
skip:
    for (; *__str__ == ' ' || *__str__ == '\t'; ++__str__)
        ;
    if (*__str__ == '\n')
    {
        ++__line__, ++__str__;
        goto skip;
    }
    /* check if short is negative */
    if (*__str__ == '-')
        popandreturn("unsigned type has a sign");
    register unsigned char s = 0;
    /* check if string contains valid decimal characters */
    if (*__str__ < '0' || *__str__ > '9')
        popandreturn("not a valid number"); /* if not -> return 0 */
    for (; *__str__ > ('0' - 1) && *__str__ < ('9' + 1); ++__str__)
    {
        if (s > DEC_CONST_UCHAR)
            popandreturn("unsigned very short constant to large");
        s = s * 10 + *__str__ - '0';
    }
    *itr_buff = s;
    pushandreturn();
}

int itr_getushort(struct _StringIterator *iterator, unsigned short *itr_buff)
{
/* skip useless characters */
skip:
    for (; *__str__ == ' ' || *__str__ == '\t'; ++__str__)
        ;
    if (*__str__ == '\n')
    {
        ++__line__, ++__str__;
        goto skip;
    }
    /* check if short is negative */
    if (*__str__ == '-')
        popandreturn("unsigned type has a sign");
    register unsigned short s = 0;
    /* check if string contains valid decimal characters */
    if (*__str__ < '0' || *__str__ > '9')
        popandreturn("not a valid number"); /* if not -> return 0 */
    for (; *__str__ > ('0' - 1) && *__str__ < ('9' + 1); ++__str__)
    {
        if (s > DEC_CONST_USHORT)
            popandreturn("unsigned short constant to large");
        s = s * 10 + *__str__ - '0';
    }
    *itr_buff = s;
    pushandreturn();
}

int itr_getuint(struct _StringIterator *iterator, unsigned int *itr_buff)
{
/* skip useless characters */
skip:
    for (; *__str__ == ' ' || *__str__ == '\t'; ++__str__)
        ;
    if (*__str__ == '\n')
    {
        ++__line__, ++__str__;
        goto skip;
    }
    /* check if int is negative */
    if (*__str__ == '-')
        popandreturn("unsigned type has a sign");
    register unsigned int i = 0;
    /* check if string contains valid decimal characters */
    if (*__str__ < '0' || *__str__ > '9')
        popandreturn("not a valid number"); /* if not -> return 0 */
    for (; *__str__ > ('0' - 1) && *__str__ < ('9' + 1); ++__str__)
    {
        if (i > DEC_CONST_UINT)
            popandreturn("unsigned integer constant to large");
        i = i * 10 + *__str__ - '0';
    }
    *itr_buff = i;
    pushandreturn();
}

int itr_getulong(struct _StringIterator *iterator, unsigned long long *itr_buff)
{
/* skip useless characters */
skip:
    for (; *__str__ == ' ' || *__str__ == '\t'; ++__str__)
        ;
    if (*__str__ == '\n')
    {
        ++__line__, ++__str__;
        goto skip;
    }
    /* check if long is negative */
    if (*__str__ == '-')
        popandreturn("unsigned type has a sign");
    register unsigned long long l = 0;
    /* check if string contains valid decimal characters */
    if (*__str__ < '0' || *__str__ > '9')
        popandreturn("not a valid number"); /* if not -> return 0 */
    for (; *__str__ > ('0' - 1) && *__str__ < ('9' + 1); ++__str__)
    {
        if (l > DEC_CONST_ULONG)
            popandreturn("unsigned long constant to large");
        l = l * 10 + *__str__ - '0';
        if (*(__str__ + 1) == '_')
            __str__++;
    }
    /* check for suffix */
    if (*__str__ != 'u' && *__str__ != 'U')
        popandreturn("missing suffix for unsigned 64 Bit integers ('U')");
    *itr_buff = l, __str__++;
    pushandreturn();
}

// jmp_buf getdouble;
//__attribute__((__returns_twice__))
int itr_getdouble(struct _StringIterator *iterator, double *itr_buff)
{
/* skip useless characters */
skip:
    for (; *__str__ == ' ' || *__str__ == '\t'; ++__str__)
        ;
    if (*__str__ == '\n')
    {
        ++__line__, ++__str__;
        goto skip;
    }
    /* check if double is negative */
    double n = 1;
    if (*__str__ == '-')
        n = -1, ++__str__;
    register double d = 0;
    /* check if string contains valid decimal characters */
    if (*__str__ < '0' || *__str__ > '9')
        popandreturn("not a valid number"); /* if not -> return 0 */
    /* parse integer before point */
    for (; *__str__ > ('0' - 1) && *__str__ < ('9' + 1); ++__str__)
        d = d * 10 + *__str__ - '0';
    if (*(__str__++) != '.')
        if (*(__str__ - 1) == 'F' || *(__str__ - 1) == 'f')
        {
            *itr_buff = __builtin_copysignq(d, n);
            pushandreturn();
        }
        else
            popandreturn("missing suffix for doubles ('D')");
    /* check if string contains valid decimal characters */
    if (*__str__ < '0' || *__str__ > '9')
        popandreturn("not a valid double"); /* if not -> return 0 */
    /* parse integer after point */
    register double a = 1, b = 0;
    for (; *__str__ > ('0' - 1) && *__str__ < ('9' + 1); ++__str__)
        b = b * 10 + *__str__ - '0', a *= 10;
    if (*__str__ == 'f' || *__str__ == 'F') /* check for suffix */
        ++__str__;
    *itr_buff = __builtin_copysignq((d /* integer before point */ + (b / a) /* integer after point */), n);
    pushandreturn();
}

#undef popandreturn
#undef pushandreturn

/* undef integer constants */
#undef DEC_CONST_UCHAR
#undef DEC_CONST_USHORT
#undef DEC_CONST_UINT
#undef DEC_CONST_ULONG

#undef DEC_CONST_CHAR
#undef DEC_CONST_SHORT
#undef DEC_CONST_INT
#undef DEC_CONST_LONG

//__attribute__((__noinline__))
// jmp_buf*itr_getdoublebuf()
//{
//    longjmp(getdouble, 1);
//}

void itr_text(struct _StringIterator *iterator, void *itr_nomatch, unsigned int options, ...)
{
    itr_skipcomments(iterator);
    __builtin_va_list arguments;
    __builtin_va_start(arguments, options);
    iterator->bound = iterator->str;
    for (unsigned int i = 0; i < options; ++i)
    {
        char *text = __builtin_va_arg(arguments, char *);
        __str__ = iterator->bound;
        for (; *text != null; ++text, ++__str__)
            if (*__str__ != *text)
                goto _continue;
            else if (*__str__ == null)
                goto _continue;
        __builtin_va_end(arguments);
        iterator->bound = __str__;
        ((void (*)()) __builtin_va_arg(arguments, void *))();
        return;
    _continue:
        __builtin_va_arg(arguments, void *);
        continue;
    }
    __builtin_va_end(arguments);
    ((void (*)())itr_nomatch)();
    __str__ = iterator->bound;
    return;
}

void *itr_jtext(struct _StringIterator *iterator, void *itr_nomatch, unsigned int options, ...)
{
    /* skip useless characters */
    itr_skipcomments(iterator);
    __builtin_va_list arguments;
    __builtin_va_start(arguments, options);
    iterator->bound = __str__;
    for (unsigned int i = 0; i < options; ++i)
    {
        char *text = __builtin_va_arg(arguments, char *);
        __str__ = iterator->bound;
        for (; *text != null; ++text, ++__str__)
            if (*__str__ != *text)
                goto _continue;
            else if (*__str__ == null)
                goto _continue;
        __builtin_va_end(arguments);
        //__str__--;
        iterator->bound = __str__;
        return __builtin_va_arg(arguments, void *);
    _continue:
        __builtin_va_arg(arguments, void *);
        continue;
    }
    __builtin_va_end(arguments);
    __str__ = iterator->bound;
    return itr_nomatch;
}

int itr_char(StringIterator *iterator, char ch)
{
/* skip useless characters */
skip:
    for (; *__str__ == ' ' || *__str__ == '\t'; ++__str__)
        ;
    if (*__str__ == '\n')
    {
        ++__line__, ++__str__;
        goto skip;
    }
    if (*__str__ == ch)
    {
        ++__str__;
        return ITR_MATCH;
    }
    else
    {
        return ITR_NO_MATCH;
    }
}

int itr_search(StringIterator * iterator, char* section)
{
    if((iterator->str = strsearch(iterator->str, section)) == null)
    {
        iterator->str = iterator->bound;
        return ITR_NO_MATCH;
    }
    iterator->bound = iterator->str;
    return ITR_MATCH;
}

int itr_searchbf(StringIterator * iterator, char* section)
{
    if((iterator->str = strsearchbf(iterator->str, section)) == null)
    {
        iterator->str = iterator->bound;
        return ITR_NO_MATCH;
    }
    iterator->bound = iterator->str;
    return ITR_MATCH;
}


#undef __str__