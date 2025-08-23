#include "at.h"
#include "mbed.h"
#include "wi-fi-types.h"


/**
 * @brief Waits for an "AT+" prefix followed by a specific command echo from a UART interface.
 *
 * This function reads characters from a UART stream and waits for the modem or device 
 * to echo the "AT+" sequence, followed by a specific command string. It performs a two-step match:
 * 
 * 1. Waits for the standard "AT+" prefix (can recover even if previous garbage characters are present).
 * 2. After detecting "AT+", it attempts to match the subsequent characters with the expected command.
 *
 * For example, if the expected command is "GMR", the function looks for "AT+GMR" in the serial input.
 *
 * @param serial         A reference to a `Uart` object that provides the `Readable()` and `Read()` methods.
 * @param command        A pointer to a null-terminated C-style string representing the command to match after "AT+".
 * @param commandLength  The length of the `command` string to match (does not rely on null-termination).
 *
 * @return ParseStatus::FAILED_NO_READ     If the serial interface is not readable or a read fails.
 * @return ParseStatus::SUCCESSFUL_NO_MATCH If "AT+" is found but the command doesn't fully match.
 * @return ParseStatus::SUCCESSFUL_MATCH    If both "AT+" and the command match successfully.
 */
ParseStatus wait_for_at_echo( Uart& serial, const char * command, int commandLength ) {
    char c;
    int stateIndex = 0;
    ////printf("wait_for_at_echo\r\n");
    // Parsing AT+ "AAAAAT+" is correct for instance
    
    if ( !serial.Readable() ) {
        //printf("no readable\r\n");
        return ParseStatus::FAILED_NO_READ;
    }
    while ( serial.Readable() ) {
        if( serial.Read(&c, 1) != 1 ) { return ParseStatus::FAILED_NO_READ;}
        //printf("%c", c);
        if ( stateIndex < 3 && "AT+"[stateIndex]==c ){ stateIndex++; }
         
        else if ( 'A' == c ) { stateIndex = 1; }

        if ( stateIndex == 3 ) { break; }

    }
    while ( !serial.Readable() ) {}
    stateIndex = 0;

    while ( serial.Readable() ) {
        if( serial.Read(&c, 1) != 1 ) { return ParseStatus::FAILED_NO_READ; }
       // printf("%c\r\n", c);
        if ( stateIndex < commandLength && command[stateIndex]==c ) { stateIndex++; }
        else return ParseStatus::SUCCESSFUL_NO_MATCH;

        if ( stateIndex==commandLength ) { break; }
    }
    return ParseStatus::SUCCESSFUL_MATCH;

}


ParseStatus wait_for_at_echo_async( Uart& serial, int & strIndex, const char * command ) {
    char c;
    int partial = 0;    
    ParseStatus st;
    
    if ( strIndex < 3 ) {
        if ( !serial.Readable() ) { st = ParseStatus::NOT_READABLE; }
        else if( serial.Read(&c, 1) != 1 ) { st = ParseStatus::FAILED_NO_READ; } 
        else if ( "AT+"[strIndex]==c ){ strIndex++; st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH; }
        else { strIndex = 0; st = ParseStatus::SUCCESSFUL_NO_MATCH; }
        //printf("%c\r\n", c);
    } else {
        partial = strIndex - 3;
        if ( command[partial] == '\0' ) { st = ParseStatus::SUCCESSFUL_MATCH; }
        else if ( !serial.Readable() ) { st = ParseStatus::NOT_READABLE; }
        else if ( serial.Read(&c, 1) != 1 ) { st = ParseStatus::FAILED_NO_READ; }
        else if ( command[partial]==c ) {
            strIndex++;
            st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH;
        } else st = ParseStatus::SUCCESSFUL_NO_MATCH;

        
    }
    return st;
}




