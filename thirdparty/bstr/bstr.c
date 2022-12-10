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

#include "bstr.h"
#include <string.h>

static inline bool bstrIsDigit ( char _C ) {

    if ( _C >= '0' && _C <= '9' ) return true;
    return false;
}
static inline bool bstrIsBlank ( char _C ) {

    if ( _C == ' ' || _C == '\t' || _C == '\r' || _C =='\n' ) {
        return true;
    }

    return false;
}

bstr * bstrCreate ( const char * str ) {

    bstr * s = malloc ( sizeof ( bstr ) );
    if ( s == NULL ) return NULL;

    // lenght
    for ( s->len = 0; str [s->len] != 0; ++(s->len) ) continue;

    s->data = malloc ( sizeof ( char ) * (s->len + 1) );
    if ( s->data == NULL ) {

        free (s);
        return NULL;
    }

    strncpy ( s->data, str, (s->len + 1) );

    return s;
}

void bstrDestroy ( bstr * s ) {

    if ( s ) {

        if ( s->data ) free ( s->data );

        free ( s );
    }

    return;
}

char bstrSGetChar ( bstr * str, size_t pos, char fail ) {

    if ( str == NULL ) return fail;
    if ( pos > str->len ) return fail;

    return str->data [pos];
}

char bstrGetChar ( bstr * str, size_t pos ) {

    return bstrSGetChar ( str, pos, 0 );
}

void bstrCatC ( bstr * str, const char * str2 ) {

    if ( str == NULL || str2 == NULL ) return;
    bstr * s = bstrCreate ( str2 );
    bstrCat ( str, s );
    bstrDestroy ( s );

    return;
}

void bstrCat ( bstr * str, bstr * str2 ) {

    if ( str == NULL ) return;
    if ( str2 == NULL ) return;

    size_t nlen = str->len + str2->len;

    str->data = realloc ( str->data, sizeof ( char ) * ( nlen + 1 ) );
    if ( str->data == NULL ) return;

    strncpy ( &str->data [ str->len ], str2->data, (str2->len) );

    str->data [nlen] = 0;
    str->len = nlen;

    return;
}

int bstrCmp ( bstr * s1, bstr * s2 ) {

    if ( s1 == NULL || s2 == NULL ) return 555;
    return strcmp ( s1->data, s2->data );
}

bstr * bstrCopy ( bstr * str ) {

    if ( str == NULL ) return NULL;
    bstr * s = malloc ( sizeof ( bstr ) );
    if ( s == NULL ) return NULL;

    s->data = malloc ( sizeof ( char ) * (str->len + 1) );
    if ( s->data == NULL ) {
        free (s);
        return NULL;
    }

    s->len = str->len;

    strncpy ( s->data, str->data, s->len );

    s->data [str->len] = 0;
    return s;
}

Lbstr * bstrSplitp ( bstr * s, size_t pos ) {

    if ( s == NULL ) return NULL;
    if ( pos == 0 || pos >= s->len ) return NULL;

    Lbstr * list = malloc ( sizeof ( Lbstr ) );
    if ( list == NULL ) return NULL;

    list->len = 2;
    list->list = malloc ( sizeof ( bstr * ) * 2 );
    
    if ( list->list == NULL ) {
        
        free ( list );
        return NULL;
    }

    size_t sLen = s->len;

    s->len = pos;
    bstr * str1 = bstrCopy ( s );

    if ( str1 == NULL ) {

        free ( list->list );
        free ( list );
    }

    list->list [0] = str1;

    s->len = sLen;

    str1 = malloc ( sizeof ( bstr ) );
    if ( str1 == NULL ) {
        
        free ( list->list [0] );
        free ( list->list );
        free ( list );
        return NULL;
    }

    str1->len = sLen - (pos);

    str1->data = malloc ( sizeof ( char ) * (str1->len + 1) );
    if ( str1->data == NULL ) {

        free (str1);
        free ( list->list [0] );
        free ( list->list );
        free ( list );
        return NULL;
    }

    strncpy ( str1->data, &s->data [pos], str1->len);
    str1->data [str1->len];

    list->list [1] = str1;

    return list;
}

void LbstrDestroy ( Lbstr * l ) {

    if ( l ) {

        if ( l->list ) {

            for ( size_t i = 0; i < l->len; ++i ) {

                bstrDestroy ( l->list [i] );
            }

            free ( l->list );
        }

        free ( l );
    }

    return;
}

