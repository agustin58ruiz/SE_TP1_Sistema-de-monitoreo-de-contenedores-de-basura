#ifndef CONTENT__H
#define CONTENT__H

#include "mbed.h"

class Content {
public:
    const char * section;
    size_t length;
    Content * next;
    Content();
    void Init(const char * section, Content * next);
    void SetNext(Content * next);
    void SetSection(const char * section);
    size_t TotalLength();
};

#endif