#include "uart.h"

Uart::Uart(PinName tx, PinName rx, int baud) {
    _serial = new BufferedSerial(tx, rx, baud);
}

Uart::~Uart() {
    delete _serial;
    _serial = nullptr;
}

void Uart::ReadString( char* str ) {
    str[0]='\0';
    
    bool readable = _serial->readable();
    char c='\0';

    for( int i = 0; readable; i++ ){
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
    _serial->write(str, strlen(str));
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