int64_t bstrFind ( bstr * str, bstr * tofind ) {

    if ( str == NULL || tofind == NULL ) return (-2);

    if ( tofind->len > str->len ) return (-1);

    bool found = true;
    for ( size_t i = 0; i < str->len; ++i ) {
        
        found = true;
        size_t save = i;
        for ( size_t j = 0; j < tofind->len && i < str->len; ++j, ++i ) {

            if ( tofind->data [j] != str->data [i] ) {

                found = false;
                break;
            }
        }

        if ( found == true ) return ( int64_t ) save;

        i = save;
    }

    return (-1);
}

int64_t bstrFindIndex ( bstr * str, bstr * tofind, size_t index ) {

    if ( str == NULL || tofind == NULL ) return (-2);

    if ( tofind->len > str->len ) return (-1);

    size_t curIndex = 0;
    bool found = true;
    for ( size_t i = 0; i < str->len; ++i ) {
        
        found = true;
        size_t save = i;
        for ( size_t j = 0; j < tofind->len && i < str->len; ++j, ++i ) {

            if ( tofind->data [j] != str->data [i] ) {

                found = false;
                break;
            }
        }

        if ( found == true && curIndex == index ) return ( int64_t ) save;
        if ( found ) curIndex++;

        i = save;
    }

    return (-1);
}

Lbstr * bstrSplitIndex ( bstr * s, bstr * b, size_t index ) {

    if ( s == NULL || b == NULL ) return NULL;
    if ( b->len >= s->len ) return NULL;

    int64_t begin = bstrFindIndex ( s, b, index );
    if ( begin == -1 ) return NULL;
    
    size_t end = begin + b->len;

    Lbstr * l = bstrSplitp ( s, begin );
    Lbstr * l2 = bstrSplitp ( s, end );

    if ( begin == 0 ) {

        bstrDestroy (l2->list [0]);

        l2->list [0] = l2->list [1];
        l2->len = 1;
        return l2;
    } else if ( end == s->len ) {
        if ( l )
        bstrDestroy (l->list [1]);

        l->len = 1;
        return l;
    } else {
        if ( l ) bstrDestroy (l->list [1]);
        if ( l2 ) bstrDestroy (l2->list [0]);

        l->list [1] = l2->list [1];

        free ( l2 );

        return l;
    }
}

size_t bstrCount ( bstr * s, bstr * b ) {

    
    if ( s == NULL || b == NULL ) return (0);

    if ( b->len > s->len ) return (0);

    size_t curIndex = 0;
    bool found = true;
    for ( size_t i = 0; i < s->len; ++i ) {
        
        found = true;
        size_t save = i;
        for ( size_t j = 0; j < b->len && i < s->len; ++j, ++i ) {

            if ( b->data [j] != s->data [i] ) {

                found = false;
                break;
            }
        }

        if ( found ) curIndex++;

        i = save;
    }

    return curIndex;
}

Lbstr * bstrSplit ( bstr * str, bstr * b ) {

    if ( str == NULL || b == NULL ) return NULL;
    size_t count = bstrCount ( str, b );
    if ( count == 0 ) return NULL;

    Lbstr * list = malloc ( sizeof ( Lbstr ) );
    Lbstr * tmp;
    list->list = malloc ( sizeof ( bstr * ) * count * 3 / 2 );
    list->len = 0;
    bstr * stemp = str;

    for ( size_t i = 0; i < count; ++i ) {

        tmp = bstrSplitIndex ( stemp, b, 0 );
        if ( tmp != NULL ) {

            list->list [list->len] = tmp->list [0];
            list->len++;
            
            if ( i == (count - 1) && tmp->len == 1 ) {
                stemp = NULL;
            } else if ( tmp->len == 1 ) {

                list->len--;
                stemp = tmp->list [0];
            } else {

                stemp = tmp->list [1];
            }

            free ( tmp->list );
            free ( tmp );   
        }
    }

    if ( stemp != NULL ) {
        list->list [list->len] = stemp;
        list->len ++;
    }
    return list;
}

bstr * bstrToLower ( bstr * str ) {

    if ( str == NULL ) return NULL;

    for ( size_t i = 0; i < str->len; ++i ) {

        if ( str->data [i] >= 'A' && str->data [i] <= 'Z' ) {
            
            str->data [i] += 32;
        }
    }

    return str;
}

