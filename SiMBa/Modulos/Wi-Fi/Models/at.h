#ifndef AT__H
#define AT__H

#include "uart.h"
#include "wi-fi-types.h"
#include "selector.h"


ParseStatus wait_for_at_echo( Uart& serial, const char * command, int commandLength );
ParseStatus wait_for_at_echo_async( Uart& serial, int & ,const char * command);
ParseStatus wait_for_string( Uart& serial, const char * str, int strLength );
ParseStatus wait_for_string_async( Uart& serial, int & strIndex, const char * str );

ParseStatus parse_integer_number( Uart& serial, int * numberOut , char separator=',');
ParseStatus parse_integer_number_async( Uart& serial, int &numberOut , char separator=',');
ParseStatus parse_string( Uart& serial, char * strOut, int bufsize );
ParseStatus parse_string_async( Uart& serial, int & strIndex, char * strOut, size_t maxStringOut );
ParseStatus parse_string_option ( Uart& serial, const char** options, int optionsNumber, int* optIndex );
bool flush_last_command_response( Uart& serial );
ParseStatus wait_for_char( Uart &serial, char c );

ParseStatus wait_for_option_async( Uart &serial, Selector * selector);

ParseStatus readline_async(Uart &serial, int & strIndex, char * stringOut, size_t maxStringOut);

#endif
