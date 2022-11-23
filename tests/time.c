/***
 * This is an example of using Mregex
 * The following code matches anythe time like "11-13-29" in 12 format or the 24 format
 * HOW TO USE:
 * time <STR> <FORMAT-optional>
 * to specify the 24f you need to set the third argument to any number instead 1
*/
#include <Mregex.h>
#include <stdio.h>

int main ( int argc, char ** argv ) {

    int format; // if format = 1 that's mean use the 12h format else use 24h format
    if ( argc < 2 ) {

        return (-1);
    }

    if ( argc < 3 ) {

        // this mean use the 12h formate
        format = 1;
    } else {

        format = (argv [2] [0]) ^ 0x30;
    }


    char _12F [] = "(1[0-2]|0[0-9]) [./-] [0-5][0-9] [./-] [0-5][0-9]";
    char _24F [] = "(2[0-3]|[01][0-9]) [./-] [0-5][0-9] [./-] [0-5][0-9]";
    
    int stat = 0;
    if ( format == 1 ) {
        stat = Regex ( _12F, argv [1], NULL, NoWhite );
    } else {
        stat = Regex ( _24F, argv [1], NULL, NoWhite );
    }

    printf ( "%d\n", stat );

    return 0;
}