#include "app.h"


uint32_t djb2(const char* str) {
    uint32_t hash = 5381;
    while (*str)
        hash = ((hash << 5) + hash) + *str++; 
    return hash;
}

/*Routes of SImBa*/

static RouteInfo Routes[] = {
    { djb2("GET /"), Action::GET, Resource::HOME },
    { djb2("GET /temp"), Action::GET_TEMPERATURE, Resource::DEVICES},
    { djb2("POST /activatemanual"), Action::ACTIVATE_MANUAL, Resource::DEVICES},
    { djb2("POST /deactivatemanual"), Action::DEACTIVATE_MANUAL, Resource::DEVICES},
    { djb2("POST /openlid"), Action::OPEN_LID, Resource::DEVICES},
    { djb2("POST /closelid"), Action::CLOSE_LID, Resource::DEVICES},
    { djb2("POST /alarmon"), Action::START_ALARM, Resource::DEVICES},
    { djb2("POST /alarmoff"), Action::STOP_ALARM, Resource::DEVICES},
};

RouteInfo App::_parseRoute(char * route) {
    uint32_t hash; 
    //Route parseRoute = Route::NOT_FOUND;
    RouteInfo parsedRoute = {0, Action::GET, Resource::NOT_FOUND };
    if ( route == nullptr ) return parsedRoute;

    hash = djb2(route);

    for (int i = 0; i < sizeof(Routes)/sizeof(Routes[0]); i++) {
        if ( Routes[i].hash == hash ) {
            parsedRoute = Routes[i];
            break;
        }
    }

    return parsedRoute;
}



View * App::_findView(Resource resource) {
    View * view;
    
    switch (resource) {
        case Resource::HOME:
            printf("HOME VIEW\r\n");
            view = &_homeView;
            break;
        case Resource::DEVICES:
            printf("DEVICES VIEW\r\n");
            view = &_devicesView;
            break;
        case Resource::NOT_FOUND:
            printf("ERROR VIEW\r\n");
            view = &_errorView;
            break;
    }

    return view;
}