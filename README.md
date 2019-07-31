## General information
Some experiments on using QObject with ranges for iterating through its children. There are two implementations available: 
* Non-lazy, that used as a fallback. Strictly speaking, this is old and well-known QObject::findChildren<QObject*> under the hood.
* Lazy. Coroutine-based implementation.

## Examples
```cpp
QObject root;

// Create some children...

for (auto &&c : root | qt::children) {
    // Process all children recursively
}

for (auto &&c : root |  qt::find_children(Qt::FindDirectChildrenOnly)) {
    // Process direct children only
}

for (auto &&c : root | qt::children | qt::with_type<Foo*>) {
    // Process all children with type Foo
}

for (auto &&c : root | qt::children | qt::by_name("foo")) {
    // Process all children with name "foo"
}

for (auto &&c : root | qt::children | qt::by_re(QRegularExpression("^b"))) {
    // Process all children with name starts with "b"
}

// Copy all children to std::vector
auto children = ranges::to<std::vector>(root | qt::children);

// This also works, but is going to be deprecated
std::vector<QObject*> otherChildren = root | qt::children;
```

## Pros
* Easy to add new functionality (you don't need to touch class interface)
* Operations are separated (no more long functions signature)
* Clean code (each operation is explicit, e.g., filter/by type/get)
* Clean interface (all extra methods might be eliminated)
* Header-only (everything is compiled by a user)
* Lazy evaluation (might better in terms of search something and avoiding temporary memory allocations)
* Might be implemented without any additional libraries
* Ranges are part of C++20, next step after iterators

## Cons
* Laziness (at some corner cases you probably want to preserve result range, which might be slightly less efficient)
* Users need to get used to this syntax and keep all of the limitations in mind

## Build instructions
```bash
git clone --recursive git@git.qt.io:vifanask/qobject-with-ranges.git
cd qobject-with-ranges/test
mkdir build
cd build
cmake ..
make
```

Tested on Linux and Mac with GCC and Clang. Clang is required for coroutines support.
