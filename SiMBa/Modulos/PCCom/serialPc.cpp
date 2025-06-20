
#include "serialPc.h"
#include "mbed.h"

static UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

SerialPc::SerialPc() {}

void SerialPc::Enviar( const char* mensaje ) {

    uartUsb.write( mensaje, strlen(mensaje) );

}

void SerialPc::Enviar( char c ) {

    uartUsb.write( &c, 1 );

}