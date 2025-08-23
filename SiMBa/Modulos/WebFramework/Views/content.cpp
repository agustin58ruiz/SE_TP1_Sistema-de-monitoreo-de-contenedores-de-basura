#include "content.h"

Content::Content() {
    this->section = nullptr;
    this->length = 0;
    this->next = nullptr;
}

void Content::Init(const char * section, Content * next) {
    this->section = section;
    this->length = strlen(section);
    this->next = next;
}

void Content::SetSection(const char * section) {
    printf("Set Section: %s\r\n", section);
    this->section = section;
    this->length = strlen(section);
}

void Content::SetNext(Content * next) {
    this->next = next;
}

size_t Content::TotalLength() {
    size_t accum = 0;
    Content * current;

    current = this; 

    while (current != nullptr) {
        accum += current->length;
        current = current->next;
    }

    return accum;


}