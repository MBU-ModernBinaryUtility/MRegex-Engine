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
#include "regex.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

bool misblank ( int c ) {

    if ( c == ' ' || c == '\r' || c == '\n' || c == '\t' ) return true;
    return false;
}
// freerange necessary
/* I don't know what hapening here but every time i get segfault on every platform */
void freeRange ( range_t * r ) {
/* on my machine there is a bug I get segfault everytime I free this pointer so I'll wait
until the operating system collects the resources*/
//#if defined (_WIN32) || defined (_WIN64)
    if ( r->a ) free ( r->a ); // fix this

    if ( r->List ) free ( r->List );
    free ( r );

    return;
}

range_t * getrange ( regex_t * regex ) {

    range_t * range = malloc ( sizeof ( range ) );
    range->a = malloc ( sizeof ( __range_t ) );
    range->a [0].start = 0;
    range->a [0].end = 0;
    range->asz = 0;
    range->lsz = 0;

    regex->pos++;

    while ( regex->begin [regex->pos] != ']' ) {

        if ( regex->begin [regex->pos + 1] == '-' && regex->begin [regex->pos + 2] != ']' ) {

            range->asz++;
            if ( range->asz == 1 ) {
                range->a = malloc ( sizeof ( sizeof ( __range_t ) ) );

            } else {
                range->a = realloc ( range->a, sizeof ( __range_t ) * range->asz );

            }

            range->a[range->asz - 1].start = regex->begin [regex->pos];

            if ( regex->begin [regex->pos + 2] == '\\' ) { 
                range->a[range->asz - 1].end = regex->begin [regex->pos + 3]; 
                regex->pos++;
            }
            else range->a[range->asz - 1].end = regex->begin [regex->pos + 2];

            regex->pos += 2;
        } else {

            range->lsz++;
            if ( range->lsz == 1 ) {
                range->List = malloc ( sizeof ( char ) );

            } else {
                range->List = realloc ( range->List, sizeof ( char ) * range->lsz );

            }
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

        if ( regex->begin [regex->pos] == '(' && regex->begin [regex->pos - 1] != '\\' ) {
            /*deal with it*/
            seakToEnd ( regex );
        }
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

    regex->pos = save;
    return FAIL;
}

bool seakToEnd ( regex_t * regex ) {

    do {

        if ( regex->begin [regex->pos] == ')' ) {
            regex->pos++;
            return true;
        }
        if ( regex->begin [regex->pos] == '(' && regex->begin [regex->pos-1] != '\\' ) {
            regex->pos++;
            seakToEnd ( regex );
        }
        if ( regex->begin [regex->pos] == '\\' ) regex->pos++;
        regex->pos++;
    } while ( regex->begin [regex->pos] != 0 );
    return false;
}

szRange getszRange ( regex_t * regex ) {
    szRange r;
    size_t be = regex->pos;
    do {
        if ( regex->begin [regex->pos] == 0 ) {
            r.state = ERROR;
            return r;
        }
 
        regex->pos++; 
 
    } while ( regex->begin [regex->pos] != '}' );
    

    size_t len = regex->pos - be;
    char * str = malloc ( len + 1 );

    strncpy ( str, &regex->begin [be], len );
    str [len] = 0;
    if ( sscanf ( str, "{ %lu, %lu }", &r.min, &r.max ) <= 0 ) r.state = ERROR;
    else r.state = SUCCES;
    free ( str );

    return r;
}