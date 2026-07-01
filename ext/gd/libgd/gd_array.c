#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "gd.h"
#include "gdhelpers.h"
#include "gd_color.h"
#include "gd_errors.h"
#include "gd_array.h"

void gdArrayInit(gdArrayPtr array, unsigned int element_size)
{
    array->size = 0;
    array->cnt_elements = 0;
    array->element_size = element_size;
    array->elements = NULL;
}

void gdArrayDestroy(gdArrayPtr array)
{
    gdFree(array->elements);
}

void gdArrayTruncate(gdArrayPtr array, unsigned int cnt_elements)
{
    if (cnt_elements < array->cnt_elements)
        array->cnt_elements = cnt_elements;
}

void *
gdArrayIndex(gdArrayPtr array, unsigned int index)
{
    if (index == 0 && array->cnt_elements == 0)
        return NULL;

    return (unsigned char *)array->elements + index * array->element_size;
}

const void *
gdArrayIndexConst(gdArrayPtr array, unsigned int index)
{
    if (index == 0 && array->cnt_elements == 0)
        return NULL;

    if (index >= array->cnt_elements) return NULL;

    return (const unsigned char *)array->elements + index * array->element_size;
}

int gdArrayReallocBy(gdArrayPtr array, unsigned int additional)
{
    char *new_elements;
    unsigned int old_size = array->size;
    unsigned int required_size = array->cnt_elements + additional;
    unsigned int new_size;

    /* check for integer overflow */
    if (required_size > INT_MAX || required_size < array->cnt_elements)
        return 0;

    if (required_size <= old_size)
        return 1;

    if (old_size == 0)
        new_size = 1;
    else
        new_size = old_size * 2;

    while (new_size <= required_size) {
        if (new_size > (unsigned int)INT_MAX / 2) {
            new_size = required_size;
            break;
        }
        new_size *= 2;
    }

    array->size = new_size;

    if (array->size > (unsigned int)INT_MAX
            || array->element_size > (unsigned int)INT_MAX
            || overflow2((int)array->size, (int)array->element_size)) {
        array->size = old_size;
        return 0;
    }
    new_elements = gdRealloc(array->elements,
        (size_t)array->size * array->element_size);

    if (new_elements == NULL)
    {
        array->size = old_size;
        return 0;
    }

    array->elements = new_elements;

    return 1;
}

int gdArrayAppend(gdArrayPtr array,
                  const void *element)
{
    return gdArrayAppendMultiple(array, element, 1);
}

int gdArrayAppendMultiple(gdArrayPtr array,
                          const void *elements,
                          unsigned int cnt_elements)
{
    int status;
    void *dest;

    status = gdArrayAlloc(array, cnt_elements, &dest);
    if (!status)
        return status;
    memcpy(dest, elements, (size_t)cnt_elements * array->element_size);

    return 1;
}

int gdArrayAlloc(gdArrayPtr array,
                    unsigned int cnt_elements,
                    void **elements)
{
    int status;

    status = gdArrayReallocBy(array, cnt_elements);
    if (!status)
        return status;

    *elements = (unsigned char *)array->elements
        + (size_t)array->cnt_elements * array->element_size;

    array->cnt_elements += cnt_elements;

    return 1;
}

unsigned int
gdArrayNumElements(const gdArrayPtr array)
{
    return array->cnt_elements;
}

unsigned int
gdArraySize(const gdArrayPtr array)
{
    return array->size;
}

void *
gdArrayGetData(const gdArrayPtr array)
{
    return array->elements;
}
