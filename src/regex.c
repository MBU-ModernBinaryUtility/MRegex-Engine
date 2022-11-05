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
#include "Mregex.h"
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

int match ( regex_t * regex, regex_t * str, char ** expre , size_t kcount, bool mul ) {

    char * expressions [10] = { NULL };
    if ( expre != NULL ) {

        for ( size_t i = 0; i < kcount; ++i ) expressions [i] = expre [i];
    }

    szRange a;
    size_t count = kcount;
    bool negate = false;
    int _c = 0;
    size_t min = 1;
    bool srange = false;

    switch ( regex->begin [ regex->pos ] ) {

        case 0:
            if ( expre != NULL ) {

                for ( size_t i = 0; i < count; ++i ) free ( expressions [i] );
            }
            if ( str->begin [str->pos] == 0 || mul ) return SUCCES;

            else return FAIL;
        case '}':
            regex->pos++;
            return SUCCES;
        case ')':
            if ( expre != NULL ) {

                for ( size_t i = 0; i < count; ++i ) free ( expressions [i] );
            }
            regex->pos++;
            return SUCCES;
        case '(':
            regex->pos++;

            if ( regex->begin [regex->pos] == 0 ) break; // special case tested
            
            /* Helper variables */
            size_t rsave = regex->pos, save = str->pos, end;

            int c = match ( regex, str , NULL, 0, mul );
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
                size_t e = str->pos - save;
                expressions [count] = malloc ( sizeof ( char ) * (e + 1) );
                strncpy ( expressions [count], &str->begin [save], e );
                expressions [count] [e] = 0;
                count++;
            }
            if ( regex->begin [regex->pos] == '+' || regex->begin [regex->pos] == '*') {
    
                regex->pos = rsave;
                
                for ( ; match ( regex, str, NULL, 0, false ) == SUCCES; regex->pos = rsave ) continue;
                
                regex->pos = end + 2;
                break;
            } else if ( regex->begin [regex->pos] == '{' ) {
                
                a = getszRange ( regex );
                end = regex->pos;
                if ( a.state == ERROR ) return ERROR;

                a.max--;
                regex->pos = rsave;

                size_t i = 1;
                size_t v = 0, n = 0;
                for ( n = str->pos;( v = match ( regex, str, NULL, 0, mul ) )== SUCCES && i < a.max; regex->pos = rsave, ++i, n = str->pos ) continue;
                
                if ( v != SUCCES ) str->pos = n;
                regex->pos = end;
                if ( i < a.min ) return FAIL;

                break;
            } else if ( regex->begin [regex->pos] == '?' ) regex->pos++;

            break;
        case '|':
            //free expressions
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
                    
                    size_t len = p->end - p->begin;
                    char * buff = malloc ( len + 1 );
                    
                    strncpy ( buff, &str->begin [str->pos], len );
                    buff [len] = 0;

                    regex_t * st = regexInit ( buff );

                    if ( (n = match ( p, st, expressions, count, false )) == SUCCES ) {

                        regexFree ( st );
                        regexFree ( p );
                        str->pos += len + 1;
                        break;
                    }
                    else  {

                        regexFree ( st );
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
                
                } else if ( regex->begin [regex->pos + 1] == '?' )  regex->pos += 1;
                else if ( regex->begin [regex->pos + 1 ] == '*' || regex->begin [regex->pos + 1] == '+' ) {

                    for ( ; isInrange ( range, str->begin [str->pos] ); str->pos++ ) continue;
                    regex->pos += 1;

                } else {
                    str->pos++;
                }

                regex->pos++;
                break;
            } else if ( negate && !inR ) {
                
                if ( srange ) {

                    size_t i = 0;
                    for ( ; !isInrange ( range, str->begin [str->pos] ) && i < a.max; str->pos++, ++i ) continue;
                    if ( i < a.min ) return FAIL;
                }

                if ( regex->begin [regex->pos + 1] == '?' ) {
                    regex->pos++;
                } else if ( regex->begin [regex->pos + 1] == '*' || regex->begin [regex->pos + 1] == '+' ) {

                    for ( ; !inR && str->begin [str->pos] != 0 ; str->pos++ ) inR = isInrange ( range, str->begin [str->pos] );

                    regex->pos++;
                    //str->pos--; // the above loop step an additional char in the string so we remove it
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
            } else if ( regex->begin [regex->pos - 1] == '\n' ) {
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

            if ( regex->begin [regex->pos] == '*' || regex->begin [regex->pos] == '+' ) {

                regex->pos++;
                if ( regex->begin [regex->pos] == 0 || regex->begin [regex->pos] == '\n' ) return SUCCES;
                
                if ( regex->begin [regex->pos] == '\\' ) regex->pos++;
                char stopChar = regex->begin [regex->pos];
                
                while ( str->begin [str->pos] != 0 || regex->begin [regex->pos] != '\n' ) {
                    
                    if ( str->begin [str->pos] == stopChar ) break;
                    str->pos++;
                }

                if ( regex->begin [regex->pos - 1] == '\\' ) regex->pos--;
            }
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
            if ( str->begin [str->pos] == 0 || str->begin [str->pos] == '\n' ) return SUCCES;

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
    return match ( regex, str, expressions, count, mul );
}

int Regex ( char * regex, char * str, char ** expre, size_t sz, unsigned char mode ) {

    if ( mode & NoWhite ) {
        clearifiy ( str );
        clearifiy ( regex );
    }

    if ( mode & NoCase ) {
        strToLower ( str );
        strToLower ( regex );
    }

    regex_t * a = regexInit ( regex );
    regex_t * b = regexInit ( str );

    int c = match ( a, b, expre, sz, mode & MultiLine );

    regexFree ( a );
    regexFree ( b );

    return c;
}


void regexFree ( regex_t * regex ) {

    regex->begin = NULL;
    regex->end = NULL;
    free ( regex );
    return;
}
