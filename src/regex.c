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
#include "Mregex.h"
#include "engine.h"
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// initialize our regular expression
regex_t * regexInit ( char * str ) {

    regex_t * t = malloc ( sizeof ( regex_t ) );
    t ->begin = str;
    t->pos = 0;
    t->end = str + strlen ( str );
    return t;
}

/// @brief match the regex with str recursively
/// @param regex the regular expression string
/// @param str the string to be matched
/// @param expre the strings matched by the sub-expressions
/// @param kcount the count of expre
/// @param mul if multi-line mode enabled
/// @return SUCCES, Faile, or ERROR
int match ( regex_t * regex, regex_t * str, char ** expre , size_t kcount, bool mul ) {

    // set up the expression before start
    char * expressions [10] = { NULL };
    if ( expre != NULL ) {

        for ( size_t i = 0; i < kcount; ++i ) expressions [i] = expre [i];
    }

    // set up the neded variable
    szRange a;
    size_t count = kcount;
    bool negate = false;
    int _c = 0;
    size_t min = 1; // if the minimum is zero in '{min,max}'
    bool srange = false;

    switch ( regex->begin [ regex->pos ] ) {

        case 0:
            // free all the expressions
            if ( expre != NULL ) {

                for ( size_t i = 0; i < count; ++i ) free ( expressions [i] );
            }
            // if multi line enabled isn't necessary for str to end
            if ( str->begin [str->pos] == 0 || mul ) return SUCCES;
            else return FAIL;
        // not yet used
        case '}':
            regex->pos++;
            return SUCCES;
        // the end of the current expression
        case ')':

            // free all the expressions
            if ( expre != NULL ) {

                for ( size_t i = 0; i < count; ++i ) free ( expressions [i] );
            }
            regex->pos++;
            return SUCCES;
        // the beginning of the sub-expression
        case '(':
            regex->pos++;

            if ( regex->begin [regex->pos] == 0 ) break; // special case tested
            
            /* Helper variables */
            size_t rsave = regex->pos, save = str->pos, end;

            // start matching the sub-expression
            int c = match ( regex, str , NULL, 0, false );
            end = regex->pos - 1;

            if ( regex->begin [regex->pos - 1] != ')' ) return ERROR; // special case

            if ( c == ERROR ) return ERROR;
            if ( c == FAIL ) {

                // seak to the end if failed to test if it's optional or not
                if ( !seakToEnd ( regex )) return ERROR;
                str->pos = save;
                if ( regex->begin [regex->pos] == '?' || regex->begin [regex->pos] == '*' ) {
                    regex->pos++;
                    break;
                } else if ( seaktoPipe ( regex ) == FAIL ) { // seak to pipe if failed then the regex failed
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
            // match it again as many times as possible
            if ( regex->begin [regex->pos] == '+' || regex->begin [regex->pos] == '*') {
    
                regex->pos = rsave;
                
                for ( ; match ( regex, str, NULL, 0, false ) == SUCCES; regex->pos = rsave ) continue;
                
                regex->pos = end + 2;
                break;
            } else if ( regex->begin [regex->pos] == '{' ) { // or as many time specified
                
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
            if ( expre != NULL ) {

                for ( size_t i = 0; i < count; ++i ) free ( expressions [i] );
            }

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

        // character range            
        case '[':

            // test if there is a negate
            if ( regex->begin [regex->pos + 1] == '^' ) {
                regex->pos++;
                negate = true;
            }

            range_t * range = getrange ( regex ); // get the range
            bool inR = isInrange ( range, str->begin [str->pos] ); // test if it's in range

            if ( regex->begin [regex->pos + 1] == '{' ) {

                regex->pos++;
                a = getszRange ( regex );
                
                if ( a.state == ERROR ) return ERROR;

                min = a.min;
                srange = true;
            }

            // if it's in range and no negate
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

                freeRange ( range );
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
                } else {
                    str->pos++;
                }
                regex->pos++;
                freeRange ( range );
                break;
            } else if ( regex->begin [regex->pos + 1] == '*' || regex->begin [regex->pos + 1] == '?' || min == 0 ) {
                regex->pos += 2;
                freeRange ( range );
                break;
            }
            if ( seaktoPipe ( regex ) == FAIL ) return FAIL;
            regex->pos++;
            str->pos = 0;

            break;
        case '^':
            // match the beginning of the string or the beginning of the line on multi-line mode
            if ( regex->begin [regex->pos] == '^' && &str->begin [str->pos] == str->begin ) {
                regex->pos++;
                break;
            } else if ( regex->begin [regex->pos - 1] == '\n' && mul ) {
                regex->pos++;
                break;
            }
            if ( seaktoPipe ( regex ) == FAIL ) return FAIL;
            regex->pos++;
            str->pos = 0;
            break;
        case '.':

            // match any character
            _c = str->begin [str->pos];

            if ( _c == 0 || _c == '\n') return FAIL;
            if ( !isascii ( _c ) ) return FAIL;

            regex->pos++;
            str->pos++;

            // the '.*' case
            /* it's simply take the next character as the stop character */
            if ( regex->begin [regex->pos] == '*' || regex->begin [regex->pos] == '+' ) {

                regex->pos++;
                if ( regex->begin [regex->pos] == 0 ) return SUCCES;
                
                if ( regex->begin [regex->pos] == '\\' ) regex->pos++;
                char stopChar = regex->begin [regex->pos];
                
                while ( str->begin [str->pos] != 0 && regex->begin [regex->pos] != '\n' ) {
                    
                    if ( str->begin [str->pos] == stopChar ) {

                        size_t ssave = str->pos;
                        size_t rsave = regex->pos;
                        int rc = match ( regex, str, expressions, count, mul);

                        if ( rc == ERROR ) return ERROR;
                        if ( rc == SUCCES ) return SUCCES;

                        str->pos = ssave;
                        regex->pos = rsave;
                    }
                    str->pos++;
                }

            }
            break;
        case '*':
            // match as many times as possible
            if ( *regex->begin == '*' ) return FAIL;
            for ( ; str->begin [str->pos] == regex->begin [regex->pos - 1]; str->pos++ ) continue;
            regex->pos++;
            break;

        case '+':
            // match as many times as possible
            if ( *regex->begin == '+' ) return FAIL;

            for ( ; str->begin [str->pos] == regex->begin [regex->pos - 1]; str->pos++ ) continue;
            regex->pos++;
            break;
        case '$':
            // match the end of the string or the end of the line in multi-line mode
            if ( str->begin [str->pos] == 0 || (str->begin [str->pos] == '\n' && mul) ) return SUCCES;

            if ( seaktoPipe ( regex ) == FAIL ) return FAIL;
            regex->pos++;
            str->pos = 0;
            break;
        default :

            // match the specified charcter
            if ( regex->begin [regex->pos + 1] == '{' ) {

                char d;
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
    // re-run 
    return match ( regex, str, expressions, count, mul );
}

/// @brief the main function
/// @param regex the regular expression string
/// @param str the string to match
/// @param expre the final string ( the feature is under-development )
/// @param mode the modes 
/// @return SUCCES, FAILED, or ERROR
int Regex ( char * regex, char * str, char ** expre, unsigned char mode ) {

    if ( mode & NoWhite ) {
        clearifiy ( str, mode & MultiLine );
        clearifiy ( regex, mode & MultiLine );
    }

    if ( mode & NoCase ) {
        strToLower ( str );
        strToLower ( regex );
    }

    regex_t * a = regexInit ( regex );
    regex_t * b = regexInit ( str );

    int c = match ( a, b, NULL, 0, mode & MultiLine );

    regexFree ( a );
    regexFree ( b );

    return c;
}

// destroy the regex
void regexFree ( regex_t * regex ) {

    regex->begin = NULL;
    regex->end = NULL;
    free ( regex );
    return;
}
