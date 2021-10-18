#pragma once
#error "APPS_UCU_ERROR: pragma is not enough. Please, add correct include guards"

//! For the PVS studio -- disable inappropriate warnings.
//-V::122 // Disables overly excessive warning.
//-V::203 // Disables warning for the explicit casts.

#include <stdlib.h>
#include <stdint.h> // For size_t
#error "APPS_UCU_ERROR: some of the important includes are missing"
#include <stdio.h>

/* ############## ERROR codes ############### */
// Show that character or substring cannot be found
#define NOT_FOUND_CODE (-1) 
// Failed to allocate memory. All functions that directly or
// indirectly (a.e. call my_str_reserve) allocate memory may return this code
#define MEMORY_ALLOCATION_ERR (-2)
// Access to invalid index attempted.
// Should be returned by the my_str_getc, my_str_putc; insert and substr families and
// my_str_popback, when the string is empty.
#define RANGE_ERR (-3)
// Error reading file. May be returned by the "read" family functions. Note: stdin is a file too.
#define IO_READ_ERR (-4)
// Error writing file. May be returned by the "write" family functions. Note: stdout and stderr are files too.
#define IO_WRITE_ERR (-5)
// Null pointer is given to functions which cannot deal with it:
// my_str_create, my_str_from_cstr, my_str_get_cstr
//        my_str_getc, my_str_putc,
//        my_str_copy,
//         insert, append and substr families
// my_str_reserve, my_str_shrink_to_fit, my_str_resize
//        read and write families
// Note: some others (e.g. my_str_free) have explicitly defined behaviours.
#define NULL_PTR_ERR (-8)
// The buffer is too small for my_str_from_cstr
#define BUFF_SIZE_ERR (-9)

// Students can define additional error codes.

typedef struct {
    size_t capacity_m; // Allocated memory block size
    size_t size_m;     // Actual size of the string
    char *data;        // Pointer to the data block - address in memory where the allocated block begins
} my_str_t;

// ###########################################################
// ############### construction and destruction ##############
// ###########################################################

/*
 * Creates an empty dynamic string (my_str_t).
 * Important! User should always use my_str_create before using ANY other function.
 *
 * This function plays the role of a constructor, well-known in other languages.
 * It signifies that you take ownership of the resource - memory, which a user of your library
 * will manage with other functions that you supply, and then free, when it is no longer needed.
 * In other words, the lifetime of the string begins after this function is called, and ends after
 * the my_str_free called.
 * So, this function should be called once and only once before using any other string functions.
 * Once more: one should never call any other library function for the my_str_t variable
 * before calling  my_str_create.
 * 
 * str: pointer to my_str_t struct that should be initialized
 * buf_size: size of memory (in bytes) that should be allocated for the buffer
 *       It is a good idea to allocate buf_size+1 bytes -- to have a space for \0 symbol,
 *       useful for the optimal my_str_get_cstr() implementation.
 * Returns:
 *      0  if OK
 *      NULL_PTR_ERR if str points to NULL
 *      MEMORY_ALLOCATION_ERR if there was an error while allocating memory for the buffer
 */
int my_str_create(my_str_t* str, size_t buf_size);

/*
 * Frees buffer for the given my_str_t structure and sets NULL to the corresponding pointer.
 *
 *  This function plays the role of the object destructor, well-known in other languages.
 *  Should be called exactly once, at the end of my_str_t lifecycle.
 *
 *  Note 1: free() can be safely called with the NULL argument.
 *  Note 2: always returns 0 for uniformity and future compatible API/ABI extensions.
 *
 * Returns:
 *     0 always
 */
int my_str_free(my_str_t* str);

/*
 * Makes content of the my_str_t-string the same as c-string.
 * Implementation should not call my_str_create!
 *
 * Note 1: function should copy the content of the c-string, pointed to by the cstr to its internal buffer.
 *       Simply assigning the internal buffer (data) pointer to the cstring is an extremely terrible idea!
 *       (Based on typical students' mistakes).
 * Note 2: memory should be allocated from the heap, and not from the stack.
 *
 * cstr: c-string (pointer) from which the data should be copied
 * buf_size: new size of the buffer
 *      if buf_size == 0 -> new buffer size = length of c-string
 *      if buf_size < size(cstr) -> return error. In this case str should not be modified!
 * 
 * Returns:
 *      0  if OK
 *      NULL_PTR_ERR if str or cstr is NULL
 *      BUFF_SIZE_ERR if buf_size is incorrect (too small)
 *      MEMORY_ALLOCATION_ERR if there occurred an error while allocating memory.
 */
int my_str_from_cstr(my_str_t* str, const char* cstr, size_t buf_size);

// ###########################################################
// ########################### string information ############
// ###########################################################

/*
 * Returns the actual size of my_str_t string.
 * if str == NULL should return 0.
 * 
 */
size_t my_str_size(const my_str_t* str);

/*
 * Returns the size of the allocated memory for my_str_t
 * Note: if you implemented the tip above, returns (size-1) because one byte is reserved for the \0 symbol.
 * If str == NULL should return 0.
 */
