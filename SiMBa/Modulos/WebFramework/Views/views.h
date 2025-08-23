#ifndef VIEWS__H
#define VIEWS__H

#include "simba.h"
#include "web-types.h"
#include "content.h"

class App;


class LayoutRender {
public:
    void RenderHttpHeader(int statusCode = 200, ContentType contentType = ContentType::APPLICATION_JSON);
    void RenderHeader(char * title);
    void RenderFooter();
    void SetJson(Content * innerContent);
    void SetInnerContent(Content * innerContent);
    void SetEnd(Content * penultimateContent);
    size_t TotalLength();

    Content * GetRenderedLayout();
};


class View {
protected:
    App * _app;
    Simba * _simba;
    size_t _contentLen;
    size_t _headerLen;
    char * _httpHeader;

    Content * content;
    
public:

    View( App * app, Simba * simba );
    virtual size_t Render(Action action) = 0;

};

class HomeView : public View {
public:

    HomeView(App * app, Simba * simba);
    size_t Render(Action action) override;

    size_t RenderPost();
    size_t RenderGet();

};

class ErrorView : public View {
public:

    ErrorView(App * app, Simba * simba);
    size_t Render(Action action) override;

    size_t RenderPost();
    
    size_t RenderGet();

};

class DevicesView : public View {
public:

    DevicesView(App * app, Simba * simba);

    size_t Render(Action action) override;
    
    size_t RenderGetTemperature();
    size_t ActivateManualMode();
    size_t DeactivateManualMode();
    size_t OpenLid();
    size_t CloseLid();
    size_t StopAlarm();
    size_t StartAlarm();

};


#endif