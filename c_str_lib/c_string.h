#pragma once
#error "APPS_UCU_ERROR: pragma is not enough. Please, add correct include guards"

#include <stdlib.h>
#include <stdint.h> // For size_t
#error "APPS_UCU_ERROR: some of the important includes are missing"
#include <stdio.h>

/* ############## ERROR codes ############### */
// Show that character or substring can not be found
#define NOT_FOUND_CODE (-1) 
// Failed to allocate memory. All functions that directly or
// indirectly (a.e. call my_str_reserve) allocate memory, can return this code
#define MEMORY_ALLOCATION_ERR (-2)
// Access to invalid index attempted.
// Should be returned by the my_str_getc, my_str_putc; insert and substr families and
// my_str_popback, when string is empty.
#define RANGE_ERR (-3)
// Error reading file. Can be returned by the "read" family functions. Note: stdin is a file too.
#define IO_READ_ERR (-4)
// Error writing file. Can be returned by the "write" family functions. Note: stdout and stderr are files too.
#define IO_WRITE_ERR (-5)
// Null pointer is given to functions which cannot deal with it:
// my_str_create, my_str_from_cstr, my_str_get_cstr
//        my_str_getc, my_str_putc,
//        my_str_copy,
//         insert, append and substr families
// my_str_reserve, my_str_shrink_to_fit, my_str_resize
//        read and write families
#define NULL_PTR_ERR (-8)
// Too small buffer size for my_str_from_cstr
#define BUFF_SIZE_ERR (-9)

// Students can define additional error codes.

typedef struct {
    size_t capacity_m; // Allocated memory block size
    size_t size_m;     // Actual size of the string
    char *data;        // Pointer to the data block
} my_str_t;

// ###########################################################
// ############### construction and destruction ##############
// ###########################################################

/*
 * Creates empty dynamic string (my_str_t).
 * Important! User should always use my_str_create before using ANY other function.
 *
 * This function plays the role of the constructor, well-known in other languages.
 *
 * Lifetime of the string begins after this function called, ends after the my_str_free called.
 * So, this function should be called once and just once before using any other string functions.
 * Once more: one should never call any other library function for the my_str_t variable
 * before calling  my_str_create.
 * 
 * str: pointer to my_str_t struct that should be initialized
 * buf_size: size of memory (in bytes) that should be allocated for the buffer
 *       It is a good idea to allocate buf_size+1 bytes -- to have a space for \0 symbol,
 *       useful fpr the optimal my_str_get_cstr() implementation.
 * Returns:
 *      0  if OK
 *      NULL_PTR_ERR if str points to NULL
 *      MEMORY_ALLOCATION_ERR if there was an error while allocating memory for the buffer
 */
int my_str_create(my_str_t* str, size_t buf_size);

/*
 * Frees buffer of the given my_str_t structure and set NULL to the corresponding pointer.
 *
 *  This function plays the role of the object destructor, well-known in other languages.
 *  Should be called exactly once, at the end of working with my_str_t.
 *
 *  Note 1: free() can be safely called with the NULL argument.
 *  Note 2: always returns 0 for uniformity and future compatible API/ABI extensions.
 *
 * Returns:
 *     0 always
 */
int my_str_free(my_str_t* str);

/*
 * Makes content of given my_str_t-string the same as given c-string.
 * Implementation should not call my_str_create!
 *
 * Note 1: function should copy the content of the c-string, pointed by the cstr to the internal buffer.
 *       Just assigning a pointer to it to the internal buffer (data) is an extremely terrible idea!
 *       (Based on typical students' failures).
 * Note 2: memory should be allocated from the heap.
 *
 * cstr: c-string from which data should be copied
 * buf_size: new size of buffer
 *      if buf_size == 0 then new buffer size = length of c-string
 *      if buf_size < size(cstr) -> return error. In this case str should not be changed!
 * 
 * Returns:
 *      0  if OK
 *      NULL_PTR_ERR if str or cstr is NULL
 *      BUFF_SIZE_ERR if buf_size is incorrect (too small)
 *      MEMORY_ALLOCATION_ERR if there occurred error while allocating memory.
 */
int my_str_from_cstr(my_str_t* str, const char* cstr, size_t buf_size);

// ###########################################################
// ########################### string information ############
// ###########################################################