size_t my_str_capacity(const my_str_t* str);

/*
 * Returns 1 if the string is empty.
 * If str == NULL should return 1 (true).
 */
int my_str_empty(const my_str_t* str);

// ###########################################################
// #################### character access #####################
// ###########################################################

/*
 * index - obvious
 * 
 * return:
 *      symbol at the index in my_str_t-string
 *      NULL_PTR_ERR if str == NULL
 *      RANGE_ERR if the index is invalid
 * 
 * Note: does not return char because of the need to also return the error codes
 */
int my_str_getc(const my_str_t* str, size_t index);

/*
 * places a given symbol at the given position in my_str_t-string
 * with replacing of the previous one
 * 
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if the str or c is NULL
 *      RANGE_ERR if the index is bad
 */
int my_str_putc(my_str_t* str, size_t index, char c);

/*
 * returns a c-string with the same content as my_str_t-string
 * the resulting c-string is constant and becomes invalid if my_str_t content is modified
 * time complexity should be O(1)!
 * 
 * returns NULL if str is NULL, otherwise the appropriate c-string
 */
const char* my_str_get_cstr(my_str_t* str);

// ###########################################################
// ################# string modifications ####################
// can cause size increases and decreases, and so, trigger memory buffer reallocation
// if the buffer is too small these functions should increase it by calling my_str_reserve
// in this case, the buffer should be increased to about 1.8-2 times its previous capacity
// ###########################################################

/*
 * copies the content of the "from" my_str_t to "to" my_str_t
 * if reserve == true -> "to" string's capacity should be the same as the "from" string's capacity
 * if reserve == false -> use "to" string's capacity if it is enough, otherwise reserve minimal
 * required capacity to hold the "from" string's content
 * 
 * Note: do not forget to free "to" string's old buffer if you allocate a new one
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if at least one of the strings is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 *      RANGE_ERR if reserve is invalid (not 0 or 1)
 */
int my_str_copy(const my_str_t* from,  my_str_t* to, int reserve);

/*
 *  clears the content of the string, does nothing if the string is NULL
 * Note: should have O(1) complexity
 *  return:
 *      always 0
 * Alternatively may return NULL_PTR_ERR if str is NULL
 */
int my_str_clear(my_str_t* str);

/*
 * inserts the given char at the given position in my_str, shifts all the following symbols
 * to the right
 * increases the capacity if needed
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if str or c is NULL
 *      RANGE_ERR if position is bad
 *      MEMORY_ALLOCATION_ERR if occurred error during memory allocation
 */
int my_str_insert_c(my_str_t* str, char c, size_t pos);

/*
 * inserts the "str" string into the "from" string at the given position, 
 * shifts all the following symbols to the right
 * increases the capacity if needed
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if str or from is NULL
 *      RANGE_ERR if position is bad
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_insert(my_str_t* str, const my_str_t* from, size_t pos);

/*
 * inserts given c-string into given my_str-string, shifts all the following symbols to the right
 * increases the capacity if needed
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if str or from is NULL
 *      RANGE_ERR if position is bad
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_insert_cstr(my_str_t* str, const char* from, size_t pos);

/*
 * appends the "from" my_str-string to "str" string
 * increases the capacity if needed
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if from or str is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_append(my_str_t* str, const my_str_t* from);

/*
 * appends c-string to my_str-string
 * increases the capacity if needed
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if from or str is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_append_cstr(my_str_t* str, const char* from);

/*
 * appends a character to my_str-string
 * increases the capacity if needed
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if str or c is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during allocating memory for buffer
 */
int my_str_append_c(my_str_t* str, char c);

/*
 * copies a substring of "from" string to "to" string in bounds [beg, end)
 * (beg inclusive, end exclusive)
 * if end > "from" string size - copy the whole remainder of the string, not a bad index
 * if beg > "from" string size - bad index
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if from or to is NULL
 *      RANGE_ERR if boundaries are bad
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_substr(const my_str_t* from, my_str_t* to, size_t beg, size_t end);

/*
 * copies a substring of "from" string to "to" c-string in bounds [beg, end)
 * (beg inclusive, end exclusive)
 * !important! the c-string should be big enough, this is the user's responsibility
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if from or to is NULL
 *      RANGE_ERR if boundaries are bad
 */
int my_str_substr_cstr(const my_str_t* from, char* to, size_t beg, size_t end);

/*
 * deletes a substring in bounds [beg, end), shifts the remainder to the left
 * (beg inclusive, end exclusive)
 * end > size is not an error - erase the whole remainder
 * beg > size is an error
 * return: 
 *      0 if OK
 *      RANGE_ERR if boundaries are bad
 *      NULL_PTR_ERR if str is NULL
 */
int my_str_erase(my_str_t* str, size_t beg, size_t end);

/*
 * deletes the last symbol from the my_str-string and returns it
 * return:
 *      the popped symbol if OK
 *      RANGE_ERR if the size of the string is already 0
 *      NULL_PTR_ERR if the str is NULL
 */
int my_str_popback(my_str_t* str);

