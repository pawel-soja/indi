#include "widgetdata.h"

void WidgetDataPrivate::ref()
{
    ++refCount;
}

void WidgetDataPrivate::deref()
{
    if (--refCount == 0)
        delete this;
}

WidgetDataPrivate::WidgetDataPrivate()
{
    ref();
}

WidgetDataPrivate::~WidgetDataPrivate()
{

}
