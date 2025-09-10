#include "miscellaneous.h"
#include "stdlib.h"

void parseLengthArray( const char * str, int* linksLenghArray ) {
    // str is "+CIPRECVLEN:0,439,0,0,0\n\r"
    
    if ( str == nullptr || linksLenghArray == nullptr ) {
        return; 
    }

    bool readable = true;
    int cursor = 0;
    for ( ; readable; cursor++ ) {
        if ( str[cursor] == ':' ) {
            readable = false;
        } 
    }

    char number[6] = "";
    int numberIndex = 0;
    int arrIndex = 0;

    for ( ; readable; cursor++) {
        
        switch ( str[cursor] ) {
            case '\r':
                readable = false;
                break;
            case ',':
                number[numberIndex] = '\0';
                numberIndex = 0;
                linksLenghArray[arrIndex++] = atoi(number);
                break;
            default:
                number[numberIndex++] = str[cursor];
                break;
            
        }
    }

}