#ifndef QRANGE_H
#define QRANGE_H

#include <range/v3/view_facade.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>
#include <QDebug>

namespace qt
{

namespace r = ranges;
namespace v = r::view;

namespace detail
{
    struct cursor
    {
        const QObject *root{nullptr};
        Qt::FindChildOptions opts{Qt::FindChildrenRecursively};
        QObject *current{nullptr};

        int index{1};
        int last_depth{1};
        int new_index{0};

        QObject *read() const noexcept;

        void next() noexcept;

        bool equal(ranges::default_sentinel_t) const noexcept;

        bool equal(const cursor &that) const noexcept;

        explicit cursor(const QObject *o, Qt::FindChildOptions opts);
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

}  // namespace detail

const auto children = ranges::make_pipeable([](auto &&o) { return detail::children_view(o); });

inline auto find_children(Qt::FindChildOptions opts = Qt::FindChildrenRecursively)
{
    return ranges::make_pipeable([opts](auto &&o) { return detail::children_view(o, opts); });
}

template <class T>
const auto with_type = v::filter([](auto &&o) {
                           using ObjType = std::remove_cv_t<std::remove_pointer_t<T>>;
                           return ObjType::staticMetaObject.cast(o);
                       }) |
                       v::transform([](auto &&o){ return static_cast<T>(o); });

inline auto by_name(const QString &name)
{
    return v::filter([name](auto &&obj) { return obj->objectName() == name; });
}

inline auto by_re(const QRegularExpression &re)
{
    return v::filter([re](auto &&obj) { return re.match(obj->objectName()).hasMatch(); });;
}

}  // namespace qt

#endif // QRANGE_H
