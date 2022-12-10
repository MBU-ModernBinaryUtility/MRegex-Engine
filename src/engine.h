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

#ifndef __ENGINE__
#define __ENGINE__

#include <stdbool.h>
#include "Mregex.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _szRange
{
    int state; // FAIL or SUCCES or ERROR
    size_t min, max;
} szRange;

typedef struct __range_t
{
    char start, end;
} __range_t;

// range
typedef struct _range_t {
    char * List;
    __range_t * a;
    size_t asz, lsz;
} range_t;

// remove all the white spaces from str
char * clearifiy ( char * str, bool );
char * strToLower ( char * str );

// get the range like: '[a-z]' 
range_t * getrange ( regex_t * );
// test if it's in range
bool isInrange ( range_t *, int c );
// destroy the range
void freeRange ( range_t * );
// find the pipe ('|') and set the regex->pos to it's position
// return SUCCES or FAIL
int seaktoPipe ( regex_t * );

// seak to the end of expression
bool seakToEnd ( regex_t * );

// get size range
szRange getszRange ( regex_t * );

// find the end of curly brace that's not escaped
bool findEnd ( regex_t * );

// find ',' in the given regex
bool findComma ( regex_t * regex );

#ifdef __cplusplus
}
#endif

#endif