/**
 * @brief Waits for a specific string to appear on a UART interface.
 * 
 * This function reads characters from a UART interface and compares them sequentially 
 * to a target string (`str`). If the characters match in order up to the specified 
 * length (`strLength`), it returns `ParseStatus::SUCCESSFUL_MATCH`. If a character 
 * does not match the expected character in the string, it returns 
 * `ParseStatus::SUCCESSFUL_NO_MATCH`. If a character could not be read from UART, 
 * it returns `ParseStatus::FAILED_NO_READ`.
 *
 * @param serial     Reference to a `Uart` object used for reading characters.
 * @param str        Pointer to the target string to match against incoming data.
 * @param strLength  The number of characters from `str` to match.
 * 
 * @return ParseStatus
 *         - `SUCCESSFUL_MATCH` if the complete string is matched.
 *         - `SUCCESSFUL_NO_MATCH` if a mismatch occurs before the full string is matched.
 *         - `FAILED_NO_READ` if reading from the UART fails.
 */
ParseStatus wait_for_string( Uart& serial, const char * str, int strLength ) {
    char c;
    int strIndex=0; 

    while ( serial.Readable() ) {

        if ( serial.Read(&c, 1) != 1 ) { return ParseStatus::FAILED_NO_READ; }

        if ( strIndex < strLength && str[strIndex] == c ) { strIndex++;}
        else return ParseStatus::SUCCESSFUL_NO_MATCH;

        if ( strIndex == strLength ) { break; }
    }
    return ParseStatus::SUCCESSFUL_MATCH;
}

ParseStatus wait_for_string_async( Uart& serial, int & strIndex, const char * str ) {
    char c;
    ParseStatus st;
    
    if ( str[strIndex] == '\0' ) { strIndex = 0; st = ParseStatus::SUCCESSFUL_MATCH;  }
    else if ( !serial.Readable() ) { st = ParseStatus::NOT_READABLE; }
    else if ( serial.Read(&c, 1) != 1 ) { st = ParseStatus::FAILED_NO_READ; }
    else if ( str[strIndex] == c ) {  strIndex++; st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH; }
    else {
        
        st = ParseStatus::SUCCESSFUL_NO_MATCH;
        strIndex = 0;
    }
    
    return st;
}

ParseStatus parse_integer_number( Uart& serial, int * numberOut , char separator) {
    int number = 0;
    int int_c;
    char c;
    
    //printf("parse_integer\r\n");

    bool readable = serial.Readable();
    if ( !readable ) { return ParseStatus::FAILED_NO_READ; }
    
    while ( readable ) {
        if ( serial.Read(&c,1) != 1) { return ParseStatus::FAILED_NO_READ; }
        int_c = (int)(c - '0'); 
        //printf("%c\r\n",c);
        //printf("%d\r\n", int_c);
        if ( int_c <= 9 && int_c >=0 ) { number*=10; number+=int_c;  }
        else if ( c =='\r' ) { continue; } 
        else if ( c =='\n' || c == separator  ) { *numberOut = number; break; }
        else return ParseStatus::SUCCESSFUL_NO_MATCH;
        readable = serial.Readable();
    }
    return ParseStatus::SUCCESSFUL_MATCH;
}

ParseStatus parse_integer_number_async( Uart& serial, int &numberOut , char separator) {
    int int_c;
    char c;
    ParseStatus st;
    
    if ( !serial.Readable() ) { st = ParseStatus::NOT_READABLE; }
    else if ( serial.Read(&c, 1) != 1 ) { st = ParseStatus::FAILED_NO_READ; }
    else {

        if ( (int_c = (int)(c - '0')) <=9 && int_c >=0 ) { 
        
        numberOut*=10; 
        numberOut+=int_c; 
        st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH; 
    } 
    else if ( c =='\r' ) { st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH; } 
    else if ( c =='\n' || c == separator  ) { st = ParseStatus::SUCCESSFUL_MATCH; }
    else {st = ParseStatus::SUCCESSFUL_NO_MATCH;}
    }
    return st;
}

