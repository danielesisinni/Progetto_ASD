#include <stdlib.h>
#include <stdio.h>
#include "generic_array.h"

//Initial capacity for the array
#define INITIAL_CAPACITY 2

/*constant used to choose between mergeSort or insertionSort algorithms 
 *If current array size is equal or less than k_value, we will use insertionSort, otherwise we will use mergeSort
*/
unsigned long k_value = 35;

//It represents the internal structure of this implementation of generic arrays
struct _GenericArray {
  void **array;
  unsigned long size;
  unsigned long array_capacity;
  int (*compare)(void*, void*);
};

GenericArray *generic_array_create(int (*compare)(void*, void*)) {
  if (compare == NULL) {
    fprintf(stderr, "generic_array_create: precedes parameter cannot be NULL");
    exit(EXIT_FAILURE);
  }
  GenericArray *generic_array = (GenericArray*)malloc(sizeof(GenericArray));
  if (generic_array == NULL) {
    fprintf(stderr, "generic_array_create: unable to allocate memory for the generic array");
    exit(EXIT_FAILURE);
  }
  generic_array->array = (void**)malloc(INITIAL_CAPACITY * sizeof(void*));
  if (generic_array->array == NULL) {
    fprintf(stderr, "generic_array_create: unable to allocate memory for the internal array");
    exit(EXIT_FAILURE);
  }
  generic_array->size = 0;
  generic_array->array_capacity = INITIAL_CAPACITY;
  generic_array->compare = compare;
  return generic_array;
}

int generic_array_is_empty(GenericArray *generic_array) {
  if (generic_array == NULL) {
    fprintf(stderr, "generic_array_is_empty: generic_array parameter cannot be NULL");
    exit(EXIT_FAILURE);
  }
  return generic_array->size == 0;
}

unsigned long generic_array_size(GenericArray *generic_array) {
  if (generic_array == NULL) {
    fprintf(stderr, "generic_array_size: generic_array parameter cannot be NULL");
    exit(EXIT_FAILURE);
  }
  return generic_array->size;
}

void generic_array_add(GenericArray *generic_array, void *element) {
  if (generic_array == NULL) {
    fprintf(stderr, "add_generic_array_element: generic_array parameter cannot be NULL");
    exit(EXIT_FAILURE);
  }
  if (element == NULL) {
    fprintf(stderr, "add_generic_array_element: element parameter cannot be NULL");
    exit(EXIT_FAILURE);
  }

  if (generic_array->size >= generic_array->array_capacity) {
    generic_array->array_capacity = 2 * generic_array->array_capacity;
    generic_array->array = (void**)realloc(generic_array->array, generic_array->array_capacity * sizeof(void*));
    if (generic_array->array == NULL) {
      fprintf(stderr,"generic_array_add: unable to reallocate memory to host the new element");
      exit(EXIT_FAILURE);
    }
  }
  generic_array->array[generic_array->size] = element;
  generic_array->size++;
}

void *generic_array_get(GenericArray *generic_array, unsigned long i) {
  if (generic_array == NULL) {
    fprintf(stderr, "generic_array_get: generic_array parameter cannot be NULL");
    exit(EXIT_FAILURE);
  }
  if (i >= generic_array->size) {
    fprintf(stderr, "generic_array_get: Index %lu is out of the array bounds", i);
    exit(EXIT_FAILURE);
  }
  return generic_array->array[i];
}

void generic_array_free_memory(GenericArray *generic_array) {
  if (generic_array == NULL) {
    fprintf(stderr, "generic_array_free_memory: generic_array parameter cannot be NULL");
    exit(EXIT_FAILURE);
  }
  free(generic_array->array);
  free(generic_array);
}

void merge(GenericArray *generic_array, int (*compare)(void*, void*), unsigned long low, unsigned long mid, unsigned long high){
    unsigned long k = 0, i = low, j = mid + 1;
    GenericArray *tmp_array = generic_array_create(compare);

    while (i <= mid && j <= high){
        if(generic_array->compare(generic_array->array[i], generic_array->array[j])){
            generic_array_add(tmp_array, generic_array->array[i]);
            i++;
        }else{
            generic_array_add(tmp_array, generic_array->array[j]);
            j++;
        }
    }
    
    while(i <= mid){
        generic_array_add(tmp_array, generic_array->array[i]);
        i++;
    }
    
    while(j <= high){
        generic_array_add(tmp_array, generic_array->array[j]);
        j++;
    }
  
    k = low;
    while(k <= high){
        generic_array->array[k] = tmp_array->array[k-low];
        k++;
    }
    generic_array_free_memory(tmp_array);
}

long binarySearch(GenericArray *generic_array, void *item, long low, long high) {

    if (high <= low) {
        if (generic_array->compare(generic_array->array[low], item)){
            return low + 1;
        } else {
            return low;
        }
    }
    long mid = (low + high)/2;
    if (generic_array->compare(generic_array->array[mid], item)){
        return binarySearch(generic_array, item, mid + 1, high);
    }
    return binarySearch(generic_array, item, low, mid - 1);  
}

void sorting_algorithm(GenericArray *generic_array, int (*compare)(void*, void*), unsigned long low, unsigned long high){

    if (high - low + 1 <= k_value){
        //InsertionSort recursive implementation with binary search
        for (unsigned long i = low + 1; i  < high + 1; i++){
            long j = (long)i - 1;
            //We need to store a generic element
            GenericArray *tmp_array = generic_array_create(compare); 
            tmp_array->array[0] = generic_array->array[i];

            long index = binarySearch(generic_array, tmp_array->array[0], (long)low, j);

            while (j >= index){
                generic_array->array[j+1] = generic_array->array[j];  
                j--;
            }
            generic_array->array[index] = tmp_array->array[0]; 
            generic_array_free_memory(tmp_array);
        }
    }else{
        unsigned long mid;
        if (low < high) {
            mid = (low+high)/2;
            sorting_algorithm(generic_array, compare, low, mid);
            sorting_algorithm(generic_array, compare, mid + 1, high);
            merge(generic_array, compare, low, mid, high);
        }
    }
}

void set_k_value(const char *k_value_char){
  k_value = strtoul(k_value_char,NULL,10);
}
