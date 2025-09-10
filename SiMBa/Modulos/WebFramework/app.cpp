#include "app.h"
#include "mbed.h"
#include "wi-fi.h"
#include "views.h"


App::App(Simba * simba) 
: _homeView(this, simba), 
  _errorView(this, simba), 
  _devicesView(this, simba)/*, 
  _homeController(this, simba, &_homeView), 
  _errorController(this, simba, &_errorView),
  _devicesController(this, simba, &_devicesView) */{ 
   
    _simba = simba;
    _currentView = nullptr;
    _content = nullptr;
}

size_t App::HandleRequest( Wifi *wifi ) { 
    if ( wifi == nullptr ) {
        return 0;
    } else if ( (wifi->Route())[0] == '\0') {
        return 0;
    }

    RouteInfo routeInfo = _parseRoute( wifi->Route() );

    //Controller * controller = _findController( routeInfo );
    View * view = _findView( routeInfo.resource );

    return view->Render( routeInfo.action );

}

void App::UpdateRenderedContent() {
    _sentChars = 0;
    _content = layout.GetRenderedLayout();

}

void App::SendHtml( Wifi* wifi , int length ) {
    if ( wifi == nullptr ) {
        return;
    }

    while (_content != nullptr && length != 0){
        if (_sentChars < _content->length ) {
            if ( (_content->length - _sentChars) > length ) {
                wifi->SendHtmlN(_content->section + _sentChars, length );
                _sentChars += length;
                length = 0;
            } else {
                wifi->SendHtml(_content->section + _sentChars);
                length -= _content->length - _sentChars;
                _sentChars=0;
                _content = _content->next;

            }
        }
    }
}

void App::_render(void (App::*viewFunc)()) {
    
    (this->*viewFunc)();

}

void App::SetRenderedContent(Content * content) {
    _content = content;
}


