#include "views.h"
#include "app.h"

static Content json;


DevicesView::DevicesView(App * app, Simba * simba) : View( app, simba ) {

    json.SetNext(nullptr);

}

size_t DevicesView::Render(Action action) {
    size_t length = 0;

    switch (action) {
        case Action::GET_TEMPERATURE:
            length = RenderGetTemperature();
            break;
        case Action::ACTIVATE_MANUAL:
            length = ActivateManualMode();
            break;
        case Action::DEACTIVATE_MANUAL:
            length = DeactivateManualMode();
            break;
        case Action::OPEN_LID:
            length = OpenLid();
            break;
        case Action::CLOSE_LID:
            length = CloseLid();
            break;
        case Action::STOP_ALARM:
            length = StopAlarm();
            break;
        case Action::START_ALARM:
            length = StartAlarm();
            break;
        default:
        break;
    } 
    return length; 
}

size_t DevicesView::RenderGetTemperature() {
    static char content[24];
    static char temp[17];
    
    _simba->ObtenerTemperatura(temp);
    
    
    sprintf(content,"{\"temperature\": %s}",temp );
    
    json.SetSection(content);

    _app->layout.SetJson(&json);
    _app->layout.RenderHttpHeader(200, ContentType::APPLICATION_JSON);
    _app->UpdateRenderedContent();
    return _app->layout.TotalLength(); 

}

size_t DevicesView::ActivateManualMode() {
    _simba->ActivarModoManual();
    
    json.SetSection("{\"modo_automatico\": \"activado\"}");

    _app->layout.SetJson(&json);
    _app->layout.RenderHttpHeader(201, ContentType::APPLICATION_JSON);
    _app->UpdateRenderedContent();
    return _app->layout.TotalLength(); 

}

size_t DevicesView::DeactivateManualMode() {
    _simba->DesactivarModoManual();
    
    json.SetSection("{\"modo_automatico\": \"desactivado\"}");

    _app->layout.SetJson(&json);
    _app->layout.RenderHttpHeader(201, ContentType::APPLICATION_JSON);
    _app->UpdateRenderedContent();
    return _app->layout.TotalLength(); 

}

size_t DevicesView::OpenLid() {
    _simba->AbrirTapa();
    
    json.SetSection("{\"tapa\": \"abierta\"}");

    _app->layout.SetJson(&json);
    _app->layout.RenderHttpHeader(201, ContentType::APPLICATION_JSON);
    _app->UpdateRenderedContent();
    return _app->layout.TotalLength(); 

}

size_t DevicesView::CloseLid() {
    _simba->CerrarTapa();
    
    json.SetSection("{\"tapa\": \"cerrada\"}");

    _app->layout.SetJson(&json);
    _app->layout.RenderHttpHeader(201, ContentType::APPLICATION_JSON);
    _app->UpdateRenderedContent();
    return _app->layout.TotalLength(); 

}


size_t DevicesView::StartAlarm() {
    _simba->EncenderAlarma();
    
    json.SetSection("{\"alarma\": \"encendida\"}");

    _app->layout.SetJson(&json);
    _app->layout.RenderHttpHeader(201, ContentType::APPLICATION_JSON);
    _app->UpdateRenderedContent();
    return _app->layout.TotalLength(); 

}

size_t DevicesView::StopAlarm() {
    _simba->ApagarAlarma();
    
    json.SetSection("{\"alarma\": \"apagada\"}");

    _app->layout.SetJson(&json);
    _app->layout.RenderHttpHeader(201, ContentType::APPLICATION_JSON);
    _app->UpdateRenderedContent();
    return _app->layout.TotalLength(); 

}