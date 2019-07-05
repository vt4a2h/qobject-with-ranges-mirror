#ifndef QRANGEFALLBACK_H
#define QRANGEFALLBACK_H

#include <range/v3/view_facade.hpp>

#include <QtCore/QObject>

namespace qt::detail
{
    struct cursor
    {
        std::shared_ptr<QObjectList> children;
        int current_index = 0;

        decltype(auto) read() const
        {
            return (*children)[current_index];
        }

        void next()
        {
            ++current_index;
        }

        auto equal(ranges::default_sentinel_t) const
        {
            return current_index == children->count();
        }

        auto equal(const cursor &that) const
        {
            return current_index == that.current_index;
        }

        explicit cursor(const QObject *o, Qt::FindChildOptions opts)
                : children(std::make_shared<QObjectList>(
                o ? o->findChildren<QObject*>(QString(), opts) : QObjectList()))
        {}

        cursor() = default;
    };

    template <class T = QObject>
    class children_view : public ranges::view_facade<children_view<T>>
    {
        friend ranges::range_access;

        T *obj;
        Qt::FindChildOptions opts;

        cursor begin_cursor()
        {
            return cursor(obj, opts);
        }

    public:
        children_view() = default;
        explicit children_view(T *o, Qt::FindChildOptions opts = Qt::FindChildrenRecursively)
                : obj(o)
                , opts(opts)
        {}

        explicit children_view(T &o, Qt::FindChildOptions opts = Qt::FindChildrenRecursively)
                : children_view(&o, opts)
        {}
    };
} // qt::detail
#endif // QRANGEFALLBACK_H
