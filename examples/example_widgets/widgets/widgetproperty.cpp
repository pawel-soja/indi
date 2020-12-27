#include "widgetproperty.h"


namespace INDI
{

void WidgetProperty<ISwitch>::setWidget(std::vector<Widget<ISwitch>> &widget)
{
    nsp = widget.size();
    sp = widget.data();
}

WidgetProperty<ISwitch> &WidgetProperty<ISwitch>::fill(
    const char *name, const char *label, const char *group,
    IPerm perm, ISRule rule, double timeout, IPState state
)
{
    IUFillSwitchVector(
        this, nullptr, 0,
        "", name, label, group, perm, rule, timeout, state
    );
    return *this;
}

WidgetProperty<ISwitch> &WidgetProperty<ISwitch>::apply(IPState state, const char *message)
{
    this->s = state;
    IDSetSwitch(this, message);
    return *this;
}

}
