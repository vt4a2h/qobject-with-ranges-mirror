#ifndef FOO_H
#define FOO_H

#include <QObject>

class Foo : public QObject
{
    Q_OBJECT
public:
    explicit Foo(QObject *parent = nullptr) : QObject(parent) {}
};

class Bar : public QObject
{
    Q_OBJECT
public:
    explicit Bar(QObject *parent = nullptr) : QObject(parent) {}
};

#endif // FOO_H
