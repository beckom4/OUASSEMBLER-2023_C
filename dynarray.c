/* -------------------------------------------- */

/**
* MAMAN 14, FILE: assembler.c
* authors: Linoy daniel & Omri Beck
**/

/* -------------------------------------------- */



/* --------- INCLUDES: --------- */


#include "dynarray.h"


/* --------- FUNCTIONS IMPLEMENTATION: --------- */


/** 
* creates a dynamic array. 
*
* @param int _cap - The initial capacity.
*  	 stride - The size of the data type.
* @return - a pointer to the beginning of the new array.
* */
void *_dynarray_create(size_t init_cap, size_t stride)
{
    size_t header_size = DYNARRAY_FIELDS * sizeof(size_t);
    size_t arr_size = init_cap * stride;
    size_t *arr = (size_t *) malloc(header_size + arr_size);
    arr[CAPACITY] = init_cap;
    arr[LENGTH] = 0;
    arr[STRIDE] = stride;
    return (void *) (arr + DYNARRAY_FIELDS);
}


/** 
* Frees the memory allocated for the array.. 
*
* @param A pointer to the beginning of the array.
* */
void _dynarray_destroy(void *arr)
{
    free((char*)arr - DYNARRAY_FIELDS * sizeof(size_t));
}


/** 
* get function  to retrieve 1 of the fields in the array. 
*
* @param A pointer to the beginning of the array.
*  	 field - The index of the field in the array.
* @return - The requested field.
* */
size_t _dynarray_field_get(void *arr, size_t field)
{
    return ((size_t *)(arr) - DYNARRAY_FIELDS)[field];
}


/** 
* set function  to set 1 of the fields in the array. 
*
* @param A pointer to the beginning of the array.
*  	 field - The index of the field in the array.
* */
void _dynarray_field_set(void *arr, size_t field, size_t value)
{
    ((size_t *)(arr) - DYNARRAY_FIELDS)[field] = value;
}


/** creates a new dynamic array with twice the size of the one passed in, and retaining
 the values that the original stored. 
*
* @param A pointer to the beginning of the old array.
* @return - A pointer to the biginning of the new array.
* */
void *_dynarray_resize(void *arr)
{
    void *temp = _dynarray_create( 
        DYNARRAY_RESIZE_FACTOR * dynarray_capacity(arr),
        dynarray_stride(arr)
    );
    memcpy(temp, arr, dynarray_length(arr) * dynarray_stride(arr)); 
    _dynarray_field_set(temp, LENGTH, dynarray_length(arr)); 
    _dynarray_destroy(arr); 
    return temp;
}


/** inserts a new element into the array.
*
* @param A pointer to the beginning of the array.
*        xptr - The new element that needs to be added.
* @return - A pointer to the biginning of the array.
* */
void *_dynarray_push(void *arr, void *xptr)
{
    if (dynarray_length(arr) >= dynarray_capacity(arr))
        arr = _dynarray_resize(arr);

    memcpy((char*)arr + dynarray_length(arr) * dynarray_stride(arr), xptr, dynarray_stride(arr));
    _dynarray_field_set(arr, LENGTH, dynarray_length(arr) + 1);
    return arr;
}


/** Removes the last element in the array and copies it to dest.
*
* @param A pointer to the beginning of the array.
*        dest - The new memory destination of the removed element.
* */
void _dynarray_pop(void *arr, void *dest)
{
    memcpy(dest, (char*)arr + (dynarray_length(arr) - 1) * dynarray_stride(arr), dynarray_stride(arr));
    _dynarray_field_set(arr, LENGTH, dynarray_length(arr) - 1); 
}
