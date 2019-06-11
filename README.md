## General information
Some experiments of using QObject with ranges for iterating through its children. This is only a proof of concept, so proper forward iterator for tree is not implemented. It means, that the view is not that lazy now. But it have to be of course.
See test folder for further details.

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
* Users need to get used to this syntax and keep all of the limitations and in mind

## Build instructions
```
git clone --recursive git@git.qt.io:vifanask/qobject-with-ranges.git
cd qobject-with-ranges/test
mkdir build
cd build
cmake ..
make
```
Tested on Linux with GCC only.
