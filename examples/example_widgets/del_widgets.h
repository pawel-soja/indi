#pragma once

#include "indiapi.h"
#include "indidevapi.h"
#include <vector>
#include <functional>

namespace INDI
{




class WidgetBase
{
public:

};

template <typename T>
struct WidgetExtend;

template <>
struct WidgetExtend<ISwitch>: public ISwitch
{
    WidgetExtend();
    ~WidgetExtend();

    WidgetExtend<ISwitch> &fill(const char *name, const char *label, ISState state)
    {
        void *aux = this->aux;
        IUFillSwitch(this, name, label, state);
        this->aux = aux;
        return *this;
    }

    void emitChange();

    void onChange(const std::function<void()> &f);
};

template <typename T>
struct WidgetPropertyExtend;


template <>
struct WidgetPropertyExtend<ISwitch>: public ISwitchVectorProperty
{
    using Type = ISwitchVectorProperty;

    void setWidget(std::vector<WidgetExtend<ISwitch>> &widget)
    {
        nsp = widget.size();
        sp = widget.data();
    }

    WidgetPropertyExtend<ISwitch> &fill(
        const char *name, const char *label, const char *group,
        IPerm perm, ISRule rule, double timeout, IPState state = IPS_IDLE
    )
    {
        IUFillSwitchVector(
            this, nullptr, 0,
            "", name, label, group, perm, rule, timeout, state
        );
        return *this;
    }
};


class WidgetEventDispatcher;

template <typename T>
class Widgets: public WidgetPropertyExtend<T>//, public std::vector<WidgetExtend<T>>
{
    using Element = WidgetExtend<T>;
    using Property = WidgetPropertyExtend<T>;

    WidgetEventDispatcher *dispatcher;

    std::vector<T> mWidgets;

public:
    Widgets(WidgetEventDispatcher *dispatcher, int count = 0)
        : dispatcher(dispatcher)
    {
        dispatcher->update(this);
        mWidgets.resize(count);
    }

    typename Property::Type * property()
    {
        //Property::setWidget(*this);
        return this;
    }

    typename Property::Type * operator&()
    {
        return property();
    }

public:
    class Reference
    {
        Widgets *parent;
    public:
        Reference(Widgets *parent)
            : parent(parent)
        {}

    };
    Reference operator[](int i)
    {
        return Reference(this);
    }    
};

using Switches = Widgets<ISwitch>;

}