#ifndef BENCHMARKS_H
#define BENCHMARKS_H

#ifdef NDEBUG
#include <chrono>
#include <iostream>
#include <memory>
#include <stack>

#include "qrange.h"

template <class F, class D>
void measureTime(const std::string &s, F f, D d, std::size_t times)
{
    double time = 0;

    for (std::size_t t = 0; t < times; ++t) {
        auto data = d();

        auto start = std::chrono::system_clock::now();
        f(data);
        auto end = std::chrono::system_clock::now();

        std::chrono::duration<double, std::milli> elapsed = end - start;
        time += elapsed.count();
    }

    std::cout << s << ": " << (time / times) << " msec" << " (run " << times << " times)" << std::endl;
}

std::shared_ptr<QObject> genTree(int depth = 22, std::size_t nodeChildrenCount = 2)
{
    auto root = std::make_shared<QObject>();

    std::stack<QObject*> s;
    s.push(root.get());

    while (depth--) {

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

void setChildrenNamesOld(const std::shared_ptr<QObject> &obj)
{
    for (auto &&c : obj->findChildren<QObject*>()) {
        c->setObjectName("foo");
    }
}

void setChildrenNamesNew(const std::shared_ptr<QObject> &obj)
{
    for (auto &&c : obj.get() | qt::find_children(Qt::FindChildrenRecursively)) {
        c->setObjectName("foo");
    }
}

void setChildrenNamesNewWithCopy(const std::shared_ptr<QObject> &obj)
{
    const std::vector<QObject*> d = obj.get() | qt::find_children(Qt::FindChildrenRecursively);
    for (auto &&c : d) {
        c->setObjectName("foo");
    }
}

void runBenchmarks()
{
    auto gen = []{ static auto tree = genTree(); return tree; };

    measureTime("Old"     , &setChildrenNamesOld        , gen, 10);
    measureTime("New"     , &setChildrenNamesNew        , gen, 10);
    measureTime("New copy", &setChildrenNamesNewWithCopy, gen, 10);
}
#endif // NDEBUG

#endif // BENCHMARKS_H
