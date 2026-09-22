/*
 * Copyright (C) 2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/core/str.h"
#include "lexbor/core/print.h"


#define lexbor_sprintf_append(dst, end, src, length)                          \
    do {                                                                      \
        if ((size_t) ((end) - (dst)) < (length)) {                            \
            return (end) - (dst);                                             \
        }                                                                     \
                                                                              \
        memcpy((dst), (src), (length));                                       \
        (dst) += (length);                                                    \
    }                                                                         \
    while (false)


size_t
lexbor_printf_size(const char *format, ...)
{
    size_t ret;
    va_list va;

    va_start(va, format);
    ret = lexbor_vprintf_size(format, va);
    va_end(va);

    return ret;
}

size_t
lexbor_vprintf_size(const char *format, va_list va)
{
    char c;
    const char *begin, *cdata;
    size_t size;
    lexbor_str_t *str;

    begin = format;
    size = 0;

    while (true) {
        c = *format;

        if (c == '%') {
            c = format[1];

            switch (c) {
                case '\0':
                    return size + (format - begin) + 1;

                case 's':
                    cdata = va_arg(va, const char *);
                    size += (format - begin) + strlen(cdata);
                    break;

                case 'S':
                    str = va_arg(va, lexbor_str_t *);
                    size += (format - begin) + str->length;
                    break;

                case '%':
                    size += (format - begin) + 1;
                    break;

                default:
                    return LXB_PRINT_ERROR;
            }

            format++;
            begin = format + 1;
        }
        else if (c == '\0') {
            return size + (format - begin);
        }

        format++;
    }
}

size_t
lexbor_sprintf(lxb_char_t *dst, size_t size, const char *format, ...)
{
    size_t ret;
    va_list va;

    va_start(va, format);
    ret = lexbor_vsprintf(dst, size, format, va);
    va_end(va);

    return ret;
}

size_t
lexbor_vsprintf(lxb_char_t *dst, size_t size, const char *format, va_list va)
{
    char c;
    const char *begin, *cdata;
    lxb_char_t *end, *start;
    lexbor_str_t *str;

    begin = format;
    start = dst;
    end = dst + size;

    while (true) {
        c = *format;

        if (c == '%') {
            c = format[1];

            switch (c) {
                case '\0':
                    size = (format - begin) + 1;
                    lexbor_sprintf_append(dst, end, begin, size);
                    goto done;

                case 's':
                    size = format - begin;
                    lexbor_sprintf_append(dst, end, begin, size);

                    cdata = va_arg(va, const char *);
                    size = strlen(cdata);
                    lexbor_sprintf_append(dst, end, cdata, size);
                    break;

                case 'S':
                    size = format - begin;
                    lexbor_sprintf_append(dst, end, begin, size);

                    str = va_arg(va, lexbor_str_t *);
                    lexbor_sprintf_append(dst, end, str->data, str->length);
                    break;

                case '%':
                    size = (format - begin) + 1;
                    lexbor_sprintf_append(dst, end, begin, size);
                    break;

                default:
                    return LXB_PRINT_ERROR;
            }

            format++;
            begin = format + 1;
        }
        else if (c == '\0') {
            size = format - begin;
            lexbor_sprintf_append(dst, end, begin, size);
            goto done;
        }

        format++;
    }

done:

    if (end - dst > 0) {
        *dst = '\0';
    }

    return dst - start;
}
