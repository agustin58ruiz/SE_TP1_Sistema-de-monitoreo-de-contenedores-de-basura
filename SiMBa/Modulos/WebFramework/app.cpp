#include "app.h"
#include "mbed.h"
#include "wi-fi.h"


/*-------------------------------------------------------*/
const char htmlHeaderOk[] = "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n\r\n";

const char htmlBody[] = "<!DOCTYPE html><html><head><title>SiMBA</title></head><body><p>&nbsp;</p><h1>SiMBa&nbsp;</h1><h2>Estad&iacute;sticas:&nbsp;</h2><ul><li>Temperatura:</li><li>Tapa:</li><li>SensorPir:</li><li>Sensor de gas:</li></ul><h2>Acciones:</h2><ul><li>Abrir tapa</li><li>Cerrar Tapa</li></ul></body></html>\r\n";
const char htmlFooter[] = "\r\n";


/*--------------------------------------------------------*/





App::App() { }

ssize_t App::RenderHtml( Wifi *wifi ) { 
    if ( wifi == nullptr ) {
        return 0;
    }
    _header = htmlHeaderOk;
    _body = htmlBody;
    _footer = htmlFooter;

    return strlen(_header) + strlen(_body)+ strlen(_footer);
}

void App::SendHtmlTo( Wifi* wifi ) {
    if ( wifi == nullptr ) {
        return;
    }
    wifi->SendHtml(_header, _body, _footer);
}