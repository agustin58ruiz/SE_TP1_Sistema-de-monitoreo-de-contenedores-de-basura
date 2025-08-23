#include "uart.h"

#define UART_WRITE_CHUNK 20

Uart::Uart(PinName tx, PinName rx, int baud) {
    _serial = new BufferedSerial(tx, rx, baud);
}

Uart::~Uart() {
    delete _serial;
    _serial = nullptr;
}

void Uart::ReadString( char* str , size_t length) {
    str[length-1]='\0';
    
    bool readable = _serial->readable();
    char c='\0';

    for( int i = 0; readable && i < (length-1); i++ ){
        _serial->read(&c, 1);

        if ( c != '\r' && c != '\n' ){
            str[i] = c;
        } else if ( c == '\n'  ){
            str[i] = '\0';
            
            break;
        }
        readable = _serial->readable();
    }
}

void Uart::WriteString(const char* str ) {
    const int chunkSize = UART_WRITE_CHUNK;
    int totalLength = strlen(str);
    int sent = 0;

    while (sent < totalLength) {
        int remaining = totalLength - sent;
        int lenToWrite = (remaining > chunkSize) ? chunkSize : remaining;

        int written = _serial->write(str + sent, lenToWrite);
        if (written > 0) {
            sent += written;
        }
    }
}

void Uart::WriteStringN(const char* data, int length) {
    const int chunkSize = UART_WRITE_CHUNK;
    int sent = 0;

    while (sent < length) {
        int remaining = length - sent;
        int lenToWrite = (remaining > chunkSize) ? chunkSize : remaining;

        int written = _serial->write(data + sent, lenToWrite);
        if (written > 0) {
            sent += written;
        }
    }
}

bool Uart::Readable() {
    return _serial->readable();
}

ssize_t Uart::Read( void* buffer,size_t length ) {
    return _serial->read( buffer, length );
}

bool Uart::ReadChar( char* c) {
    char local_c = '\0';
    if ( _serial->readable() ) {
        _serial->read( &local_c, 1 );
        *c = local_c;
        return true;
    }
    return false;

}
void Uart::SetBlocking( bool opt ) {
    _serial->set_blocking( opt );
}

int Uart::FlushInput(){
    int charsFlushed = 0;
    char c;

    while (_serial->readable()) {
        _serial->read( &c, 1 );
        charsFlushed++;
    }

    return charsFlushed;
}