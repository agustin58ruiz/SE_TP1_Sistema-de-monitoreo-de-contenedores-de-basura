#ifndef UART__H
#define UART__H

#include "mbed.h"

class Uart {
private:
BufferedSerial* _serial;
public:
Uart( PinName tx,
      PinName rx,
      int baud = MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE);
~Uart();
void WriteString(const char * str);
void ReadString(char* str);
bool Readable();
ssize_t Read( void* buffer,size_t length );
bool ReadChar( char* c);
void SetBlocking( bool opt );

};

#endif 