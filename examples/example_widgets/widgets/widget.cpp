#include "widget.h"

namespace INDI
{

// Switch

struct WidgetSwitchPrivate: public WidgetDataPrivate
{
    std::function<void()> onChanged;
    std::function<void()> onSelected;
};

Widget<ISwitch>::Widget()
    : WidgetData(*new WidgetSwitchPrivate)
{

}

Widget<ISwitch>::~Widget()
{
}


Widget<ISwitch> &Widget<ISwitch>::fill(const char *name, const char *label, ISState state)
{
    auto autoRestore = restoreLater(this->aux);
    IUFillSwitch(this, name, label, state);
    return *this;
}

void Widget<ISwitch>::onChanged(const std::function<void()> &callback)
{
    D_PTR(WidgetSwitch);
    d->onChanged = callback;
}

void Widget<ISwitch>::onSelected(const std::function<void()> &callback)
{
    D_PTR(WidgetSwitch);
    d->onSelected = callback;
}
#if 0
void Widget<ISwitch>::emit()
{
    D_PTR(WidgetSwitch);
    if (d->onToggled) d->onToggled(false);
}
#endif
bool Widget<ISwitch>::emit(Widget<ISwitch> *old)
{
    D_PTR(WidgetSwitch);
    d->previous = old;
    if (d->onChanged) d->onChanged();
    if (d->onSelected && this->s == ISS_ON) d->onSelected();
    return true;
}

// Light
struct WidgetLightPrivate: public WidgetDataPrivate
{
    std::function<void()> onChanged;
};

Widget<ILight>::Widget()
    : WidgetData(*new WidgetLightPrivate)
{

}

Widget<ILight>::~Widget()
{

}

Widget<ILight> &Widget<ILight>::fill(const char *name, const char *label, IPState state)
{
    auto autoRestore = restoreLater(this->aux);
    IUFillLight(this, name, label, state);
    return *this;
}

void Widget<ILight>::onChanged(const std::function<void()> &callback)
{
    D_PTR(WidgetLight);
    d->onChanged = callback;
}

bool Widget<ILight>::emit(Widget<ILight> *old)
{
    D_PTR(WidgetLight);
    d->previous = old;
    if (d->onChanged) d->onChanged();
    return true;
}

}