ParseStatus parse_string( Uart& serial, char * strOut, int bufsize ) {
    char c;
    int index = 0;
    
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

ParseStatus parse_string_async( Uart& serial, int & strIndex, char * strOut, size_t maxStringOut ) {
    ParseStatus st;
    char c;
    
    if ( !serial.Readable() ) { st = ParseStatus::NOT_READABLE; }   
    else if ( serial.Read(&c, 1) != 1 ) { st = ParseStatus::FAILED_NO_READ; } 
    else if ( strIndex == maxStringOut - 1 ) {
        strOut[strIndex] = '\0';
        if ( c == '"') { strIndex = 0; st = ParseStatus::SUCCESSFUL_MATCH; }
        else { strIndex = 0; st = ParseStatus::FULL_BUFFER; }

    } 
    else if ( c != '"' ) { strOut[strIndex++] = c; st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH;  } 
    else if ( strIndex != 0 ) {strOut[strIndex] = '\0'; strIndex = 0; st = ParseStatus::SUCCESSFUL_MATCH; }
    else { st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH; } 
    
    

    return st;
}

bool flush_last_command_response(Uart& serial) {
    
    char c;
    int strIndex = 0;

    
    while (serial.Readable()) {
        if (serial.Read(&c, 1) != 1) {
            return false;
        }
        
        
        
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
    
    fill(noSearch,noSearch+optionsNumber,true);
    


    if ( (st = wait_for_char(serial, '"')) != ParseStatus::SUCCESSFUL_MATCH ) { return st; }
    
    int charIndex = 0;
    while ( serial.Readable() ) {
        
        if ( serial.Read( &c, 1) != 1 ) { delete[] noSearch; return ParseStatus::FAILED_NO_READ; }

        for ( int option = 0; option < optionsNumber; option++ ) {
            if ( !noSearch[option] ) continue;

            if ( c == '"' &&  options[option][charIndex] == '\0' ) { *optIndex = option; return ParseStatus::SUCCESSFUL_MATCH; }

            if ( c != options[option][charIndex] ) { noSearch[option] = false; continue; }
            
            charIndex++;
        } 


    }

    return ParseStatus::FAILED_UNKNOWN_OPTION;

}

ParseStatus wait_for_option_async( Uart &serial, Selector * selector) {
    ParseStatus st;
    char c;
    
    if ( selector == nullptr ) { st = ParseStatus::FAILED_NO_SELECTOR_SET; } 
    else if ( !serial.Readable() ) { st = ParseStatus::NOT_READABLE; }
    else if( serial.Read(&c, 1) != 1 ) { st = ParseStatus::FAILED_NO_READ; } 
    else {

        selector->Update(c);
        if ( !selector->Finished()  ) { st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH; }
        else if ( selector->Result() == -1) { st = ParseStatus::SUCCESSFUL_NO_MATCH; }
        else st = ParseStatus::SUCCESSFUL_MATCH;
    }
    return st;
}

ParseStatus readline_async(Uart &serial, int & strIndex, char * stringOut, size_t maxStringOut) {
    ParseStatus st;
    char c;

    
    if ( !serial.Readable() ) { st = ParseStatus::NOT_READABLE; }   
    else if ( serial.Read(&c, 1) != 1 ) { st = ParseStatus::FAILED_NO_READ; } 
    else if ( (strIndex + 2) == maxStringOut ) { 
        stringOut[strIndex+1] = '\0';
        // last char
        if ( c != '\r' ) { 
            stringOut[strIndex] = c; 
            st = ParseStatus::FULL_BUFFER; 
            strIndex = 0;
        }
        else {
            stringOut[strIndex++] = '\0'; 
            st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH;
        }
    } 
    else if ( (c != '\r') && (c != '\n') ) { 
        stringOut[strIndex++] = c; 
        st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH; 
    } 
    else if ( c == '\r' ) {
        stringOut[strIndex++] = '\0'; 
        st = ParseStatus::SUCCESSFUL_PARTIAL_MATCH;         
    } 
    else { // c is '\n'
        strIndex = 0;
        st = ParseStatus::SUCCESSFUL_MATCH;
    }

    return st;
}