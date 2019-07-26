#ifndef QRANGEFALLBACK_H
#define QRANGEFALLBACK_H

#include <range/v3/view_facade.hpp>

#include <cppcoro/generator.hpp>
#include <cppcoro/recursive_generator.hpp>

#include <QtCore/QObject>

namespace qt::detail
{
    cppcoro::recursive_generator<QObject*> takeChildRecursivelyImpl(
        const QObjectList &children, Qt::FindChildOptions opts)
    {
        for (QObject *c : children) {
            if (opts == Qt::FindChildrenRecursively) {
                co_yield takeChildRecursivelyImpl(c->children(), opts);
            }
            co_yield c;
        }
    }

    cppcoro::recursive_generator<QObject*> takeChildRecursively(
        QObject *root, Qt::FindChildOptions opts = Qt::FindChildrenRecursively)
    {
        if (root) {
            co_yield takeChildRecursivelyImpl(root->children(), opts);
        }
    }

    template <class T = QObject>
    class children_view : public ranges::view_facade<children_view<T>>
    {
        friend ranges::range_access;

        struct Data {
            T *obj;
            cppcoro::recursive_generator<QObject*> gen;
        };
        std::shared_ptr<Data> m_data;

        struct cursor;

        cursor begin_cursor()
        {
            return cursor(m_data->gen.begin());
        }

    public:
        children_view() = default;

        explicit children_view(T *o, Qt::FindChildOptions opts = Qt::FindChildrenRecursively)
            : m_data(std::make_shared<Data>(Data{o, takeChildRecursively(o, opts)}))
        {}

        explicit children_view(T &o, Qt::FindChildOptions opts = Qt::FindChildrenRecursively)
            : children_view(&o, opts)
        {}
    };

    template <class T>
    struct children_view<T>::cursor
    {
        cppcoro::recursive_generator<QObject*>::iterator it;

        decltype(auto) read() const
        {
            return *it;
        }

        void next()
        {
            ++it;
        }

        auto equal(ranges::default_sentinel_t) const
        {
            return it == cppcoro::recursive_generator<QObject*>::iterator(nullptr);
        }

        explicit cursor(cppcoro::recursive_generator<QObject*>::iterator it)
            : it(it)
        {}

        cursor() = default;
    };

} // qt::detail

#endif // QRANGEFALLBACK_H
