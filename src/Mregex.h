/*
MIT License

Copyright (c) 2022 MBU-ModernBinaryUtility

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef __REGEX__
#define __REGEX__

#define SUCCES 0
#define FAIL 1
#define ERROR (-1)
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NoWhite   0b001
#define NoCase    0b010
#define MultiLine 0b100

typedef struct _regex_t {

    char * begin; // the beginning of the string
    char * end; // the end of the string
    size_t pos; // current position
} regex_t;

typedef struct _regex_l {

    regex_t ** regex;
    size_t len;
} regex_l; // regex list

// initialize our regular expression
regex_t * regexInit ( char * str );

/// @brief match the regex with str recursively
/// @param regex the regular expression string
/// @param str the string to be matched
/// @param expre the strings matched by the sub-expressions
/// @param kcount the count of expre
/// @param mul if multi-line mode enabled
/// @param sv A helper variable always set to 0
/// @return SUCCES, Faile, or ERROR
int match ( regex_t * regex, regex_t * str, char **, size_t, bool, size_t );

/// @brief the main function
/// @param regex the regular expression string
/// @param sz1 the size of string 1
/// @param str the string to match
/// @param sz2 the size of string 2
/// @param fstr the final string ( the feature is under-development )
/// @param mode the modes 
/// @return SUCCES, FAILED, or ERROR
int Regex ( char * regex, char * str, char **, unsigned char mode );

// destroy the regex
void regexFree ( regex_t * );

#ifdef __cplusplus
}
#endif

#endif
