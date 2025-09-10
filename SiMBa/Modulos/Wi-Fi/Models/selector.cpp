#include "selector.h"

Selector::Selector(const char ** options): _options(options), _currentOption(-1), _finished(false) {
    int count = 0;
    while (_options[count] != nullptr) ++count;

    _indexes = new int[count];
    for (int i =0; i<count; i++) {
        _indexes[i] = 0;
    }
}

Selector::~Selector() {
    delete[] _indexes;
}

void Selector::Update(char c) {
    if (_finished) return;

    /*if (_crReceived && c == '\n') {
        _finished = true;
        return;
    }

    _rcReceived = (c == '\r');*/

     bool areOptions = false;

    for(int i = 0; _options[i] != nullptr; i++) {
        if (_indexes[i] >= 0 && _options[i][_indexes[i]] == c) {
            _indexes[i]++;
            if (_options[i][_indexes[i]] == '\0') {
                _currentOption = i;
                _finished = true;
                return;
            }
            areOptions = true;
        } else {
            _indexes[i] = -1;
        }
    }

    if (!areOptions) {
        _finished = true;
    }
}

bool Selector::Finished() {
    return _finished;
}

int Selector::Result() {
    if (!_finished) return -1;
    return _currentOption;
}

void Selector::Reset() {
    int count = 0;
    
    _currentOption = -1;
    _finished = false;
    
    while (_options[count] != nullptr) ++count;

    for (int i =0; i<count; i++) {
        _indexes[i] = 0;
    } 

}

