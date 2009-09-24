#ifndef EVENTS_H_
#define EVENTS_H_

#include <vector>
#include <string>

class Events 
{
public:
    enum Severity 
    {
        EV_INFO,
        EV_WARNING,
        EV_ERROR
    };
    void AddEvent(const char *msg, Severity type);
    static Events *GetEvents();

private:
    Events(){}
    Events(const Events &);
    Events &operator= (const Events &);
    std::vector<std::pair<std::string, Severity> > events;
};

#endif // EVENTS_H_
