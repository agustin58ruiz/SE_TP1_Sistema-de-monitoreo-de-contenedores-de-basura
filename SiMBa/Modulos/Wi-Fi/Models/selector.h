#ifndef SELECTOR__H
#define SELECTOR__H


class Selector {
private:
    const char ** _options;
    int *_indexes;
    int _currentOption;
    bool _finished;
    //bool _crReceived;
public:
    Selector(const char ** options);
    ~Selector();
    bool Finished();

    void Update(char c);
    int Result();
    void Reset();


};


#endif
