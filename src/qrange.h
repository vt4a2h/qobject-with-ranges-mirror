#ifndef QRANGE_H
#define QRANGE_H

#ifdef COROUTINES_SUPPORT
#include <qrangecoro_p.h>
#else
#include <qrangefallback_p.h>
#endif // COROUTINES_SUPPORT

#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

namespace qt
{
    namespace r = ranges;
    namespace v = r::view;

    constexpr auto children = ranges::make_pipeable([](auto &&o) { return detail::children_view(o); });

    constexpr auto find_children(Qt::FindChildOptions opts = Qt::FindChildrenRecursively)
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
