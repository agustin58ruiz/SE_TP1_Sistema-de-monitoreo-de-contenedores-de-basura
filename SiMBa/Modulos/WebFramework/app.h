#ifndef APP__H
#define APP__H

#include "mbed.h"
#include "simba.h"
#include "web-types.h"
#include "views.h"

class Wifi;

uint32_t djb2(const char* str);

class View;

class App {
private:
    char **_routes;
    
    Content * _content; 
        
    Simba * _simba;
    
    int _sentChars;
    
    //char * _generateHeader(int statusCode = 200, ContentType contentType = ContentType::APPLICATION_JSON );
    void _render(void (App::*viewFunc)());
    
    View * _currentView;
    
    RouteInfo _parseRoute(char * route);
   
    View * _findView(Resource resource);

    // Views
    HomeView _homeView;
    ErrorView _errorView;
    DevicesView _devicesView;
    /*
    // Controllers
    HomeController _homeController;
    ErrorController _errorController;
    DevicesController _devicesController;
  */
public:

    LayoutRender layout;
    
    App(Simba * simba);

    void UpdateRenderedContent();
    
    void SendHtml( Wifi* wifi , int length);
    
    size_t HandleRequest(Wifi *wifi); 

    void SetRenderedContent(Content * content);


};


#endif