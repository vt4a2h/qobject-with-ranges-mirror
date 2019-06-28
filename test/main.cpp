#include <vector>

#include <range/v3/algorithm/find.hpp>

#include <QScopeGuard>
#include <QElapsedTimer>
#include <QtCore/private/qobject_p.h>

#include "FooBar.h"

#include "qrange.h"

#include <QTest>

class QChildrenRangeTest : public QObject {
    Q_OBJECT

private slots:
  void children_data();
  void children();
  void children_lazy();
  void with_type();
  void bench();
};

template<typename T = QObject>
QObject *make_child(QList<QObject *> &bft_nodes,
                           QObject *root, const QString &name)
{
    auto obj = new T(root);
    obj->setObjectName(name);
    bft_nodes.append(obj);
    return obj;
};

void QChildrenRangeTest::children_data()
{
    QTest::addColumn<QList<QObject *>>("bft_nodes");

    auto make_root = [this] (QList<QObject *> &bft_nodes) {
        return make_child(bft_nodes, this, "root");
    };



/*
       root
         |
       /   \
      /     \
     /       \
   1.0      1.1
*/

    QTest::newRow("1.0 -> 1.1") << [=] () {
      QList<QObject *> bft_nodes;

      auto root = make_root(bft_nodes);

      auto l10 = make_child(bft_nodes, root, "1.0");
      auto l11 = make_child(bft_nodes, root, "1.1");
      return bft_nodes;
    }();

/*
       root
         |
       /   \
      /     \
     /       \
   1.0      1.1
    |
   /
  /
 2.0
*/
    QTest::newRow("1.0 -> 1.1 -> 2.0") << [=] () {
      QList<QObject *> bft_nodes;

      auto root = make_root(bft_nodes);

      auto l10 = make_child(bft_nodes, root, "1.0");
      auto l11 = make_child(bft_nodes, root, "1.1");
      auto l20 = make_child(bft_nodes, l10, "2.0");
      Q_UNUSED(l20);

      return bft_nodes;
    }();

/*
       root
         |
       /   \
      /     \
     /       \
   1.0      1.1
    |          \
   /            \
  /              \
 2.0             2.1
*/
  QTest::newRow("1.0 -> 1.1 -> 2.0 -> 2.1") << [=] () {
      QList<QObject *> bft_nodes;

      auto root = make_root(bft_nodes);

      auto l10 = make_child(bft_nodes, root, "1.0");
      auto l11 = make_child(bft_nodes, root, "1.1");
      auto l20 = make_child(bft_nodes, l10, "2.0");
      auto l21 = make_child(bft_nodes, l11, "2.1");

      Q_UNUSED(l20);
      Q_UNUSED(l21);

      return bft_nodes;
    }();

/*
       root
         |
       /   \
      /     \
     /       \
   1.0      1.1
    |          \
   /            \
  /              \
 2.0             2.1
 |
 3.0
*/
  QTest::newRow("1.0 -> 1.1 -> 2.0 -> 2.1 -> 3.0") << [=] () {
      QList<QObject *> bft_nodes;

      auto root = make_root(bft_nodes);

      auto l10 = make_child(bft_nodes, root, "1.0");
      auto l11 = make_child(bft_nodes, root, "1.1");
      auto l20 = make_child(bft_nodes, l10, "2.0");
      auto l21 = make_child(bft_nodes, l11, "2.1");
      auto l30 = make_child(bft_nodes, l20, "3.0");

      Q_UNUSED(l21);
      Q_UNUSED(l30);

      return bft_nodes;
    }();

/*
       root
         |
       / | \
      /  |  \
     /   |   \
   1.0  1.1 1.2
    |    |      \
   /   /   \    \
  /   2.1   2.2       \
 2.0             2.3
 |
 3.0
*/
  QTest::newRow("1.0 .. 1.2 -> 2.0 .. 2.3 -> 3.0") << [=] () {
      QList<QObject *> bft_nodes;

      auto root = make_root(bft_nodes);

      auto l10 = make_child(bft_nodes, root, "1.0");
      auto l11 = make_child(bft_nodes, root, "1.1");
      auto l12 = make_child(bft_nodes, root, "1.2");

      auto l20 = make_child(bft_nodes, l10, "2.0");
      auto l21 = make_child(bft_nodes, l11, "2.1");
      auto l22 = make_child(bft_nodes, l11, "2.2");
      auto l23 = make_child(bft_nodes, l12, "2.3");

      auto l30 = make_child(bft_nodes, l22, "3.0");

      Q_UNUSED(l21);
      Q_UNUSED(l22);
      Q_UNUSED(l23);
      Q_UNUSED(l30);

      return bft_nodes;
    }();
}

