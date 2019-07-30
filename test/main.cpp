#include <vector>
#include <set>
#include <memory>
#include <tuple>
#include <stack>

#include "FooBar.h"

#include "qrange.h"

#include <QtTest/QtTest>

class TestQRange : public QObject
{
    Q_OBJECT

public:
    using QObjectVector = std::vector<QObject*>;
    using RootPtr = std::shared_ptr<QObject>;
    using Runner = std::function<void(const TestQRange::RootPtr &)>;

private Q_SLOTS:
    void initTestCase();

    void childrenLookup();
    void childrenLookup_data();

    void benchmarks();
    void benchmarks_data();

private:
    void generateSimpleTree();
    static std::shared_ptr<QObject> generateTree(int depth, std::size_t nodeChildrenCount);

private:
    RootPtr m_simpleTreeRoot;
    QObjectVector m_simpleTreeChildren;
};

Q_DECLARE_METATYPE(TestQRange::QObjectVector);
Q_DECLARE_METATYPE(TestQRange::RootPtr);
Q_DECLARE_METATYPE(TestQRange::Runner);

void TestQRange::initTestCase() {
    generateSimpleTree();
}

void TestQRange::childrenLookup()
{
    QFETCH(QObjectVector, foundChildren);
    QFETCH(QObjectVector, expectedChildren);

    QCOMPARE(foundChildren.size(), expectedChildren.size());
    QCOMPARE(ranges::to<std::set>(foundChildren), ranges::to<std::set>(expectedChildren));
}

void TestQRange::childrenLookup_data()
{
    QTest::addColumn<QObjectVector>("foundChildren");
    QTest::addColumn<QObjectVector>("expectedChildren");

    QTest::addRow("All children")
        << QObjectVector(*m_simpleTreeRoot | qt::children)
        << m_simpleTreeChildren;

    QTest::addRow("Direct children")
        << QObjectVector(*m_simpleTreeRoot | qt::find_children(Qt::FindDirectChildrenOnly))
        << QObjectVector{m_simpleTreeChildren[0], m_simpleTreeChildren[1]};

    QTest::addRow("Children with type specified")
        << QObjectVector(*m_simpleTreeRoot | qt::children | qt::with_type<Foo*>)
        << QObjectVector{m_simpleTreeChildren[2]};

    QTest::addRow("Children with name specified")
        << QObjectVector(*m_simpleTreeRoot | qt::children | qt::by_name("foo"))
        << QObjectVector{m_simpleTreeChildren[2]};

    QTest::addRow("Children with name that matches regex")
        << QObjectVector(*m_simpleTreeRoot | qt::children | qt::by_re(QRegularExpression("^b")))
        << QObjectVector{m_simpleTreeChildren[3]};
}

void TestQRange::benchmarks()
{
    QFETCH(Runner, runner);
    QFETCH(int, depth);
    QFETCH(int, childrenPerNode);

    auto root = generateTree(depth, childrenPerNode);
    QBENCHMARK {
        runner(root);
    };
}

void TestQRange::benchmarks_data()
{
#ifndef NDEBUG
    QSKIP("It makes no sense to run any benchmarks in debug mode. Skipping...");
#endif // NDEBUG

    qInfo() << "Run with \"-iterations n\" to adjust iterations count";

    QTest::addColumn<Runner>("runner");
    QTest::addColumn<int>("depth");
    QTest::addColumn<int>("childrenPerNode");

    Runner oldF = [](const std::shared_ptr<QObject> &obj){
        for (auto &&c : obj->findChildren<QObject*>()) {
            c->setObjectName("foo");
        }
    };
    Runner newF = [](const std::shared_ptr<QObject> &obj){
        for (auto &&c : obj.get() | qt::find_children(Qt::FindChildrenRecursively)) {
            c->setObjectName("foo");
        }
    };
    Runner newCopyF = [](const std::shared_ptr<QObject> &obj) {
        const std::vector<QObject*> d = obj.get() | qt::find_children(Qt::FindChildrenRecursively);
        for (auto &&c : d) {
            c->setObjectName("foo");
        }
    };

    // Change for some experiments...
    for (int depth = 22; depth <= 22; ++depth) {
        for (int childrenPerNode = 2; childrenPerNode <= 2; ++childrenPerNode) {
            int elementsCount = childrenPerNode * (1 - std::pow(childrenPerNode, depth)) / (1 - childrenPerNode);

            QTest::addRow("Old (elements: %i)", elementsCount)
                << oldF << depth << childrenPerNode;

            QTest::addRow("New (elements: %i)", elementsCount)
                << newF << depth << childrenPerNode;

            QTest::addRow("New copy (elements: %i)", elementsCount)
                << newCopyF << depth << childrenPerNode;
        }
    }
}

void TestQRange::generateSimpleTree()
{
    m_simpleTreeRoot = std::make_shared<QObject>();

    auto l1 = new QObject(m_simpleTreeRoot.get());
    m_simpleTreeChildren.push_back(l1);
    auto l2 = new QObject(m_simpleTreeRoot.get());
    m_simpleTreeChildren.push_back(l2);

    auto l3 = new Foo(l1);
    l3->setObjectName("foo");
    m_simpleTreeChildren.push_back(l3);

    auto l4 = new Bar(l2);
    l4->setObjectName("baz");
    m_simpleTreeChildren.push_back(l4);
}

std::shared_ptr<QObject> TestQRange::generateTree(int depth, std::size_t nodeChildrenCount)
{
    auto root = std::make_shared<QObject>();

    std::stack<QObject*> s;
    s.push(root.get());

    while (depth --> 0) {
        std::stack<QObject*> layer;
        while (!s.empty()) {
            auto e = s.top();
            s.pop();

            for (std::size_t cc = 0; cc < nodeChildrenCount; ++cc) {
                layer.push(new QObject(e));
            }
        }

        s.swap(layer);
    }

    return root;
}

QTEST_MAIN(TestQRange)
#include "main.moc"
