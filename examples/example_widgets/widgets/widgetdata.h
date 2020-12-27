#pragma once

#include <atomic>
#include "indiapi.h"
#include "utility.h"

/**
 * @brief Private data for the widget
 * Base class that cares about data lifetime.
 */
class WidgetDataPrivate
{
public:
    WidgetDataPrivate();
    virtual ~WidgetDataPrivate();

private:
    void ref();
    void deref();
    std::atomic_int refCount;

    template <typename> friend class WidgetData;
    WidgetDataPrivate(const WidgetDataPrivate&) = delete;
    WidgetDataPrivate(WidgetDataPrivate &&) = delete;

public:
    void *previous;
};

/**
 * @brief Adding data to the structure
 * Extending structures with user data without increasing the size of the structure.
 */
template <typename T>
class WidgetData: public T
{
public:
    // a place to store the data pointer.
    struct Extract
    {
        static void *&aux(const IText   *s) { return const_cast<void*&>(s->aux0); }
        static void *&aux(const INumber *s) { return const_cast<void*&>(s->aux0); }
        static void *&aux(const ISwitch *s) { return const_cast<void*&>(s->aux ); }
        static void *&aux(const ILight  *s) { return const_cast<void*&>(s->aux ); }
        static void *&aux(const IBLOB   *s) { return const_cast<void*&>(s->aux0); }
    };

public:

    WidgetData()
    {
        d_func() = new WidgetDataPrivate();
    }

    WidgetData(const WidgetData &other)
        : T(other)
    {
        if (d_func() != nullptr)
            d_func()->ref();
    }

    WidgetData(WidgetData &&other)
        : T(other)
    {
        other.d_func() = nullptr;
    }

    ~WidgetData()
    {
        if (d_func() != nullptr)
            d_func()->deref();
    }

    WidgetData &operator=(const WidgetData &other)
    {
        return *this = WidgetData(other);
    }

    WidgetData &operator=(WidgetData &&other)
    {
        std::swap(static_cast<T&>(*this), static_cast<T&>(other));
        return *this;
    }

public:
#warning return Widget<T> &
    WidgetData &previous() const
    {
        return *reinterpret_cast<WidgetData*&>(d_func()->previous);
    }

protected:
    // Extend the data structure by inheriting from WidgetDataPrivate.
    WidgetData(WidgetDataPrivate &dd)
    {
        d_func() = &dd;
    }

    WidgetDataPrivate * &d_func()
    {
        WidgetDataPrivate **dp = reinterpret_cast<WidgetDataPrivate**>(&Extract::aux(this));
        return *dp;
    }

    WidgetDataPrivate * &d_func() const
    {
        WidgetDataPrivate **dp = reinterpret_cast<WidgetDataPrivate**>(&Extract::aux(this));
        return *dp;
    }
};
