#ifndef APP__H
#define APP__H

#include "mbed.h"
class Wifi;

class App {
private:
char **_routes;
const char * _header;
const char * _body;
const char * _footer;

public:
App();
void SendHtmlTo(Wifi *wifi);
ssize_t RenderHtml(Wifi *wifi);
};



#endif