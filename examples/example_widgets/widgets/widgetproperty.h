#pragma once

#include "indiapi.h"
#include "indidevapi.h"

#include <vector>
#include "widget.h"

namespace INDI
{

template <typename T>
struct WidgetProperty;

template <>
struct WidgetProperty<ISwitch>: public ISwitchVectorProperty
{
    using Type = ISwitchVectorProperty;

    void setWidget(std::vector<Widget<ISwitch>> &widget);

    WidgetProperty &fill(
        const char *name, const char *label, const char *group,
        IPerm perm, ISRule rule, double timeout, IPState state = IPS_IDLE
    );

    //WidgetProperty &define(const char *fmt, ...)

    WidgetProperty &reset()
    {
        IUResetSwitch(this);
        return *this;
    }

    WidgetProperty &apply(IPState state = IPS_OK, const char *message = nullptr);
};

template <>
struct WidgetProperty<ILight>: public ILightVectorProperty
{
    using Type = ILightVectorProperty;

    void setWidget(std::vector<Widget<ILight>> &widget)
    {
        nlp = widget.size();
        lp = widget.data();
    }

    WidgetProperty &fill(
        const char *name, const char *label, const char *group,
        IPState state = IPS_IDLE
    )
    {
        IUFillLightVector(
            this, nullptr, 0,
            "", name, label, group, state
        );
        return *this;
    }

    WidgetProperty &apply(IPState state = IPS_OK, const char *message = nullptr)
    {
        this->s = state;
        IDSetLight(this, message);
        return *this;
    }
};

}
