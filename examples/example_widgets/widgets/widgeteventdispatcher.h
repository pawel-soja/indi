#pragma once

#include <memory>
#include "widgets.h"

namespace INDI
{

//class Switches;
class DefaultDevice;
class WidgetEventDispatcherPrivate;
class WidgetEventDispatcher
{
public:
    WidgetEventDispatcher(DefaultDevice *device);
    ~WidgetEventDispatcher();

    void update(Switches *switches);
    void update(Lights   *lights);

public:
    //void rollback();
    //void apply(IPSState state, const char *message);

protected:
    std::shared_ptr<WidgetEventDispatcherPrivate> d_ptr;
    WidgetEventDispatcherPrivate * d_func();
};

}