void QChildrenRangeTest::children()
{
    QFETCH(QList<QObject *>, bft_nodes);

    QVERIFY(bft_nodes.size());

    auto root = bft_nodes.takeAt(0);
    auto l10 = bft_nodes.at(1);
    auto cleanup = qScopeGuard([root] { delete root; });

    if (bft_nodes.size() != 8) {
      QSKIP("");
    }

    QCOMPARE(root->objectName(), QLatin1String{"root"});


    std::vector<QObject*> children = root | qt::children;
    QCOMPARE(children.size(), bft_nodes.size());

    for (int i = 0; i < bft_nodes.size(); i++) {
      QCOMPARE(children[i], bft_nodes.at(i));
    }

    QVERIFY(ranges::find(children, l10) != ranges::end(children));
}

void QChildrenRangeTest::children_lazy()
{
    QSKIP("FF");
    QList<QObject *> bft_nodes;

    QObject root;
    make_child(bft_nodes, &root, "1.0");
    make_child(bft_nodes, &root, "1.1");

    auto lazy = root | qt::children;

    int i = 0;
    for (auto child : lazy) {
      if (i >= bft_nodes.size()) {
        QFAIL("out of range");
      }

      qDebug() << "it";
      QCOMPARE(child, bft_nodes.at(i++));
    }
}

void QChildrenRangeTest::with_type()
{
  QSKIP("test");
    QList<QObject *> bft_nodes;

    QObject root;
    auto l10 = make_child(bft_nodes, &root, "1.0");
    auto l11 =  make_child(bft_nodes, &root, "1.1");
    auto foo = make_child<Foo>(bft_nodes, l10, "2.0(foo)");
    auto bar = make_child<Bar>(bft_nodes, l11, "2.1(bar)");

    std::vector<QObject*> children = root | qt::children | qt::with_type<Foo*>;
    QCOMPARE(children.size(), 1);
    QCOMPARE(children[0], foo);

    children = root | qt::children | qt::with_type<Bar*>;
    QCOMPARE(children.size(), 1);
    QCOMPARE(children[0], bar);
}

void QChildrenRangeTest::bench()
{
  // QSKIP("test");
  QObject root;
  for (int x = 0; x < 100; x++) {
    auto child = new QObject(&root);
    for (int y = 0; y < 1000; y++) {
      auto child_foo = new Foo(child);
      child_foo->setObjectName(QString("Foo(x = %1, y = %2)").arg(x).arg(y));
      auto child_bar = new Bar(child);
      child_bar->setObjectName(QString("Bar(x = %1, y = %2)").arg(x).arg(y));
    }
  }

  QElapsedTimer t;
  t.start();
  auto children_old = root.findChildren<Bar *>();
  for (auto c : children_old) {
    c->setObjectName("lazy");
  }
  const auto elapsed_old = t.elapsed();
  qDebug() << "children old.size:" << children_old.size();
  qDebug() << "elapsed  old:    " << elapsed_old << "ms";

  t.start();
  std::vector<QObject*> children_new = root | qt::children | qt::with_type<Foo*>;
  const auto elapsed_new = t.elapsed();

  qDebug() << "children new.size:" << children_new.size();
  qDebug() << "elapsed  new:    " << elapsed_new << "ms";

  std::vector<QObject*> children_new2;
  t.start();
  auto lazy = root | qt::children | qt::with_type<Foo*>;
  qDebug() << "elapsed lazy:" << t.elapsed();

  t.start();
  for (auto c : lazy) {
    c->setObjectName("lazy");
  }

  qDebug() << "elapsed lazy:" << t.elapsed();
}


QTEST_MAIN(QChildrenRangeTest)
#include "main.moc"
