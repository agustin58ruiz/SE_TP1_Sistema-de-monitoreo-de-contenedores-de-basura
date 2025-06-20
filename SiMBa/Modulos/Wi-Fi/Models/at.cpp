#include "at.h"
#include "mbed.h"
#include "wi-fi-types.h"

ParseStatus wait_for_at_echo( Uart& serial, const char * command, int commandLength ) {
    char c;
    int stateIndex = 0;
    printf("wait_for_at_echo\r\n");
    // Parsing AT+ "AAAAAT+" is correct for instance
    while ( serial.Readable() ) {
        if( serial.Read(&c, 1) != 1 ) { return ParseStatus::FAILED_NO_READ;}
        printf("%c\r\n", c);
        if ( stateIndex < 3 && "AT+"[stateIndex]==c ){ stateIndex++; }
         
        else if ( 'A' == c ) { stateIndex = 1; }

        if ( stateIndex == 3 ) { break; }

    }
    
    stateIndex = 0;

    while ( serial.Readable() ) {
        if( serial.Read(&c, 1) != 1 ) { return ParseStatus::FAILED_NO_READ; }
        printf("%c\r\n", c);
        if ( stateIndex < commandLength && command[stateIndex]==c ) { stateIndex++; }
        else return ParseStatus::SUCCESSFUL_NO_MATCH;

        if ( stateIndex==commandLength ) { break; }
    }
    return ParseStatus::SUCCESSFUL_MATCH;

}

ParseStatus wait_for_string( Uart& serial, const char * str, int strLength ) {
    char c;
    int strIndex=0; 
    printf("wait_for_string\r\n");

    while ( serial.Readable() ) {

        if ( serial.Read(&c, 1) != 1 ) { return ParseStatus::FAILED_NO_READ; }

        printf("%c\r\n", c);

        if ( strIndex < strLength && str[strIndex] == c ) { strIndex++;}
        else return ParseStatus::SUCCESSFUL_NO_MATCH;

        if ( strIndex == strLength ) { break; }
    }
    return ParseStatus::SUCCESSFUL_MATCH;
}

ParseStatus parse_integer_number( Uart& serial, int * numberOut ) {
    int number = 0;
    int int_c;
    char c;
    printf("parse_integer_number\r\n");
    while ( serial.Readable() ) {
        if ( serial.Read(&c,1) != 1) { return ParseStatus::FAILED_NO_READ; }
        int_c = (int)(c - '0'); 
        printf("%c\r\n", c);
        if ( int_c <= 9 && int_c >=0 ) { number*=10; number+=int_c;  }
        else if ( c =='\r' ) { continue; } 
        else if ( c =='\n' || c ==','  ) { *numberOut = number; break; }
        else return ParseStatus::SUCCESSFUL_NO_MATCH;
    }
    return ParseStatus::SUCCESSFUL_MATCH;
}

ParseStatus parse_string( Uart& serial, char * strOut, int bufsize ) {
    char c;
    int index;
    if ( serial.Readable() ) {
        if (serial.Read(&c, 1) != 1 ) { return ParseStatus::FAILED_NO_READ; }
        if ( c != '"') { return ParseStatus::SUCCESSFUL_NO_MATCH; }
    } else return ParseStatus::FAILED_NO_READ;

    while ( serial.Readable() ) {
        if (serial.Read(&c, 1) != 1 ) { return ParseStatus::FAILED_NO_READ; }
        
        if ( c == '"' ) { strOut[index] = '\0'; return ParseStatus::SUCCESSFUL_MATCH; }
        
        if ( index == (bufsize-1) ) { strOut[index] = '\0'; return ParseStatus::FULL_BUFFER; }
        
        strOut[index++] = c;
    }
    return ParseStatus::SUCCESSFUL_MATCH;

}

bool flush_last_command_response(Uart& serial) {
    // Finds OK\r\n
    char c;
    int strIndex = 0;

    printf("flush_last_command_respose\r\n");
    while (serial.Readable()) {
        if (serial.Read(&c, 1) != 1) {
            return false;
        }
        
        printf("%c\r\n", c);
        
        if (strIndex < 4 && "OK\r\n"[strIndex] == c) {
            strIndex++;
        } else {
            strIndex = 0; // Reset if sequence breaks
        }
        
        if (strIndex == 4) {
            break; // Stop reading after \n
        }
    }
    return true;
}

ParseStatus wait_for_char( Uart &serial, char c ) {
    
    char d;

    // true if char c is in serial
    if ( serial.Readable() ) {
        if ( serial.Read(&d, 1) != 1) { return ParseStatus::FAILED_NO_READ; }
        if ( d == c ) { return ParseStatus::SUCCESSFUL_MATCH; }

    }
    return ParseStatus::SUCCESSFUL_NO_MATCH;
}

ParseStatus parse_string_option ( Uart& serial, const char** options, int optionsNumber, int* optIndex ) {

    char c;
    int indexOptions=0;
    ParseStatus st;

    bool *noSearch = new bool[optionsNumber];
    
    fill(noSearch,noSearch+optionsNumber,false);
    


    if ( (st = wait_for_char(serial, '"')) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    
    while ( serial.Readable() ) {
        
        if ( serial.Read( &c, 1) != 1 ) { delete[] noSearch; return ParseStatus::FAILED_NO_READ; }
        
        for ( int i = 0; i < optionsNumber; i++ ) {
            if ( noSearch[i] ) continue;
            if ( c == '"' ) { 
                if ( options[i][optionsNumber] == '\0' ) {
                    *optIndex = i;
                    delete[] noSearch;
                    return ParseStatus::SUCCESSFUL_MATCH;
                } else {
                    delete[] noSearch;
                    return ParseStatus::SUCCESSFUL_NO_MATCH;
                }
            }
            
            if ( options[i][optionsNumber] == '\0' || options[i][optionsNumber] != c ) { noSearch[i] = true; }
        } 


    }

    return ParseStatus::SUCCESSFUL_MATCH;

}