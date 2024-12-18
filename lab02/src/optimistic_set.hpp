#pragma once

#include "set.hpp"
#include "std_set.hpp"

#include <atomic>
#include <mutex>

#include <memory> // For smart pointers

/// The node used for the linked list implementation of a set in the
/// [`OptimisticSet`] class. This struct is used for task 3
struct OptimisticSetNode {
  // A01: You can add or remove fields as needed.
  int value;

  /// The next pointer should actually be an
  /// std::atomic<OptimisticNextPointer*>.  Using a normal pointer will probably
  /// work in most cases, but it is undefined behaviour to read and write to a
  /// non atomic data type simultaneously. In this lab it is okay to use a
  /// normal pointer but in future labs this will be a hard requirement.
  ///
  /// See this Stack Overflow thread for explonations of atomics in c++:
  /// <https://stackoverflow.com/questions/31978324/what-exactly-is-stdatomic>.
  /// See also the documentation for std::atomic:
  /// <https://en.cppreference.com/w/cpp/atomic/atomic>
  std::atomic<OptimisticSetNode *> next;

  std::mutex lock;

  OptimisticSetNode(int elem = 0, OptimisticSetNode *nextN = nullptr)
      : value(elem), next(nextN) {}
};

/// A set implementation using a linked list with optimistic syncronization.
class OptimisticSet : public Set {
private:
  // A01: You can add or remove fields as needed. Just having the `head`
  // pointer should be sufficient for this task
  OptimisticSetNode *head;
  OptimisticSetNode *last;

public:
  OptimisticSet() {
    // A01: Initiate the internal state
    this->last = new OptimisticSetNode(
        2147483647, nullptr); // 2147483647 = INT_MAX (32-bit)
    this->head = new OptimisticSetNode(-2147483648,
                                       last); //-2147483648 = INT_MIN (32-bit)
  }

  ~OptimisticSet() override {
    // A01: Cleanup any memory that was allocated
    // This is optional for the optimistic set since it might be tricky to
    // implement and is out of scope for the exercise, but remember to document
    // this in your report.
  }

private:
  bool validate(OptimisticSetNode *p, OptimisticSetNode *c) {
    // A01: Implement the `validate` function used during
    // optimistic synchronization.

    OptimisticSetNode *current = head;
    OptimisticSetNode *next = current->next;

    // Traverse to the correct position
    while (next != nullptr && next->value < c->value) {
      current = next;
      next = next->next;
    }

    return current == p && next == c;
  }

public:
  bool add(int elem) override {
    bool result = false;
    // A01: Add code to insert the element into the set and update result.

    OptimisticSetNode *current = head;
    OptimisticSetNode *next = current->next.load();

    // Traverse to the correct position
    while (next->value < elem) {
      current = next;
      next = next->next.load();
    }

    // Lock
    current->lock.lock();
    next->lock.lock();

    // If postion exists
    if (validate(current, next)) {
      if (next->value > elem) { // Add element if element exist
        current->next.store(new OptimisticSetNode(elem, next));
        result = true;
      }
    }

    current->lock.unlock();
    next->lock.unlock();

    return result;
  }

  bool rmv(int elem) override {
    bool result = false;
    // A01: Add code to remove the element from the set and update `result`.
    OptimisticSetNode *current = head;
    OptimisticSetNode *next = current->next.load();

    // Traverse to the correct position
    while (next->value < elem) {
      current = next;
      next = next->next.load();
    }

    // Lock
    current->lock.lock();
    next->lock.lock();

    // Validate and remove the element
    if (validate(current, next)) {
      if (next->value == elem) {
        current->next.store(next->next.load());
        result = true;
      }
    }

    // Unlock nodes
    next->lock.unlock();
    current->lock.unlock();

    return result;
  }

  bool ctn(int elem) override {
    bool result = false;
    // A01: Add code to check if the element is inside the set and update
    // `result`.
    OptimisticSetNode *current = head;
    OptimisticSetNode *next = current->next.load();

    // Traversing list to correct position
    while (next->value <= elem) {
      current = next;
      next = next->next.load();
    }

    // Lock
    current->lock.lock();
    next->lock.lock();

    // If postion exists
    if (validate(current, next)) {
      if (current->value == elem) {
        // Element exists
        result = true;
      }
    }

    current->lock.unlock();
    next->lock.unlock();

    return result;
  }

  void print_state() override {
    // A01: Optionally, add code to print the state. This is useful for
    // debugging, but not part of the assignment
    std::cout << "Test Set\n[";

    OptimisticSetNode *current = head;
    while (current != nullptr) {
      std::cout << current->value << ", ";
      current = current->next.load();
    }
    std::cout << "]";
  }
};
