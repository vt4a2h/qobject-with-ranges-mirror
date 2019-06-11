#include <vector>

#include <range/v3/algorithm/find.hpp>

#include "FooBar.h"

#include "qrange.h"

int main(int argc, char *argv[])
{
    QObject root;

    auto l1 = new QObject(&root);
    auto l2 = new QObject(&root);

    auto l3 = new Foo(l1);
    l3->setObjectName("foo");

    auto l4 = new Bar(l2);
    l4->setObjectName("baz");

    {
        std::vector<QObject*> children = root | qt::children;
        assert(children.size() == 4);
    }

    {
        std::vector<QObject*> children = root | qt::find_children(Qt::FindDirectChildrenOnly);
        assert(children.size() == 2);
        assert(ranges::find(children, l1) != ranges::end(children));
        assert(ranges::find(children, l2) != ranges::end(children));
    }

    {
        std::vector<QObject*> children = root | qt::children | qt::with_type<Foo*>;
        assert(children.size() == 1);
        assert(children.at(0) == l3);
    }

    {
        std::vector<QObject*> children = root | qt::children | qt::by_name("foo");
        assert(children.size() == 1);
        assert(children.at(0) == l3);
    }

    {
        std::vector<QObject*> children = root | qt::children | qt::by_re(QRegularExpression("^b"));
        assert(children.size() == 1);
        assert(children.at(0) == l4);
    }

    return 0;
}
