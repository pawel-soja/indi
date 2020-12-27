#pragma once

#include "indiapi.h"
#include "indidevapi.h"

#include "widgetdata.h"

#include <functional>
#include <vector>

namespace INDI
{


template <typename>
struct Widget;

class WidgetSwitchPrivate;
template<>
struct Widget<ISwitch>: public WidgetData<ISwitch>
{
    DECLARE_PRIVATE_D(WidgetData<ISwitch>::d_func(), WidgetSwitch)
public:
    Widget();
    ~Widget();

public:
    Widget &fill(const char *name, const char *label, ISState state);
    ISState state() const
    {
        return this->s;
    }

    bool isState(ISState stat) const
    {
        return state() == stat;
    }


public:
    void onChanged(const std::function<void()> &callback);
    void onSelected(const std::function<void()> &callback);

public:
    bool emit(Widget *old);
};

class WidgetLightPrivate;
template<>
struct Widget<ILight>: public WidgetData<ILight>
{
    DECLARE_PRIVATE_D(WidgetData<ILight>::d_func(), WidgetLight)
public:
    Widget();
    ~Widget();

public:
    Widget &fill(const char *name, const char *label, IPState state = IPS_IDLE);
    IPState state() const
    {
        return this->s;
    }

    bool isState(IPState stat) const
    {
        return state() == stat;
    }

public:
    void onChanged(const std::function<void()> &callback);

public:
    bool emit(Widget *old);
};

using Switch = Widget<ISwitch>;
using Light  = Widget<ILight>;

}
