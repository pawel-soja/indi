#include "indiapi.h"
#include "indidriver.h"
#include "indidevapi.h"
#include "basedevice.h"
#include "defaultdevice.h"

#include "widgets.h"
#include "widgeteventdispatcher.h"

#define D_PTR(Class) Class##Private * const d = d_func()

#include <algorithm>
#include <list>
#include <map>
#include <cstring>

namespace INDI
{

class WidgetEventDispatcherPrivate
{
public:
    DefaultDevice *parent;
    std::list<Switches*> switches;
    std::list<Lights*>   lights;

    WidgetEventDispatcherPrivate();
    virtual ~WidgetEventDispatcherPrivate();
    void ISGetProperties(const char *dev);
    void ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n);
    void ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n);
    void ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n);
    void ISNewBLOB(const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[], char *formats[], char *names[], int n);

    void ISSnoopDevice(XMLEle *root);

    friend void ::ISGetProperties(const char *dev);
    friend void ::ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int num);
    friend void ::ISNewText(const char *dev, const char *name, char *texts[], char *names[], int num);
    friend void ::ISNewNumber(const char *dev, const char *name, double values[], char *names[], int num);
    friend void ::ISNewBLOB(const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[],
                            char *formats[], char *names[], int n);
    friend void ::ISSnoopDevice(XMLEle *root);

public:
    template <typename T>
    void emit(Widgets<T> &widgets, typename Widgets<T>::Elements &old);
};




WidgetEventDispatcher::WidgetEventDispatcher(DefaultDevice *device)
    : d_ptr(new WidgetEventDispatcherPrivate)
{
    D_PTR(WidgetEventDispatcher);
    d->parent = device;
}

WidgetEventDispatcher::~WidgetEventDispatcher()
{

}

void WidgetEventDispatcher::update(Switches *switches)
{
    D_PTR(WidgetEventDispatcher);
    d->switches.push_back(switches);
}

void WidgetEventDispatcher::update(Lights *lights)
{
    D_PTR(WidgetEventDispatcher);
    d->lights.push_back(lights);
}





WidgetEventDispatcherPrivate * WidgetEventDispatcher::d_func()
{
    return static_cast<WidgetEventDispatcherPrivate *>(d_ptr.get());
}








void WidgetEventDispatcherPrivate::ISGetProperties(const char *dev)
{
    parent->ISGetProperties(dev);
    for (auto &item: switches)
    {
        if (item->isSnoop())
            continue;
        auto property = item->property();
        strcpy(property->device, parent->getDeviceName());
        parent->defineSwitch(property);
    }
    
    for (auto &item: lights)
    {
        if (item->isSnoop())
            continue;
        auto property = item->property();
        strcpy(property->device, parent->getDeviceName());
        parent->defineLight(property);
    }    
}

template <typename T>
void WidgetEventDispatcherPrivate::emit(Widgets<T> &widgets, typename Widgets<T>::Elements &old)
{
    widgets.mPrevious = &old;

    auto n = std::begin(*widgets.elements());
    auto o = std::begin(old);
    auto end = std::end(*widgets.elements());
    for(; n != end; ++n, ++o)
        if (n->s != o->s)
            n->emit(&*o);

    widgets.mPrevious = nullptr;
}

void WidgetEventDispatcherPrivate::ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n)
{
    for (auto &it : switches)
    {
        if (it->name != std::string(name))
            continue;

        bool emited = false;
        auto copy = *it->elements();
        IUUpdateSwitch(it->property(), states, names, n);

        emit(*it, copy);

        if (!emited)
            it->apply();
        break;
    }
    parent->ISNewSwitch(dev, name, states, names, n);
}

void WidgetEventDispatcherPrivate::ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n)
{
    parent->ISNewText(dev, name, texts, names, n);
}

void WidgetEventDispatcherPrivate::ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n)
{
    parent->ISNewNumber(dev, name, values, names, n);
}

void WidgetEventDispatcherPrivate::ISNewBLOB(const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[], char *formats[], char *names[], int n)
{
    parent->ISNewBLOB(dev, name, sizes, blobsizes, blobs, formats, names, n);
}


void WidgetEventDispatcherPrivate::ISSnoopDevice(XMLEle *root)
{
    for (auto &it: lights)
    {
        if (!it->isSnoop())
            continue;

        auto copy = *it->elements();
        if (IUSnoopLight(root, it->property()) == 0)
            emit(*it, copy);
    }
}



//static std::map<std::string, INDI::WidgetEventDispatcherPrivate*> dispatchers;
static std::list<INDI::WidgetEventDispatcherPrivate*> &dispatchers()
{
    static std::list<INDI::WidgetEventDispatcherPrivate*> sdispatchers;
    return sdispatchers;
}

WidgetEventDispatcherPrivate::WidgetEventDispatcherPrivate()
{
    dispatchers().push_back(this);
}

WidgetEventDispatcherPrivate::~WidgetEventDispatcherPrivate()
{
    dispatchers().remove(this);
}

}

template <typename Function>
static void for_each_dispatchers(const char *dev, Function f)
{
    // not specified - eval for all
    if (dev == nullptr)
    {
        for(auto &item : INDI::dispatchers())
            f(*item);

        return;
    }

    for(auto &item : INDI::dispatchers())
    {
        if (item->parent && item->parent->getDeviceName() == std::string(dev))
        {
            f(*item);
            break;
        }
    }
}

void ISGetProperties(const char *dev)
{
    fprintf(stderr, "%s: %s\n", __FUNCTION__, dev);
    for_each_dispatchers(dev, [&](INDI::WidgetEventDispatcherPrivate &dispatcher){
        dispatcher.ISGetProperties(dev);
    });
}

void ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n)
{
    fprintf(stderr, "%s: %s\n", __FUNCTION__, dev);
    for_each_dispatchers(dev, [&](INDI::WidgetEventDispatcherPrivate &dispatcher){
        dispatcher.ISNewSwitch(dev, name, states, names, n);
    });
}

void ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n)
{
    fprintf(stderr, "%s: %s\n", __FUNCTION__, dev);
    for_each_dispatchers(dev, [&](INDI::WidgetEventDispatcherPrivate &dispatcher){
        dispatcher.ISNewText(dev, name, texts, names, n);
    });
}

void ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n)
{
    fprintf(stderr, "%s: %s\n", __FUNCTION__, dev);
    for_each_dispatchers(dev, [&](INDI::WidgetEventDispatcherPrivate &dispatcher){
        dispatcher.ISNewNumber(dev, name, values, names, n);
    });
}

void ISNewBLOB(const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[], char *formats[],
               char *names[], int n)
{
    fprintf(stderr, "%s: %s\n", __FUNCTION__, dev);
    for_each_dispatchers(dev, [&](INDI::WidgetEventDispatcherPrivate &dispatcher){
        dispatcher.ISNewBLOB(dev, name, sizes, blobsizes, blobs, formats, names, n);
    });
}

void ISSnoopDevice(XMLEle *root)
{
    fprintf(stderr, "%s\n", __FUNCTION__);
    for_each_dispatchers(nullptr, [&](INDI::WidgetEventDispatcherPrivate &dispatcher){
        dispatcher.ISSnoopDevice(root);
    });
}