/*
 * increases the my_str-string's buffer to the given value
 * if buf_size <= my_str-string capacity, does nothing
 * 
 * Note1: remember to allocate buf_size+1 if you implemented the tip from my_str_create(),
 * and do it only once per each buffer allocation
 * Note2: do not copy the whole buffer to the new one, only the actual string
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if str is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_reserve(my_str_t* str, size_t buf_size);

/*
 * shrinks the buffer of the my_str-string to its actual size
 * Note1: if not explicitly mentioned in other functions' descriptions, do not implement
 * this behaviour
 * Note2: remember to allocate buf_size+1 if you implemented the tip from my_str_create(),
 * and do it only once per each buffer allocation
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if str is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_shrink_to_fit(my_str_t* str);

/*
 * resizes my_str-string to the new_size
 * if new_size == size of the string -> does nothing
 * if new_size < size of the string -> truncates the string starting from the end
 * if new_size > size of the string -> fills the remainder with sym
 * increases the buffer if needed
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if str is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_resize(my_str_t* str, size_t new_size, char sym);

// ###########################################################
// ############ lookup and comparison functions ##############
// All of these function return their result or NOT_FOUND_CODE (which indicates the result's absence).
// 
// This is essentially a completely different API, but a more homogenous approach --
// make them return more meaningful error codes and provide their results to the user via pointers --
// would make the user code unnecessarily bloated.
// 
// However, do not forget to check for NULL pointers! Simply handle NULL pointers as you would empty strings.
// 
// Note: It is better to explicitly convert NOT_FOUND_CODE to size_t where necessary like this:
// (size_t) NOT_FOUND_CODE
// ###########################################################

/*
 * finds the first (from the "from", inclusive) substring that matches "tofind" in the "str" string and return the 
 * index of its first symbol
 *
 * if "tofind" is larger than "str" or empty -> consider it not found
 * 
 * return:
 *      0 if OK
 *      NOT_FOUND_CODE if not found
 */

size_t my_str_find(const my_str_t* str, const my_str_t* tofind, size_t from);

/*
 * compares two my_str-strings like conventional c-strings (in lexicographical order)
 *
 * Note: consider NULL equivalent to an empty string
 * return:
 *       0 if str1 == str2
 *      -1 (or another value < 0) if str1 < str2
 *       1 (or another value > 0) if str1 > str2
 */
int my_str_cmp(const my_str_t* str1, const my_str_t* str2);

/*
 * compares a my_str-strings and a c-string in lexicographical order
 *
 * Note: consider NULL equivalent to an empty string
 * return:
 *       0 if str1 == cstr2
 *      -1 (or another value < 0) if str1 < cstr2
 *       1 (or another value > 0) if str1 > cstr2
 */
int my_str_cmp_cstr(const my_str_t* str1, const char* cstr2);

/*
 * finds the given symbol in my_str-string starting from "from", inclusive
 *
 * Note: consider NULL equivalent to an empty string
 * return:
 *      index of the symbol if it exists in the string
 *      NOT_FOUND_CODE if it's not in the string
 */
size_t my_str_find_c(const my_str_t* str, char tofind, size_t from);

/*
 * finds the a symbol for which the precidate returns 1 in my_str-string starting from "from", inclusive
 *
 * Note: consider NULL equivalent to an empty string
 * return:
 *      index of the symbol that satisfies the predicate
 *      NOT_FOUND_CODE if it's not in the string
 */
size_t my_str_find_if(const my_str_t* str, size_t beg, int (*predicat)(int));

// ###########################################################
// ############## input and output functions #################
// Back to the original API.
// 
// Assume that the file is properly open (no UB - undefined behave), but do not assume that the
// file pointer is entirely correct (consider situations like trying to write to a file opened for reading).
// Tip: look into the ferror() function
// ###########################################################

/*
 * reads the file's content and saves it to the my_str-string
 * resizes the string's buffer if needed
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if str or file is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 *      IO_READ_ERR if an error occured while reading
 */
int my_str_read_file(my_str_t* str, FILE* file);

/*
 * reads the content of stdin and it to the my_str-string
 * resizes the string's buffer if needed
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if str is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 *      IO_READ_ERR if an error occured while reading
 */
int my_str_read(my_str_t* str);

/*
 * reads the file's content and saves it to the my_str-string, stops when reaches delimiter or EOF
 * resizes the string's buffer if needed
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if str or file is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 *      IO_READ_ERR if an error occured while reading
 */
int my_str_read_file_delim(my_str_t* str, FILE* file, char delimiter);

/*
 * writes the content of the given my_str-string to the given file
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if str or file is NULL
 *      IO_WRITE_ERR if an error occurred while writing
 */
int my_str_write_file(const my_str_t* str, FILE* file);

/*
 * writes content of the given my_str-string to the stdout
 * return:
 *      0 if OK
 *      NULL_PTR_ERR if str is NULL
 *      IO_WRITE_ERR if an error occurred while writing
 */
int my_str_write(const my_str_t* str);

#error "APPS_UCU_ERROR: please check functions' names and signatures before deleting this error!
