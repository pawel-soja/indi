#pragma once


#ifndef D_PTR
# define D_PTR(Class) Class##Private * const d = d_func()
#endif

template <typename T> static inline T *getPtrHelper(T *ptr) { return ptr; }
template <typename Wrapper> static inline typename Wrapper::pointer getPtrHelper(const Wrapper &p) { return p.get(); }


#ifndef DECLARE_PRIVATE_
#define DECLARE_PRIVATE(Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(getPtrHelper(d_ptr)); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(getPtrHelper(d_ptr)); } \
    friend class Class##Private;
#endif

#ifndef DECLARE_PRIVATE_D
#define DECLARE_PRIVATE_D(Dptr, Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>((Dptr)); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(getPtrHelper(Dptr)); } \
    friend class Class##Private;
#endif


template <typename...T>
class RestoreLater
{
    std::tuple<T&...> r;
    std::tuple<T ...> v;
public:
    RestoreLater(const RestoreLater &) = default;
    RestoreLater(T&...r): r(r...), v(r...) { }
    ~RestoreLater() { r = v; }
};

template <typename...T>
inline RestoreLater<T...> restoreLater(T&...t)
{
    return RestoreLater<T...>(t...);
}


#if 0
class WidgetException
{
    IPState mState;
public:
    WidgetException(const IPState &state)
        : mState(state)
    {

    }
    WidgetException(IPState state, const char *message)
        : mState(state)
    {

    }

    WidgetException()
        : mState(IPS_OK)
    {

    }

    IPState state() const
    {
        return mState;
    }
};
#endif
