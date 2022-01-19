#pragma once
#error "APPS_UCU_ERROR: pragma is not enough. Please, add correct include guards"

//! For the PVS studio -- disable inappropriate warnings.
//-V::122 // Disables overly excessive warning.
//-V::203 // Disables warning for the explicit casts.

#include <stdlib.h>
#include <stdint.h> // For size_t
#include <stdio.h>

#error "APPS_UCU_ERROR: some of the important includes are missing"

/* ############## ERROR codes ############### */
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
// Note: several functions (e.g. my_str_free) have explicitly defined behavior -- see descriptions..
#define NULL_PTR_ERR (-8)
// The buffer is too small for my_str_from_cstr
#define BUFF_SIZE_ERR (-9)

// Students can define additional error codes.

typedef struct {
    size_t m_capacity; // Allocated memory block size
    size_t m_size;     // Actual size of the string
    char *m_data;        // Pointer to the data block - address in memory where the allocated block begins
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
 *
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
int my_str_create(my_str_t *str, size_t buf_size);

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
int my_str_free(my_str_t *str);

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
int my_str_from_cstr(my_str_t *str, const char *cstr, size_t buf_size);

// ###########################################################
// ########################### string information ############
// ###########################################################

/*
 * Returns the actual size of my_str_t string.
 * if str == NULL should return 0.
 * 
 */
size_t my_str_size(const my_str_t *str);

/*
 * Returns the size of the allocated memory for my_str_t
 * Note: if you implemented the tip above, returns (m_capacity-1) because one byte is reserved for the \0 symbol.
 * If str == NULL should return 0.
 */
size_t my_str_capacity(const my_str_t *str);

/*
 * Returns 1 if the string is empty.
 * If str == NULL should return 1 (true).
 */
int my_str_empty(const my_str_t *str);

// ###########################################################
// #################### character access #####################
// ###########################################################

/*
 * index - obvious
 * 
 * Returns:
 *      symbol at the index in my_str_t string, pointed by str.
 *      NULL_PTR_ERR if str == NULL
 *      RANGE_ERR if the index is invalid
 * 
 * Note: does not return char because of the need to also return the error codes and
 * general API uniformity.
 */
int my_str_getc(const my_str_t *str, size_t index);

/*
 * Places a given symbol at the given position in my_str_t string, pointed by str
 * with replacing of the previous one
 * 
 * Returns:
 *      0 if OK
 *      NULL_PTR_ERR if the str is NULL
 *      RANGE_ERR if the index is bad -- out of the range [0, str->m_size).
 */
int my_str_putc(my_str_t *str, size_t index, char c);

/*
 * Returns a c-string with the same content as my_str_t string, pointed by str.
 * The resulting c-string is constant and becomes invalid if my_str_t content is modified.
 * Time complexity should be O(1)!
 *
 * If the buffer m_data has one reserved byte (maximal m_size == m_capacity - 1), implementation
 * can just put the \0 char at the end of the actual string and return the m_data pointer.
 * 
 * Returns:
 *      NULL if str is NULL, otherwise the appropriate c-string.
 */
const char *my_str_get_cstr(my_str_t *str);

// #########################################################################################
// ################# string modifications ##################################################
// Those functions can cause string size increases and decreases, trigger memory buffer
// reallocation.
// If the buffer is too small these functions should increase it by calling my_str_reserve().
// In this case, the buffer should be increased to about 1.8-2 times its previous capacity.
// #########################################################################################

/*
 * Copies the content of the "from" my_str_t to "to" my_str_t.
 * If reserve == true -> "to" string's capacity should be the same as the "from" string's capacity.
 * If reserve == false -> use "to" string's capacity if it is enough, otherwise reserve minimal
 * required capacity to hold the "from" string's content.
 *
 * Note 1: if !reserve (reserve == false) -- should reallocate only if to->m_data memory block
 * is too small for the from string contents.
 * Note 2: do not forget to free "to" string's old buffer if you allocate
 * a new one -- my_str_reserve() should take care of this.
 *
 * Returns:
 *      0 if OK
 *      NULL_PTR_ERR if at least one of the strings is NULL
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation
 */
int my_str_copy(const my_str_t *from, my_str_t *to, int reserve);

/*
 * Clears the content of the string, does nothing if the str is NULL.
 * Note: should have O(1) complexity
 *
 * Returns:
 *      always 0
 * Alternatively may return NULL_PTR_ERR if str is NULL (using the fail-fast principle)
 */
int my_str_clear(my_str_t *str);

/*
 * Inserts the given char at the given position in str, shifts all the following symbols
 * to the right.
 * Increases the capacity if needed.
 *
 * Note: Insert at the end -- position m_data[m_size] is allowed.
 *
 * Returns:
 *      0 if OK,
 *      NULL_PTR_ERR if str is NULL,
 *      RANGE_ERR if position is bad -- out of the range [0, m_size],
 *      MEMORY_ALLOCATION_ERR if occurred error during memory allocation.
 */
int my_str_insert_c(my_str_t *str, char c, size_t pos);

/*
 * Inserts the "str" string into the "from" string at the given position,
 * shifts all the following symbols to the right.
 * Increases the capacity if needed.
 *
 * Note: Insert at the end -- position m_data[m_size] is allowed.
 *
 * Returns:
 *      0 if OK,
 *      NULL_PTR_ERR if str or from is NULL,
 *      RANGE_ERR if position is bad -- out of the range [0, m_size],
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation.
 */
int my_str_insert(my_str_t *str, const my_str_t *from, size_t pos);

/*
 * Inserts given c-string from into given my_str_t string str,
 * shifts all the following symbols to the right.
 * Increases the capacity if needed.
 *
 * Note: Insert at the end -- position m_data[m_size] is allowed.
 *
 * return:
 *      0 if OK,
 *      NULL_PTR_ERR if str or from is NULL,
 *      RANGE_ERR if position is bad -- out of the range [0, m_size],
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation.
 */
int my_str_insert_cstr(my_str_t *str, const char *from, size_t pos);

/*
 * Appends the "from" my_str-string to "str" string.
 * Increases the capacity if needed.
 *
 * Returns:
 *      0 if OK,
 *      NULL_PTR_ERR if from or str is NULL,
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation.
 */
int my_str_append(my_str_t *str, const my_str_t *from);

/*
 * appends c-string from to my_str_t string str.
 * Increases the capacity if needed.
 *
 * Returns:
 *      0 if OK,
 *      NULL_PTR_ERR if from or str is NULL,
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation.
 */
int my_str_append_cstr(my_str_t *str, const char *from);

/*
 * Appends a character to my_str_t string str.
 * Increases the capacity if needed.
 *
 * Returns:
 *      0 if OK,
 *      NULL_PTR_ERR if str is NULL,
 *      MEMORY_ALLOCATION_ERR if there was an error during allocating memory for the buffer.
 */
int my_str_append_c(my_str_t *str, char c);

/*
 * Copies a substring of "from" string to "to" string in bounds [beg, end) --
 * (beg inclusive, end exclusive).
 * If end > "from" string size - copy the whole remainder of the string, do not return a bad index error.
 * if beg > "from" string size - returns the RANGE_ERR error.
 *
 * Returns:
 *      0 if OK,
 *      NULL_PTR_ERR if from or to is NULL,
 *      RANGE_ERR if boundaries are bad,
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation.
 */
int my_str_substr(const my_str_t *from, my_str_t *to, size_t beg, size_t end);

/*
 * Copies a substring of "from" string to "to" c-string in bounds [beg, end) --
 * (beg inclusive, end exclusive).
 *
 * Important note: the c-string should be big enough,
 *          this is the function user's responsibility.
 *
 * Returns:
 *      0 if OK,
 *      NULL_PTR_ERR if from or to is NULL,
 *      RANGE_ERR if boundaries are bad.
 */
int my_str_substr_cstr(const my_str_t *from, char *to, size_t beg, size_t end);

/*
 * Deletes a substring in bounds [beg, end), shifts the remainder to the left,
 * (beg inclusive, end exclusive).
 * end > size is not an error -- erase the whole remainder,
 * beg > size is an error.
 *
 * Returns:
 *      0 if OK,
 *      RANGE_ERR if boundaries are bad,
 *      NULL_PTR_ERR if str is NULL.
 */
int my_str_erase(my_str_t *str, size_t beg, size_t end);

/*
 * Deletes the last symbol from the my_str_t string srt and returns it.
 *
 * Returns:
 *      the popped symbol if OK,
 *      RANGE_ERR if the size of the string is already empty,
 *      NULL_PTR_ERR if the str is NULL.
 */
int my_str_popback(my_str_t *str);

// ###########################################################
// ########### String buffer and size modifications ##########
// ###########################################################
/*
 * Increases the my_str-string's buffer to the given size.
 * if buf_size <= my_str_t string str capacity, does nothing.
 *
 * Note 0: The most important function for this lab assignment.
 * Note 1: Remember to allocate buf_size+1 if you implemented the tip from my_str_create().
 * Note 2: Do not copy the whole buffer to the new one, only copy the actual string contents.
 *
 * Returns:
 *      0 if OK,
 *      NULL_PTR_ERR if str is NULL,
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation.
 */
int my_str_reserve(my_str_t *str, size_t buf_size);

/*
 * Shrinks the buffer of the my_str_t string str to its actual size.
 *
 * Note1: Other functions should not use this behavior if not explicitly
 *      required in their description.
 * Note2: Remember to allocate buf_size+1 if you implemented the tip from my_str_create().
 *
 * Returns:
 *      0 if OK,
 *      NULL_PTR_ERR if str is NULL,
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation.
 */
int my_str_shrink_to_fit(my_str_t *str);

/*
 * Resizes my_str_t string str to the new_size.
 * If new_size == size of the string -> does nothing,
 * if new_size < size of the string -> truncates the string starting from the end,
 * if new_size > size of the string -> fills the remainder with sym.
 *
 * Increases the buffer if needed. Should not decrease the buffer size.
 *
 * Returns:
 *      0 if OK,
 *      NULL_PTR_ERR if str is NULL,
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation.
 */
int my_str_resize(my_str_t *str, size_t new_size, char sym);

// ################################################################################################
// ############ lookup and comparison functions ###################################################
// All of these functions should return their result or SIZE_MAX
// (which indicates the result's absence).
//
// This is essentially a different approach to create C API than in the previous functions,
// but is more convenient here.
// Another possible approach: to make functions return more meaningful error codes and provide
// their results to the user via pointers -- would make the user code unnecessarily bloated.
//
// However, do not forget to check for NULL pointers!
// Simply handle NULL pointers as they are empty strings.
//
// ################################################################################################

/*
 * Finds the first substring that matches "tofind" in the "str", starting from the position "from"
 * (in the str), inclusive, and returns the index of its first symbol.
 * If "tofind" is larger than "str" or empty -> consider it is not found.
 * If "tofind" is empty -- consider it not found.
 *
 * Returns:
 *      position index if OK,
 *      SIZE_MAX if not found.
 */
size_t my_str_find(const my_str_t *str, const my_str_t *tofind, size_t from);

/*
 * Compares two my_str_t strings like the ordinary c-strings -- in lexicographical order.
 *
 * Note: consider NULL equivalent to an empty string.
 *
 * Returns:
 *       0 if str1 == str2,
 *      -1 (or another value < 0) if str1 < str2,
 *       1 (or another value > 0) if str1 > str2.
 */
int my_str_cmp(const my_str_t *str1, const my_str_t *str2);

/*
 * Compares a my_str_t string and a c-string in lexicographical order.
 *
 * Note: consider NULL equivalent to an empty string
 *
 * Returns:
 *       0 if str1 == cstr2,
 *      -1 (or another value < 0) if str1 < cstr2,
 *       1 (or another value > 0) if str1 > cstr2.
 */
int my_str_cmp_cstr(const my_str_t *str1, const char *cstr2);

/*
 * Finds the given symbol in my_str_t string "str", starting from "from", inclusive.
 *
 * Note: consider NULL equivalent to an empty string.
 *
 * Returns:
 *      index of the symbol if it exists in the string,
 *      SIZE_MAX if it's not in the string.
 */
size_t my_str_find_c(const my_str_t *str, char tofind, size_t from);

// TODO: rename to my_str_find_c_if for the uniformity.
/*
 * Finds a symbol for which the "precidat" function returns true (1),
 * in my_str-string starting from "from", inclusive.
 *
 * Note: consider NULL equivalent to an empty string.
 *
 * Returns:
 *      index of the symbol that satisfies the predicate,
 *      SIZE_MAX if it's not in the string.
 */
size_t my_str_find_if(const my_str_t *str, size_t beg, int (*predicate)(int));

// ########################################################################################
// ############################ input and output functions ################################
// Back to the original API approach.
// 
// Assume that the file is properly open (so, if no, we have expected UB -- undefined behave),
// but do not assume that the file handler is entirely correct.  For example, consider situations
// like trying to write to a file opened for reading.
//
// Tip: look into the ferror() function.
// ########################################################################################

/*
 * Reads the file's content and saves it to the my_str_t string.
 * Resizes the string's buffer if needed.
 *
 * Note: function can use additional internal buffer for the I/O operations.
 *
 * Returns:
 *      0 if OK,
 *      NULL_PTR_ERR if str or file is NULL,
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation,
 *      IO_READ_ERR if an error occurred while reading.
 */
int my_str_read_file(my_str_t *str, FILE *file);

/*
 * Reads the content of stdin and saves it to the my_str-string.
 * Resizes the string's buffer if needed.
 *
 * Note: function can use additional internal buffer for the I/O operations.
 *
 * Returns:
 *      0 if OK,
 *      NULL_PTR_ERR if str is NULL,
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation,
 *      IO_READ_ERR if an error occurred while reading.
 */
int my_str_read(my_str_t *str);

/*
 * Reads the file's content and saves it to the my_str_t string "str",
 * stops when reaches "delimiter" or EOF.
 * Resizes the string's buffer if needed.
 *
 * Note 1: If occurred, put delimiter into the string. (2021-10-18 addition).
 * Note 2: function can use additional internal buffer for the I/O operations.
 *
 * Returns:
 *      0 if OK,
 *      NULL_PTR_ERR if str or file is NULL,
 *      MEMORY_ALLOCATION_ERR if there was an error during memory allocation,
 *      IO_READ_ERR if an error occurred while reading.
 */
int my_str_read_file_delim(my_str_t *str, FILE *file, char delimiter);

/*
 * Writes the content of the given my_str_t string "str" to the given file.
 *
 * Returns:
 *      0 if OK,
 *      NULL_PTR_ERR if str or file is NULL,
 *      IO_WRITE_ERR if an error occurred while writing.
 */
int my_str_write_file(const my_str_t *str, FILE *file);

/*
 * Writes content of the given my_str_t string "str" to the stdout.
 *
 * Return:
 *      0 if OK,
 *      NULL_PTR_ERR if str is NULL,
 *      IO_WRITE_ERR if an error occurred while writing.
 */
int my_str_write(const my_str_t *str);

#error "APPS_UCU_ERROR: please check functions' names and signatures before deleting this error!
