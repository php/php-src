#ifndef GD_ARRAY_H
#define GD_ARRAY_H 1

typedef struct gdArrayStruct {
    unsigned int size;
    unsigned int cnt_elements;
    unsigned int element_size;
    void *elements;
} gdArray;
typedef gdArray *gdArrayPtr;

/*
C++ Vect like for C.
element_size is f.e. sizeof(gdPath) equivalent to
gdPathPtr path;
or
gdPathPtr path[12]; for a fixed length

Each function takes a pointers to the array.
*/

/* Initialize a gdArray, gdArrayarray allocation and freed is the caller's responsability  */
void gdArrayInit(gdArrayPtr array, unsigned int element_size);

/* Destroy the internal data of a gdArray, gdArrayarray has to be freed by the caller */
void gdArrayDestroy(gdArrayPtr array);

/* Growth the internal data storage to be able to store the additional requested amount (of
 * elements) */
int gdArrayReallocBy(gdArrayPtr array, unsigned int additional);

/* Reduce the elements to cnt_elements (storage will remain the same until more are
requested) */
void gdArrayTruncate(gdArrayPtr array, unsigned int cnt_elements);

/* Append an element at the end of the array */
int gdArrayAppend(gdArrayPtr array, const void *element);

/* Append cnt_elements elements to the array.
 *elements is the actual gdSpanPtr[12] for  12 elements */
int gdArrayAppendMultiple(gdArrayPtr array, const void *elements, unsigned int cnt_elements);

/* Returns the elements at the given index */
void *gdArrayIndex(gdArrayPtr array, unsigned int index);

/* Same as gdArrayIndex but immutable pointer */
const void *gdArrayIndexConst(gdArrayPtr array, unsigned int index);

/* Return the current storage size. Storage size is the available space, not the actual
amount of elements stored. Use gdArrayNumElements to know the actual stored elements count   */
unsigned int gdArraySize(const gdArrayPtr array);
unsigned int gdArrayNumElements(const gdArrayPtr array);

/* Private for internal usage in gdArray */
int gdArrayAlloc(gdArrayPtr array, unsigned int cnt_elements, void **elements);
void *gdArrayGetData(const gdArrayPtr array);
#endif /* GD_ARRAY_H */
