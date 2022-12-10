/*
 * MIT License

 * Copyright (c) 2022 MBU-ModernBinaryUtility

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct str_t {

    size_t len;
    char * data;
} bstr;

typedef struct str_l {

    size_t len;
    bstr ** list;
} Lbstr;

/* Create a bstr string from a C string */
bstr * bstrCreate ( const char * str );
/* Destroy a bstr string */
void bstrDestroy ( bstr * str );

/* return the lenght of bstr string */
#define bstrLen( s ) ( s == NULL ? NULL: s->len )
/* return a C string from bstr string */
#define bstrCStr( s ) ( s == NULL? NULL: s->data )


/// @brief return a character in the position pos or return fail if the postion out of range
/// @param str bstr string
/// @param pos position
/// @param fail the character to be returned
/// @return fail or str->data [pos]
char bstrSGetChar ( bstr * str, size_t pos, char fail );

/// @brief The same as bstrSGetChar but it return 0 as fail
/// @param str bstr string
/// @param pos position
/// @return 0 or str->data [pos]
char bstrGetChar ( bstr * str, size_t pos );


/// @brief concatenate a bstr string with c string
void bstrCatC ( bstr * str, const char * str2 );
/// @brief concatenate two bstr string
void bstrCat ( bstr * str, bstr * str2 );

/// @brief compare two bstr string
/// @return 0 if equal, > 0 if s1 > s2, < 0 if s1 < s2 
int bstrCmp ( bstr * s1, bstr * s2 );

/// @brief create a new bstr string from another bstr string
/// @return a pointer to the new bstr string
bstr * bstrCopy ( bstr * str );

/// @brief find the first "tofind" appear in "str"
/// @param str the search string
/// @param tofind to find
/// @return return -1 if it didn't find "tofind" in "str" else return the position
int64_t bstrFind ( bstr * str, bstr * tofind );

/// @brief find the n th appear of "tofind" in "str"
/// @param str the search string
/// @param tofind to find
/// @param index n
/// @return return -1 if it didn't find else the position
int64_t bstrFindIndex ( bstr * str, bstr * tofind, size_t index );

/// @brief count how many time "b" appeared in "s"
/// @return return how many time "b" appeared in "s"
size_t bstrCount ( bstr * s, bstr * b );

/// @brief split a bstr string from the given position
/// @param s string to split
/// @param pos the position to split
/// @return return LBSTR which is bstr list
Lbstr * bstrSplitp ( bstr * s, size_t pos );

/// @brief split the string in the n th appear of "b"
/// @param s the string to split
/// @param b the string to split on
/// @param index n
/// @return return LBSTR of bstr string
Lbstr * bstrSplitIndex ( bstr * s, bstr * b, size_t index );

/// @brief split the string on every appear of "b"
/// @param str the string to split
/// @param b the string to split on
/// @return return LBSTR of bstr string
Lbstr * bstrSplit ( bstr * str, bstr * b );

/// @brief  destroy the given Lbstr list
void LbstrDestroy ( Lbstr * l );

/// @brief turn the given bstr string into lowercase string
/// @return a pointer to the bstr string
bstr * bstrToLower ( bstr * str );

/// @brief turn the given bstr string into uppercase string
/// @return a pointer to the bstr string
bstr * bstrToUpper ( bstr * str );

/// @brief remove all the white space from the given bstr string
/// @return a pointer to the string
bstr * bstrClearify ( bstr * str );

/// @brief replace "what" with "with" in "str"
/// @return return a pointer to the string
bstr * bstrReplace ( bstr * str, bstr * what, bstr * with );

/// @brief replace "what" with "with" in the n th appear in "str"
/// @param Index n appear
/// @return return a pointer to the string
bstr * bstrReplaceIndex ( bstr * str, bstr * what, bstr * with, size_t Index );

/// @brief replace from begin to the end position by "what" in "str"
/// @return a pointer to the string
bstr * bstrReplacep ( bstr * str, size_t begin, size_t end, bstr * what );

/// @brief remove "toRemove" in "str"
/// @return a pointer to the string
bstr * bstrRemove ( bstr * str, bstr * toRemove );

/// @brief remove "toRemove" in "str" the n th appear
/// @param index n
/// @return a pointer to the string
bstr * bstrRemoveIndex ( bstr * str, bstr * toRemove, size_t index );

/// @brief reverse the given string
/// @return a pointer to the string
bstr * bstrReverse ( bstr * str );

/// @brief captalise the given string
/// @return a pointer to the string
bstr * bstrCaptalize ( bstr * str );

/// @brief append or insert "toAppend" to "str" in pos
/// @return the new string
bstr * bstrAppend ( bstr * str, bstr * toAppend, size_t pos );

/// @brief check if the given string is a number
/// @return true if it is a number
bool bstrIsNum ( bstr * str );

/// @brief check if the given string is floating-point number
/// @return true if it is a floating-point number
bool bstrIsFPNum ( bstr * str );

/// @brief check if the given string has all the letters in upper case
/// @return true if it is
bool bstrIsAllUpper ( bstr * str );

/// @brief check if the given string has all the letters in lower case
/// @return true if it is
bool bstrIsAllLower ( bstr * str );

/// @brief check if "str" ends with "with"
/// @return true if it is
bool bstrEndsWith ( bstr * str, bstr * with );

/// @brief check if "str" begins with "with"
/// @return true if it is
bool bstrBeginsWith ( bstr * str, bstr * with );

#ifdef __cplusplus
}
#endif