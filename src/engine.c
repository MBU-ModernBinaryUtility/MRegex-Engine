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
#include "engine.h"
#include "Mregex.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define isrange( i, b, c ) (i >= b && i <= c)   

// Used because the original one had problemes
bool misblank ( int c, bool l ) {

    if ( c == ' ' || ( (c == '\r' || c == '\n') && !l ) || c == '\t' ) return true;
    return false;
}

// remove all the white spaces from the string
char * clearifiy ( char * str, bool mul ) {

    size_t i = 0, d = 0;
    while ( str [d] != 0 ) {

        while ( misblank ( str [d], mul ) ) {
            d++;
        }
        str [i] = str [d];
        ++i;
        ++d;
    }
    str [i] = 0;
    return str;
}

char * strToLower ( char * c ) {

    size_t len = strlen ( c );
    for ( size_t i = 0; i < len; ++i ) {
        c [i] = tolower ( c[i] );
    }
    return c;
}

// freerange necessary
void freeRange ( range_t * r ) {

    if ( r->a ) free ( r->a );
    if ( r->List ) free ( r->List );
    free ( r );

    return;
}

range_t * getrange ( regex_t * regex ) {

    // initialize the range
    range_t * range = malloc ( sizeof ( range_t ) );
    range->a = malloc ( sizeof ( __range_t ) );
    range->List = malloc ( sizeof ( char ) );
    range->asz = 0;
    range->lsz = 0;


    regex->pos++;

    while ( regex->begin [regex->pos] != 0 ) {

        if ( regex->begin [regex->pos] == ']' && regex->begin [regex->pos - 1] != '\\' ) break; // found the end

        if ( regex->begin [regex->pos] == '\\' ) regex->pos++;        
        // '-' considered as a character if it's at the begining or at the end
        if ( regex->begin [regex->pos + 1] == '-' && regex->begin [regex->pos + 2] != ']' ) {

            range->asz++;
            range->a = realloc ( range->a, sizeof ( __range_t ) * range->asz );

            range->a[range->asz - 1].start = regex->begin [regex->pos];

            if ( regex->begin [regex->pos + 2] == '\\' ) { 
                range->a[range->asz - 1].end = regex->begin [regex->pos + 3]; 
                regex->pos++;
            }
            else range->a[range->asz - 1].end = regex->begin [regex->pos + 2];

            // final check
            if ( range->a->end < range->a->start ) {

                size_t c = range->a[range->asz - 1].start;
                range->a[range->asz - 1].start = range->a[range->asz - 1].end;
                range->a[range->asz - 1].end = c;
            }
            regex->pos += 2;
        } else {

            range->lsz++;
            range->List = realloc ( range->List, sizeof ( char ) * range->lsz );

            if ( regex->begin [regex->pos] == '\\' ) range->List [range->lsz - 1] = regex->begin [regex->pos + 1];
            else range->List [range->lsz - 1] = regex->begin [regex->pos];
        }

        regex->pos++;
    }

    return range;
}

bool isInrange ( range_t * range, int c ) {

    for ( size_t i = 0; i < range->lsz; ++i ) {
        if ( c == range->List [i] ) return true;
    }

    for ( size_t i = 0; i < range->asz; ++i ) {
        if ( isrange ( c, range->a[i].start, range->a[i].end ) ) return true;
    }

    return false;
}

int seaktoPipe ( regex_t * regex ) {

    size_t save = regex->pos;
    while ( regex->begin [regex->pos] != 0 ) {

        // skip the sub-expression
        if ( regex->begin [regex->pos] == '(' && regex->begin [regex->pos - 1] != '\\' ) {
            seakToEnd ( regex );
        }
        // test if escaped like this  '\|': it match the '|' character
        if ( regex->begin [regex->pos - 1] != '\\' ) {

            switch ( regex->begin [regex->pos] ) {

                case '[':
                    while ( &regex->begin [regex->pos] != regex->end ) {
                        if ( regex->begin [regex->pos] == ']' && regex->begin [regex->pos] != '\\' ) break;
                        regex->pos++;
                    }
                    break;
                case '|':
                    
                    return SUCCES;
                default:
                    regex->pos++;
                    break;
            }
        } else regex->pos++;
    }

    regex->pos = save; // restore the position if failed
    return FAIL;
}

bool seakToEnd ( regex_t * regex ) {

    do {

        if ( regex->begin [regex->pos] == ')' ) {
            regex->pos++;
            return true; // the job has done
        }
        if ( regex->begin [regex->pos] == '(' ) {
            // skip the nested sub-expression
            regex->pos++;
            seakToEnd ( regex );
        }
        // skip the escaped character
        if ( regex->begin [regex->pos] == '\\' ) regex->pos++;
        regex->pos++;

    } while ( regex->begin [regex->pos] != 0 );
    return false;
}

// find the end of curly brace that's not escaped
// the function is recursive
bool findEnd ( regex_t * regex ) {

    do {

        regex->pos++;
        if ( regex->begin [regex->pos] == '\\' ) regex->pos += 2;
        if ( regex->begin [regex->pos] == '}' ) return true;
        if ( regex->begin [regex->pos] == '{' ) {
            if ( !findEnd ( regex ) ) return false;
        }

    } while ( regex->begin [regex->pos] != 0 );

    return false;
}

szRange getszRange ( regex_t * regex ) {
    
    szRange r;
    // get the size if the str
    size_t be = regex->pos;
    if ( !findEnd ( regex ) ) {
        r.state = ERROR;
        return r;
    }
    size_t end = regex->pos, sz = end - be + 1;

    // create our str and clear it
    char * buff = malloc ( sz );
    strncpy ( buff, &regex->begin [be], (sz - 1) );
    buff [sz - 1] = 0;

    clearifiy ( buff, false );

    // get the necessary data
    if ( !sscanf ( buff, "{%zd,%zd}", &r.min, &r.max ) ) {
        // case failure
        r.state = ERROR;
        return r;
    }

    regex->pos++;

    // final check
    if ( r.min > r.max ) {

        size_t c = r.max;
        r.max = r.min;
        r.min = c;
    }
    return r;
}

bool findComma ( regex_t * regex ) {

    while ( regex->begin [regex->pos] != 0 ) {

        if ( regex->begin [regex->pos] == ',' && regex->begin [regex->pos - 1] != '\\' ) return true;

        regex->pos++;
    }

    return false;
}
