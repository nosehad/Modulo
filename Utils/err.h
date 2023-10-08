#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"

void throwerr(char *err, ...)
{
    __builtin_va_list list;
    __builtin_va_start(list, err);
    printf("\e[31m\e[1mfatal error:\e[0m ");
    vprintf(err, list);
    exit(0);
}

#pragma GCC diagnostic pop