bstr * bstrToUpper ( bstr * str ) {

    if ( str == NULL ) return NULL;


    for ( size_t i = 0; i < str->len; ++i ) {

        if ( str->data [i] >= 'a' && str->data [i] <= 'z' ) {
            
            str->data [i] -= 32;
        }
    }

    return str;
}

bstr * bstrClearify ( bstr * str ) {

    if ( str == NULL ) return NULL;


    size_t i = 0, d = 0;

    for ( ; d < str->len; ++d, ++i ) {

        while ( bstrIsBlank (str->data [d] ) ) {
            ++d;
        }

        str->data [i] = str->data [d];
    }

    str->len = i;
    str->data [str->len] = 0;
    return str;
}

bstr * bstrReplace ( bstr * str, bstr * what, bstr * with ) {

    if ( str == NULL || what == NULL || with == NULL ) return NULL;
    bstr * s;
    Lbstr * list = bstrSplit ( str, what );
    if ( list == NULL ) return str;

    size_t count = bstrCount ( str, what );
    size_t i = 0;
    size_t end = bstrFindIndex ( str, what, count - 1 ) + what->len;

    if ( bstrFindIndex ( str, what, 0 ) == 0 ) {

        s = bstrCopy ( with );
    } else {

        s = bstrCopy ( list->list [0] );
        bstrCat ( s, with );
    
        ++i;
    }

    for ( ; i < list->len; ++i ) {

        bstrCat ( s, list->list [i] );
        if ( i != list->len - 1 ) {
            bstrCat ( s, with );
        }
    }

    if ( end == str->len ) {

        bstrCat ( s, with );
    }

    str->len = s->len;
    free (str->data);

    str->data = s->data;
    free ( s );
    LbstrDestroy ( list );
    
    return str;
}

bstr * bstrReplaceIndex ( bstr * str, bstr * what, bstr * with, size_t Index ) {

    if ( str == NULL || what == NULL || with == NULL ) return NULL;
    if ( what->len > str->len ) return NULL;
    
    size_t count = bstrCount ( str, what );
    if ( count == 0 ) return NULL;
    if ( Index >= count ) return NULL;

    Lbstr * list = bstrSplitIndex ( str, what, Index );
    if ( list == NULL ) return NULL;

    bstr * tmp;
    size_t begin = bstrFindIndex ( str, what , Index ), end = begin + what->len;
    if ( begin == 0 ) {

        tmp = bstrCopy ( with );
        bstrCat ( tmp, list->list [0] );
    } else if ( end == str->len ) {

        tmp = bstrCopy ( list->list [0] );
        bstrCat ( tmp, with );
    } else {

        tmp = bstrCopy ( list->list [0] );
        bstrCat ( tmp, with );
        bstrCat ( tmp, list->list [1] );
    }

    LbstrDestroy ( list );
    str->len = tmp->len;

    free ( str->data );
    str->data = tmp->data;
    
    free ( tmp );
    return str;
}

bstr * bstrReverse ( bstr * str ) {

    if ( str == NULL ) return NULL;

    bstr * s = bstrCopy ( str );

    for ( size_t i = 0, j = str->len - 1; i < str->len; ++i, --j ) {

        s->data [i] = str->data [j];
    }

    free ( str->data );
    str->len = s->len;

    str->data = s->data;
    free ( s );

    return str;
}

bstr * bstrRemove ( bstr * str, bstr * toRemove ) {

    if ( str == NULL || toRemove == NULL ) return NULL;
    if ( toRemove->len > str->len ) return NULL;

    Lbstr * list = bstrSplit ( str, toRemove );

    if ( list == NULL ) return NULL;

    bstr * s = bstrCopy ( list->list [0] );

    for ( size_t i = 1; i < list->len; ++i ) {

        bstrCat ( s, list->list [i] );
    }

    LbstrDestroy ( list );

    free ( str->data );
    str->len = s->len;
    
    str->data = s->data;
    free ( s );

    return str;
}

bstr * bstrRemoveIndex ( bstr * str, bstr * toRemove, size_t index ) {

    if ( str == NULL || toRemove == NULL ) return NULL;
    if ( toRemove->len > str->len ) return NULL;

    Lbstr * list = bstrSplitIndex ( str, toRemove, index );
    if ( list == NULL ) return NULL;

    bstr * s = bstrCopy (  list->list [0] );

    if ( list->len == 2 ) {
        bstrCat ( s, list->list [1] );
    }

    LbstrDestroy ( list );

    str->len = s->len;
    free ( str->data );
    str->data = s->data;

    free ( s );
    return str;
}

