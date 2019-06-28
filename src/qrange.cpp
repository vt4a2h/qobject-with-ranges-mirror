#include "qrange.h"

#include <QtCore/private/qobject_p.h>


// #define DEBUG_IDDFS
#ifdef DEBUG_IDDFS
#  define qdebug(format, ...) qDebug(format, ##__VA_ARGS__)
#else
#  define qdebug(format, ...)
#endif

namespace qt
{

namespace detail {

cursor::cursor(const QObject *o, Qt::FindChildOptions opts)
    : root{o},
      opts{opts}
{
    qdebug("cursor ctor");
    if (root) {
      const auto &children = QObjectPrivate::get(root)->children;
      if (children.size())
        current = children.at(0);
    }
}

QObject *cursor::read() const noexcept
{
    qdebug("=========read: %s============",
          current ? qPrintable(current->objectName()) : nullptr);

    return current;
}

struct DFSRet {
    const QObjectPrivate *found;
    bool remaining;
};

DFSRet dls(const QObjectPrivate *node, const int depth, int &index, int &min) {
    qdebug("  dls, node: %s, depth: %d, index: %d",
          node && node->extraData ? qPrintable(node->extraData->objectName) : "nullptr",
          depth, index);

    if (depth == 0) {
      if (index == 0) {
        return {node, true};
      }

      qdebug("    index--");
      index--;
      min++;
      qdebug("    min++");
      return {nullptr, true};
    }

    bool any_remaining = false;

    // int i = index;
    // if (node->children.size() > index) {
      // index = node->children.size() - index;
      // qdebug("    index: %d updated", index);
    // }

    qdebug("    children.size: %d", node->children.size());
    if (depth - 1 == 0) {
      if (index >= node->children.size()) {
        qdebug("   ooopsss");
        index -= node->children.size();
        min += node->children.size();
        return {nullptr, true};
      }

      qdebug("   depth == 0");
      int ff = 0;
      for (int i = index; i < node->children.size(); i++) {
          const auto &res = dls(QObjectPrivate::get(node->children.at(i)), depth - 1, ff, min);
          if (res.found)
              return {res.found, true};
          if (res.remaining)
              any_remaining = true; // (At least one node found at depth, let IDDFS deepen)
      }

      // return {nullptr, true};
    }

    for (const auto &child : qAsConst(node->children)) {
        const auto &res = dls(QObjectPrivate::get(child), depth - 1, index, min);
        if (res.found)
            return {res.found, true};
        if (res.remaining)
            any_remaining = true; // (At least one node found at depth, let IDDFS deepen)
    }

    return {nullptr, any_remaining};
}

const QObjectPrivate *iddfs(const QObjectPrivate *root, const int index, int &last_depth, int &mm) {
    qdebug("iddfs:");
    qdebug("  index:      %d", index);
    qdebug("  last_depth: %d", last_depth);
    qdebug("  mm:         %d", mm);
    mm = 0;
    int i = index;
    int depth = last_depth;
    for (; depth < index + 2; depth++) {
        // int min = 0;
        const auto &res = dls(root, depth, i, mm);
        if (res.found) {
            if (last_depth < depth)
             qdebug("  last_depth++");

           last_depth = depth;
           // qDebug() << "  min:" << mm;
           // mm = min;
           return res.found;
        }

        if (!res.remaining)
           return nullptr;
    }

    return nullptr;
}

/*
       root
         |
       / | \
      /  |  \
     /   |   \
   1.0  1.1 1.2
    |          \
   /            \
  /              \
 2.0             2.1
*/

void cursor::next() noexcept
{
    qdebug("next:");
    qdebug("  index: %d", index);
    int current_depth = last_depth;
    auto p = iddfs(QObjectPrivate::get(root), index, last_depth, new_index);
    if (!p) {
        current = nullptr;
        return;
    }

    current = p->q_ptr;

    if (last_depth > current_depth) {
      qdebug("  new_index: %d", new_index);
      index -= new_index;
    }

    index++;
}

bool cursor::equal(ranges::default_sentinel_t) const noexcept
{
    return current == nullptr;
}

bool cursor::equal(const cursor &that) const noexcept
{
    qDebug() << "equal 2";
    return true;
}

}  // namespace detail

}  // namespace qt
