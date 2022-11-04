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
/* Fix the refrence back FUNCTIONALITY */
#include "regex.h"
#include "engine.h"
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/* NEXT Subexpression*/
regex_t * regexInit ( char * str ) {

    regex_t * t = malloc ( sizeof ( regex_t ) );
    t ->begin = str;
    t->pos = 0;
    t->end = str + strlen ( str );
    return t;
}

int match ( regex_t * regex, regex_t * str, char * expre , size_t kcount ) {

    char * expressions [10] = { NULL };
    if ( expre != NULL ) {
        for ( size_t i = 0; i < kcount; ++i ) {
            expressions [i] = (expre + i * 8);
        }
    }

    szRange a;
    size_t count = kcount;
    bool negate = false;
    int _c = 0;
    size_t min = 1;
    bool srange = false;

    switch ( regex->begin [ regex->pos ] ) {

        case 0:
            if ( str->begin [str->pos] == 0 ) return SUCCES;
            else return FAIL;
        case '}':
            regex->pos++;
            return SUCCES;
        case ')':
            regex->pos++;
            return SUCCES;
        case '(':
            regex->pos++;

            if ( regex->begin [regex->pos] == 0 ) break; // special case tested
            
            /* Helper variables */
            size_t rsave = regex->pos, save = str->pos, end;

            int c = match ( regex, str , NULL, 0 );
            end = regex->pos - 1;



            if ( c == ERROR ) return ERROR;
            if ( c == FAIL ) {

                if ( !seakToEnd ( regex )) return ERROR;
                str->pos = save;
                if ( regex->begin [regex->pos] == '?' || regex->begin [regex->pos] == '*' ) {
                    regex->pos++;
                    break;
                } else if ( seaktoPipe ( regex ) == FAIL ) {
                    str->pos = 0;
                    return FAIL;
                }
                regex->pos++;
                break;
            }
            if ( count <= 9 ) {
                size_t e = end - save - 1;
                expressions [count] = malloc ( sizeof ( char ) * (e + 1) );
                strncpy ( expressions [count], &str->begin [save], e );
                expressions [count] [e] = 0;
                count++;
            }
            if ( regex->begin [regex->pos] == '+' || regex->begin [regex->pos] == '*') {
    
                regex->pos = rsave;
                
                for ( ; match ( regex, str, NULL, 0 ) == SUCCES; regex->pos = rsave ) continue;
                
                regex->pos = end + 2;
                break;
            } else if ( regex->begin [regex->pos] == '{' ) {
                
                a = getszRange ( regex );
                end = regex->pos;
                if ( a.state == ERROR ) return ERROR;

                a.max--;
                regex->pos = rsave;

                size_t i = 1;                
                for ( ; match ( regex, str, NULL, 0 ) == SUCCES && i < a.max; regex->pos = rsave, ++i ) continue;
                
                regex->pos = end + 1;
                if ( i < a.min ) return FAIL;

                break;
            } else if ( regex->begin [regex->pos] == '?' ) regex->pos++;

            break;
        case '|':
            return 0;
        case '\\':
            /* NOT YET FINISHED */            

            if ( regex->begin [regex->pos + 1] == '\\' ) {

                if ( str->begin [str->pos] == '\\' ) {
                    regex->pos += 2;
                    str->pos++;
                    break;
                }

                return 1;
            } else if ( isdigit ( (int) regex->begin [regex->pos+1] ) ) {

                regex->pos++;
                int n = (regex->begin [regex->pos]) ^ 0x30;
                if ( expressions [n] != NULL ) {

                    regex->pos++;
                    regex_t * p = regexInit ( expressions [n] );
                    if ( (n = match ( p, str, expressions [0], count )) == SUCCES ) {
                        regexFree ( p );
                        break;
                    }
                    else  {
                        regexFree ( p );
                        return n;
                    }
                // add those options
                } else return ERROR;
            } else if ( regex->begin [regex->pos+1] == str->begin [str->pos] ) {
                if ( regex->begin [regex->pos + 2] == '?' ) {
                    regex->pos++;
                }
                regex->pos += 2;
                str->pos++;
                break;
            } else if ( regex->begin [regex->pos + 2] == '?' || regex->begin [regex->pos + 2] == '*' ) {
                regex->pos += 3;
                break;
            }
            if ( seaktoPipe ( regex ) == FAIL ) return FAIL;
            regex->pos++;
            str->pos = 0;
            
        case '[':


            if ( regex->begin [regex->pos + 1] == '^' ) {
                regex->pos++;
                negate = true;
            }
            range_t * range = getrange ( regex );
            bool inR = isInrange ( range, str->begin [str->pos] );

            if ( regex->begin [regex->pos + 1] == '{' ) {

                regex->pos++;
                a = getszRange ( regex );
                
                if ( a.state == ERROR ) return ERROR;

                min = a.min;
                srange = true;
            }



            if ( inR && !negate ) {

                if ( srange ) {

                    size_t i = 0;
                    for ( ; isInrange ( range, str->begin [str->pos] ) && i < a.max; str->pos++, ++i ) continue;
                    if ( i < a.min ) return FAIL;
                
                } else if ( regex->begin [regex->pos + 1] == '?' )  regex->pos += 2;
                else if ( regex->begin [regex->pos + 1 ] == '*' || regex->begin [regex->pos + 1] == '+' ) {

                    for ( ; isInrange ( range, str->begin [str->pos] ); str->pos++ ) continue;
                    regex->pos += 2;

                } else {
                    str->pos++;
                }

                break;
            } else if ( negate && !inR ) {
                
                if ( srange ) {

                    size_t i = 0;
                    for ( ; !isInrange ( range, str->begin [str->pos] ) && i < a.max; str->pos++, ++i ) continue;
                    if ( i < a.min ) return FAIL;
                }

                if ( regex->begin [regex->pos + 1] == '?' || regex->begin [regex->pos + 1] == '*' || regex->begin [regex->pos + 1] == '+' ) {
                    regex->pos++;
                }
                if ( regex->begin [regex->pos] == '*' || regex->begin [regex->pos] == '+' ) {

                    for ( ; !inR && str->begin [str->pos] != 0 ; str->pos++ ) inR = isInrange ( range, str->begin [str->pos] );

                    str->pos--; // the above loop step an additional char in the string so we remove it
                } else {
                    str->pos++;
                }
                regex->pos++;
                //freeRange ( range );
                break;
            } else if ( regex->begin [regex->pos + 1] == '*' || regex->begin [regex->pos + 1] == '?' || min == 0 ) {
                regex->pos += 2;
                //freeRange ( range );
                break;
            }
            if ( seaktoPipe ( regex ) == FAIL ) return FAIL;
            regex->pos++;
            str->pos = 0;

            break;
        case '^':
            if ( regex->begin [regex->pos] == '^' && &str->begin [str->pos] == str->begin ) {
                regex->pos++;
                break;
            }
            if ( seaktoPipe ( regex ) == FAIL ) return FAIL;
            regex->pos++;
            str->pos = 0;
            break;
        case '.':
            _c = str->begin [str->pos];
            if ( _c == 0 || _c == '\n') return FAIL;
            if ( !isascii ( _c ) ) return FAIL;
            regex->pos++;
            str->pos++;
            break;
        case '*':
            
            if ( *regex->begin == '*' ) return FAIL;
            for ( ; str->begin [str->pos] == regex->begin [regex->pos - 1]; str->pos++ ) continue;
            regex->pos++;
            break;

        case '+':

            if ( *regex->begin == '+' ) return FAIL;

            for ( ; str->begin [str->pos] == regex->begin [regex->pos - 1]; str->pos++ ) continue;
            regex->pos++;
            break;
        case '$':
            if ( regex->begin [regex->pos + 1] != '|' && regex->begin [regex->pos + 1] != 0 ) return ERROR;
            if ( str->begin [str->pos] == 0 ) return SUCCES;
            if ( seaktoPipe ( regex ) == FAIL ) return FAIL;
            regex->pos++;
            str->pos = 0;
            break;
        default :

            char d;
            if ( regex->begin [regex->pos + 1] == '{' ) {

                d = regex->begin [regex->pos];
                regex->pos++;
                a = getszRange ( regex );
                
                if ( a.state == ERROR ) return ERROR;

                size_t i = 0;
                for ( ; d == str->begin [str->pos] && i < a.max; str->pos++, ++i ) continue;
                if ( i < a.min ) return FAIL;

                break;

            } else if ( regex->begin [regex->pos] == str->begin [str->pos] ) {

                if ( regex->begin [regex->pos + 1] == '?' ) {
                    regex->pos++;
                }

                regex->pos++;
                str->pos++;
                break;
            } else if ( regex->begin [regex->pos + 1] == '?' || regex->begin [regex->pos + 1] == '*' ) {
                regex->pos += 2;
                break;
            }
            if ( seaktoPipe ( regex ) == FAIL ) return FAIL;
            regex->pos++;
            str->pos = 0;
            break;

    }
    return match ( regex, str, expressions [0], count );
}

void regexFree ( regex_t * regex ) {

    regex->begin = NULL;
    regex->end = NULL;
    free ( regex );
    return;
}