/*
 * Returns actual size of my_str_t string
 * if str == NULL should return 0.
 */
size_t my_str_size(const my_str_t* str);

/*
 * Returns size of allocated memory - 1 for my_str_t
 * Note: -1 because one byte is reserved for the \0 symbol.
 * If str == NULL shoud return 0.
 */
size_t my_str_capacity(const my_str_t* str);

/*
 * Returns 1 if string is empty
 * If str == NULL should return 1 (true).
 */
int my_str_empty(const my_str_t* str);

// ###########################################################
// #################### characters access ####################
// ###########################################################

/*
 * index - obvious
 * 
 * return:
 *      symbol on given index in my_str_t-string
 *      NULL_PTR_ERR if str == NULL
 *      RANGE_ERR if index is bad
 */
int my_str_getc(const my_str_t* str, size_t index);

/*
 * puts given symbol on given position in my_str_t-string 
 * with replacing of previous one
 * 
 * return:
 *      0  if OK
 *      NULL_PTR_ERR if str or c is NULL
 *      RANGE_ERR if index is bad
 */
int my_str_putc(my_str_t* str, size_t index, char c);

/*
 * returns c-string with the same content as my_str_t-string
 * result c-string is constant 
 * and can be incorrect after changing my_str_t content
 * time complexity should be O(1)!
 * 
 * returns NULL if str is NULL
 */
const char* my_str_get_cstr(my_str_t* str);

// ###########################################################
// ################# string modifications ####################
// could cause size changing, reallocation
// if buffer is too small - this functions increase them by calling my_str_reserve
// normal multiplier for buffer size is 1.8-2
// ###########################################################

/*
 * copies content of one my_str-string to other
 * after manipulations with second string, its buffer will be like capacity of first
 * if reserve == 1 else if reserve == 0 size of first
 * return:
 *      0  if OK
 *      NULL_PTR_ERR if one of string or both are NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_copy(const my_str_t* from,  my_str_t* to, int reserve);

/*
 *  clears content of string, does nothing if string is NULL
 *  return:
 *      always 0
 */
int my_str_clear(my_str_t* str);

/*
 * inserts given char in the given position in my_str
 * if needed increases buffer
 * return:
 *      0  if OK
 *      NULL_PTR_ERR if str or c is NULL
 *      RANGE_ERR if position is bad
 *      MEMORY_ALLOCATION_ERR if occurred error during memory allocation
 */
int my_str_insert_c(my_str_t* str, char c, size_t pos);

/*
 * inserts given my_str-string into another one at given position
 * return:
 *      0  if OK
 *      NULL_PTR_ERR if str or from is NULL
 *      RANGE_ERR if position is bad
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_insert(my_str_t* str, const my_str_t* from, size_t pos);

/*
 * inserts given c-string into given my_str-string\
 * return:
 *      the same as in my_str_insert(...) function
 */
int my_str_insert_cstr(my_str_t* str, const char* from, size_t pos);

/*
 * appends one my_str-string to another
 * return:
 *      0  if OK
 *      NULL_PTR_ERR if from ot str is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_append(my_str_t* str, const my_str_t* from);


/*
 * appends c-string to my_str-string
 * return: the same as in my_str_append(...) function
 */
int my_str_append_cstr(my_str_t* str, const char* from);

/*
 * pushes given char on the end of my_str-string
 * if needed increases buffer by INC_PARAM (2)
 * return:
 *      0  if OK
 *      NULL_PTR_ERR if str or c is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during allocating memory for buffer
 */
int my_str_append_c(my_str_t* str, char c);

/*
 * saves substring of my_str-string in given bounds to given my_str-string
 * return:
 *      0  if oK
 *      NULL_PTR_ERR if from or to is NULL
 *      RANGE_ERR if boundaries are bad
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_substr(const my_str_t* from, my_str_t* to, size_t beg, size_t end);

/*
 * saves my_str-string substring in given bounds to given c-string
 * !important! given c-string should be big enough
 * return:
 *      0  if oK
 *      NULL_PTR_ERR if from or to is NULL
 *      RANGE_ERR if boundaries are bad
 */
int my_str_substr_cstr(const my_str_t* from, char* to, size_t beg, size_t end);

