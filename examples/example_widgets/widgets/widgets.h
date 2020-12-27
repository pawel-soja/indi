#pragma once

#include "indiapi.h"
#include "indidevapi.h"
#include <vector>
#include <functional>
#include <tuple>

#include <cstring>

#include "widget.h"
#include "widgetdata.h"
#include "widgetproperty.h"

namespace INDI
{

class WidgetEventDispatcher;

template <typename T>
class Widgets: public WidgetProperty<T>, public std::vector<Widget<T>>
{
    using Element = Widget<T>;
    using Elements = std::vector<Widget<T>>;
    using Property = WidgetProperty<T>;

    WidgetEventDispatcher *dispatcher;
    Elements *mPrevious = nullptr;

    bool snoop = false;
public:
    Widgets(WidgetEventDispatcher *dispatcher, int count = 0)
        : Elements(count)
        , dispatcher(dispatcher)
    {
        dispatcher->update(this);
    }

    using Elements::operator[];///(typename Elements::size_type);
#if 0
    Element &operator[](const std::string &name)
    {

    }
#endif
public:
    Elements *elements()
    {
        return this;
    }

    Elements *previous() const
    {
        return mPrevious;
    }

    void rollback()
    {
        *elements() = *previous();
    }

    Property *property()
    {
        Property::setWidget(*this);
        return this;
    }

public:
    using Property::fill;
    using Property::apply;

public:
    Widgets &snooping(const char *device)
    {
        snoop = true;
        strncpy(this->device, device, sizeof(this->device));
        IDSnoopDevice(device, this->name);
        return *this;
    }

    bool isSnoop() const
    {
        return snoop;
    }

    friend class WidgetEventDispatcherPrivate;
};

using Switches = Widgets<ISwitch>;
using Lights   = Widgets<ILight>;

}