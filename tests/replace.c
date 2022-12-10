/*
 * The following program fix the miss-spelling of 'the' or 'this' or 'that'
 * Usage: misspel <str> 
 * where <str> not longer than 4 character long
 */

#include "Mregex.h"
#include <stdio.h>

int main ( int argc, char ** argv ) {

    if ( argc < 2 ) {

        fprintf ( stderr, "Usage: %s <str>\n", argv [0] );
        return (-1);
    }

    char * st = NULL;

    char rgx [] = "@{[the][the][the],the}|@{[tha][tha][tha][tha],that}|@{[this]+,this}";
    Regex ( rgx, argv [1], &st, 0 );
    printf ( "%s\n", st );

    return 0;
}