bstr * bstrReplacep ( bstr * str, size_t begin, size_t end, bstr * what ) {

    if ( str == NULL || what == NULL ) return NULL;
    if ( end > begin ) return NULL;

    if ( begin > str->len || end > str->len || what->len > str->len ) return NULL;

    Lbstr * ls;
    bstr * s;

    if ( begin == end ) {

        ls = bstrSplitp ( str, begin );

        s = bstrCopy ( ls->list [0] );
        bstrCat ( s, what );
        if ( ls->len == 2 ) {
            bstrCat ( s, ls->list [1] );
        }
    } else if ( begin == 0 && end == str->len ) {

        return bstrCopy ( what );
    } else if ( begin == 0 ) {

        s = bstrCopy ( what );
        ls = bstrSplitp ( str, end );

        bstrCat ( s, ls->list [1] );
    } else if ( end == str->len ) {

        ls = bstrSplitp ( str, begin );
        
        s = bstrCopy ( ls->list [0] );
        bstrCat ( s, what );
    } else {

        ls = bstrSplitp ( str, begin );
        
        s = bstrCopy( ls->list [0] );
        
        bstrCat ( s, what );
        LbstrDestroy ( ls );

        ls = bstrSplitp ( str, end );
        bstrCat ( s, ls->list [1] );
    }

    LbstrDestroy ( ls );

    str->len = s->len;
    free ( str->data );

    str->data = s->data;

    free ( s );

    return str;
}

bstr * bstrAppend ( bstr * str, bstr * toAppend, size_t pos ) {

    return bstrReplacep ( str, pos, pos, toAppend );
}

bool bstrIsNum ( bstr * str ) {

    if ( str == NULL ) return false;

    for ( size_t i = 0; i < str->len; ++i ) {

        if ( !bstrIsDigit ( bstrGetChar ( str, i ) ) ) return false;
    }

    return true;
}

bool bstrIsAllUpper ( bstr * str ) {

    if ( str == NULL ) return false;
    bstr * s = bstrClearify ( str );

    bool is = true;
    for ( size_t i = 0; i < s->len; ++i ) {

        if ( !( i <='Z' && i >= 'A') ) {
            is = false;
            break;
        }
    }
    bstrDestroy ( s );

    return is;
}

bool bstrIsAllLower ( bstr * str ) {

    if ( str == NULL ) return false;
    bstr * s = bstrClearify ( str );

    bool is = true;
    for ( size_t i = 0; i < s->len; ++i ) {

        if ( !( i <='z' && i >= 'a') ) {
            is = false;
            break;
        }
    }
    bstrDestroy ( s );

    return is;
}

bool bstrBeginsWith ( bstr * str, bstr * with ) {

    if ( str == NULL || with == NULL ) return false;
    if ( with->len > str->len ) return false;
    for ( size_t i = 0; i < with->len; ++i ) {

        if ( with->data [i] != str->data [i] ) return false;
    }

    return true;
}

bool bstrEndsWith ( bstr * str, bstr * with ) {

    if ( str == NULL || with == NULL ) return false;

    if ( with->len > str->len ) return false;

    for ( int64_t i = with->len - 1, j = str->len - 1; i >= 0; --i, --j ) {

        if ( str->data [j] != with->data [i] ) return false;
    }

    return true;
}

bool bstrIsFPNum ( bstr * str ) {

    if ( str == NULL ) return false;

    bool found = false;
    for ( size_t i = 0; i < str->len ; ++i ) {

        if ( str->data [i] == '.' && found ) return false;
        else if ( str->data [i] == '.' ) found = true;
        else if ( !bstrIsDigit ( str->data [i] ) ) return false;
    }

    return true;
}

bstr * bstrCaptalize ( bstr * str ) {

    if ( str == NULL ) return NULL;

    bstr * s = bstrToLower (str);

    if ( s->data [0] >= 'a' && s->data [0] <= 'z' ) {

        s->data [0] -= 32;
    }

    for ( size_t i = 1; i < s->len; ++i ) {

        if ( s->data [i] >= 'a' && s->data [i] <= 'z' && bstrIsBlank ( s->data [i - 1] ) ) {
            
            s->data [i] -= 32;
        }
    }

    return str;
}