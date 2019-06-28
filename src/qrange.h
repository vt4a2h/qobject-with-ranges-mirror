#ifndef QRANGE_H
#define QRANGE_H

#include <range/v3/view_facade.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

namespace qt
{
    namespace r = ranges;
    namespace v = r::view;

    namespace detail
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

    }

    const auto children = ranges::make_pipeable([](auto &&o) { return detail::children_view(o); });

    auto find_children(Qt::FindChildOptions opts = Qt::FindChildrenRecursively)
    {
        return ranges::make_pipeable([opts](auto &&o) { return detail::children_view(o, opts); });
    }

    template <class T>
    const auto with_type = v::filter([](auto &&o) {
                               using ObjType = std::remove_cv_t<std::remove_pointer_t<T>>;
                               return ObjType::staticMetaObject.cast(o);
                           }) |
                           v::transform([](auto &&o){ return static_cast<T>(o); });

    auto by_name(const QString &name)
    {
        return v::filter([name](auto &&obj) { return obj->objectName() == name; });
    }

    auto by_re(const QRegularExpression &re)
    {
        return v::filter([re](auto &&obj) { return re.match(obj->objectName()).hasMatch(); });;
    }
}

#endif // QRANGE_H