/*
 * return symbols with indexes [beg, end), shifts reminder to the left
 * end > size is not an error - erase all possible
 * beg > size is an error
 * return: 
 *      0 if OK
 *      RANGE_ERR if boundaries are bad
 *      MEMORY_ALLOCATION_ERROR if there was an error during memory allocation
 *      NULL_PTR_ERR if from is NULL
 */
int my_str_erase(my_str_t* str, size_t beg, size_t end);

/*
 * pops last symbol in the my_str-string
 * return:
 *      0  if OK
 *      RANGE_ERR if size of string is already 0
 *      NULL_PTR_ERR if str is NULL
 *      else - popped symbol
 */
int my_str_popback(my_str_t* str);

/*
 * increases given my_str-string's buffer to given value.
 * if buf_size < my_str-string capacity, does nothing
 * return:
 *      0  if OK
 *      NULL_PTR_ERR if str is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_reserve(my_str_t* str, size_t buf_size);

/*
 * decreases buffer of given my_str-string to size of string
 * return:
 *      0  if OK
 *      NULL_PTR_ERR if from ot str is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_shrink_to_fit(my_str_t* str);

/*
 * resizes my_str-string to given size.
 * if given size > size of string than filles next byteswith given chars
 * return:
 *      0  if OK
 *      NULL_PTR_ERR if str is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_resize(my_str_t* str, size_t new_size, char sym);

/*
 * pushes given char on the end of my_str-string
 * if needed increases buffer by INC_PARAM (2)
 * return:
 *      0  if OK
 *      NULL_PTR_ERR if str or c is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during allocating memory for buffer
 */

size_t my_str_find(const my_str_t* str, const my_str_t* tofind, size_t from);

/*
 * compares two my_str-strings like conventional c-strings (in lexicographical order)
 * return:
 *      0 if str1 == str2
 *      -1 if str1 < str2
 *      1  if str1 > str2
 *      NULL_PTR_ERR if str1 or str2 is NULL
 */
int my_str_cmp(const my_str_t* str1, const my_str_t* str2);

/*
 * compares my_str-strings and c-stirng lexicographical order
 * return: the same as in my_str_cmp
 */
int my_str_cmp_cstr(const my_str_t* str1, const char* cstr2);

/*
 * returns position of given symbol in my_str-string, -1 if it's not there
 * starts search from given position
 * return:
 *      position of char symbol if its in string
 *      NOT_FOUND_CODE if it's not in string
 *      NULL_PTR_ERR if str is NULL
 */
size_t my_str_find_c(const my_str_t* str, char tofind, size_t from);

/*
 * returns position of symbol that predicate on ot returns 1, if there is no symbol like that than -1
 * return:
 *      position of char symbol that satisfies predicate
 *      NOT_FOUND_CODE if it's not in string
 *      NULL_PTR_ERR if str or predicate is NULL
 */
size_t my_str_find_if(const my_str_t* str, size_t beg, int (*predicat)(int));

/*
 * reads file and saves it's content into given my_str-string
 * resizes string's buffer if needed
 * return:
 *      0  if OK
 *      NULL_PTR_ERR if str or file is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_read_file(my_str_t* str, FILE* file);

/*
 * reads content from stdin and saves to my_str-string
 * return:
 *      0  if OK
 *      NULL_PTR_ERR if str is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during increasing buffer
 *      IO_READ_ERR if there was error while reading
 */
int my_str_read(my_str_t* str);

/*
 * reads file and saves it's content into my_str-string, stops when reached delimiter or EOF
 * return:
 *      0  if OK
 *      NULL_PTR_ERR if str or file is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_read_file_delim(my_str_t* str, FILE* file, char delimiter);

/*
 * writes content of given my_str-string to given file
 * return:
 *      0  if OK
 *      NULL_PTR_ERR if str or file is NULL
 *      IO_WRITE_ERR if error occurred while writing
 */
int my_str_write_file(const my_str_t* str, FILE* file);

/*
 * writes content of given my_str-string into console
 * return:
 *      0  if OK
 *      NULL_PTR_ERR if str is NULL
 *      IO_WRITE_ERR if error occurred while writing
 */
int my_str_write(const my_str_t* str);

#error "APPS_UCU_ERROR: please check functions' names and signatures before deleting this error!
