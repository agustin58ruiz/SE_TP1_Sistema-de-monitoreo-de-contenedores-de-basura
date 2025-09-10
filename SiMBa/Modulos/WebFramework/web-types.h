#ifndef WEB_TYPES__H
#define WEB_TYPES__H

enum class Resource { HOME, DEVICES, NOT_FOUND };
enum class Action { GET, 
    POST, 
    GET_TEMPERATURE, 
    ACTIVATE_MANUAL, 
    DEACTIVATE_MANUAL,
    OPEN_LID,
    CLOSE_LID,
    START_ALARM,
    STOP_ALARM };
enum class ContentType { APPLICATION_JSON, TEXT_HTML};

struct RouteInfo {
    uint32_t hash;
    Action action;
    Resource resource;
};

#endif