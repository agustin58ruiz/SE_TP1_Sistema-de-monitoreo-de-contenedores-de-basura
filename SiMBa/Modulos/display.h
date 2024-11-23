#ifndef DISPLAY_H
#define DISPLAY_H

#include "mbed.h"
#include "chrono"

enum class  conexionDisplay_t{DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER};

class Display{
private:

conexionDisplay_t Conexion; 
void pinWrite( uint8_t pinName, int value );
void codeWrite( bool type, uint8_t dataBus );
void dataBusWrite( uint8_t dataBus );
void _countdownBacklightCallback();
bool _tickerActivated;
bool _backlight;
Ticker * _backlightTicker;
float _countdownSecs;

public:

Display();
void IniciarDisplay();
void CharPositionWrite( uint8_t charPositionX, uint8_t charPositionY );
void StringWrite( const char * );
void Flush();
void Backlight();
void EstablecerCountdown(float secs);
void ActivarCountdownBacklight();
void DesactivarCountdownBacklight();
void NoBacklight();



};

#endif