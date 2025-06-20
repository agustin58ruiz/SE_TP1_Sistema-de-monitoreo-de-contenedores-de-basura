#ifndef AT__H
#define AT__H

#include "uart.h"
#include "wi-fi-types.h"


ParseStatus wait_for_at_echo( Uart& serial, const char * command, int commandLength );
ParseStatus wait_for_string( Uart& serial, const char * str, int strLength );
ParseStatus parse_integer_number( Uart& serial, int * numberOut );
ParseStatus parse_string( Uart& serial, char * strOut, int bufsize );
ParseStatus parse_string_option ( Uart& serial, const char** options, int optionsNumber, int* optIndex );
bool flush_last_command_response( Uart& serial );
ParseStatus wait_for_char( Uart &serial, char c );

#endif
