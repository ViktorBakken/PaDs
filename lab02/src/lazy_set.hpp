#pragma once

#include "set.hpp"
#include "std_set.hpp"

#include <atomic>
#include <climits>
#include <mutex>

/// The node used for the linked list implementation of a set in the [LazySet]
/// class. This struct is used for task 3
struct LazySetNode {
  int value;

  /// The next pointer and the mark should actually be an
  /// std::atomic<OptimisticNextPointer*>. Using a normal pointer will probably
  /// work in most cases, but it is undefined behaviour to read and write to a
  /// non atomic data type simultaneously. In this lab it is okay to use a
  /// normal pointer but in future labs this will be a hard requirement.
  ///
  /// See this Stack Overflow thread for explonations of atomics in c++:
  /// <https://stackoverflow.com/questions/31978324/what-exactly-is-stdatomic>.
  /// See also the documentation for std::atomic:
  /// <https://en.cppreference.com/w/cpp/atomic/atomic>
  std::atomic<bool> mark;
  std::atomic<LazySetNode *> next;

  std::mutex lock;

  /// Default constructor which sets value, mark, and next to initial values.
  LazySetNode() : value(0), mark(false), next(nullptr) {}

  /// Parameterized constructor for easier initialization.
  LazySetNode(int val = 0, bool m = false, LazySetNode *nxt = nullptr)
      : value(val), mark(m), next(nxt) {}
};

/// A set implementation using a linked list with optimistic synchronization.
class LazySet : public Set {
private:
  // A02: You can add or remove fields as needed. Just having the `head`
  // pointer should be sufficient for this task
  LazySetNode *head;
  LazySetNode *tail;

public:
  LazySet() {
    // A02: Initiate the internal state
    this->tail = new LazySetNode(2147483647, false,
                                 nullptr); // 2147483647 = INT_MAX (32-bit)

    this->head = new LazySetNode(-2147483648, false,
                                 tail); //-2147483648 = INT_MIN (32-bit)
  }

  ~LazySet() override {
    // Cleanup code if needed (optional for this exercise)
  }

private:
  /// Locate function used for lazy synchronization.
  /// Returns a pair of nodes (previous, current).
  std::pair<LazySetNode *, LazySetNode *> locate(int value) {
    LazySetNode *current;
    LazySetNode *next;

    while (true) {
      current = head;
      next = current->next.load();

      // Traversing list to correct position
      while (next->value < value) {
        current = next;
        next = next->next.load();
      }

      // Lock
      current->lock.lock();
      next->lock.lock();

      // Check availability
      if (!current->mark.load() && !next->mark.load() &&
          current->next.load() == next) {
        return {current, next};
      }
      // Unlock
      current->lock.unlock();
      next->lock.unlock();
    }
  }

public:
  bool add(int elem) override {
    bool result = false;
    // A02: Add code to insert the element into the set and update `result`.

    // Find position of element
    auto [current, next] = locate(elem);

    if (next->value != elem) {
      current->next.store(new LazySetNode(elem, false, next));
      result = true;
    }

    // Unlock
    current->lock.unlock();
    next->lock.unlock();
    return result;
  }

  bool rmv(int elem) override {
    bool result = false;
    // A02: Add code to remove the element from the set and update `result`.

    // Find position of element
    auto [current, next] = locate(elem);

    if (next->value == elem) {
      next->mark.store(true);
      current->next.store(next->next.load());
      result = true;
    }

    // Unlock
    current->lock.unlock();
    next->lock.unlock();
    return result;
  }

  bool ctn(int elem) override {
    bool result = false;
    // A02: Add code to check if the element is inside the set and update
    // `result`.
    LazySetNode *current = head;

    // traverse
    while (current->value < elem) {
      current = current->next.load();
    }

    // Evaluate value
    result = !current->mark.load() && current->value == elem;
    return result;
  }

  void print_state() override {
    // Optional debug function
    std::cout << "LazySet {...}" << std::endl;
  }
};
