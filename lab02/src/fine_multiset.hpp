#pragma once

#include "set.hpp"
#include "std_set.hpp"

#include <mutex>

/// The node used for the linked list implementation of a multiset in the
/// [`FineMultiset`] class. This struct is used for task 4.
struct FineMultisetNode {
  // A06: You can add or remove fields as needed.
  int value;
  FineMultisetNode *next;
  std::mutex lock;

  FineMultisetNode(int elem = INT_MIN, FineMultisetNode *nextN = nullptr)
      : value(elem), next(nextN) {}
};

/// A multiset implementation using a linked list with fine grained locking.
class FineMultiset : public Multiset {
private:
  // A06: You can add or remove fields as needed.
  FineMultisetNode *head;
  FineMultisetNode *tail;
  EventMonitor<FineMultiset, StdMultiset, MultisetOperator> *monitor;

public:
  FineMultiset(
      EventMonitor<FineMultiset, StdMultiset, MultisetOperator> *monitor)
      : monitor(monitor) {
    // A06: Initiate the internal state
    this->tail = new FineMultisetNode(INT_MAX, nullptr);
    this->head = new FineMultisetNode(INT_MIN, this->tail);
  }

  ~FineMultiset() override {
    // A06: Cleanup any memory that was allocated
    FineMultisetNode *current = head;
    while (current != nullptr) {
      FineMultisetNode *toDelete = current;
      current = current->next;
      delete toDelete;
    }
  }

  int add(int elem) override {
    int result = true;
    // A06: Add code to insert the element into the set.
    //      Make sure, to insert the event inside the locked region of
    //      the linearization point.

    // Lock
    head->lock.lock();
    FineMultisetNode *current = head;
    FineMultisetNode *next = current->next;
    next->lock.lock();

    // Traversing list to correct position
    while (next->value < elem) {
      current->lock.unlock();
      current = next;
      next = next->next;
      next->lock.lock();
    }

    // Insert element
    current->next = new FineMultisetNode(elem, next);

    // Record operation
    this->monitor->add(MultisetEvent(MultisetOperator::MSetAdd, elem, result));

    // Unlock
    next->lock.unlock();
    current->lock.unlock();

    return result;
  }

  int rmv(int elem) override {
    int result = false;
    // A06: Add code to remove the element from the set and update `result`.
    //      Also make sure, to insert the event inside the locked region of
    //      the linearization point.

    // Lock
    head->lock.lock();
    FineMultisetNode *current = head;
    FineMultisetNode *next = head->next;
    next->lock.lock();

    // Traversing list to correct position
    while (next->value < elem) {
      current->lock.unlock();
      current = next;
      next = next->next;
      next->lock.lock();
    }

    // Remove element if found
    if (next->value == elem) {
      current->next = next->next;
      delete next;
      result = true;
    }

    // Record operation
    this->monitor->add(
        MultisetEvent(MultisetOperator::MSetRemove, elem, result));

    // Unlock
    current->lock.unlock();
    next->lock.unlock();

    return result;
  }

  int ctn(int elem) override {
    int result = 0;
    // A06: Add code to count how often elem is inside the set and update
    // `result`.
    //      Also make sure, to insert the event inside the locked region of
    //      the linearization point.
    //
    //      There are different ways to implement a multiset ADT. The
    //      skeleton code provides `monitor->add()`, `monitor->reserve()`,
    //      and `event->complete()` functions for this purpose. One can
    //      use only `monitor->add() or a combination of `monitor->reserve()`
    //      and `event->complete()` depending on their multiset
    //      implementation. Go to `monitoring.hpp` and see the descriptions
    //      of these function.

    // Lock list from new threads
    head->lock.lock();

    // Lock next step
    FineMultisetNode *current = head->next;
    current->lock.lock();

    // Traverse list
    while (current != nullptr) {
      if (current->value == elem) { // Count found value
        result++;
      }

      // Take a step
      FineMultisetNode *next = current->next;
      if (next != nullptr)
        next->lock.lock();

      // Unlock previous step
      current->lock.unlock();
      current = next;
    }

    // Record operation
    this->monitor->add(
        MultisetEvent(MultisetOperator::MSetCount, elem, result));

    // Unlock list
    head->lock.unlock();
    return result;
  }

  void print_state() override {
    // A06: Optionally, add code to print the state. This is useful for
    // debugging, but not part of the assignment
    std::cout << "FineMultiset:{";
    FineMultisetNode *current = head->next;
    while (current != nullptr) {
      std::cout << current->value << ", ";
      current = current->next;
    }
    std::cout << "}";
  }
};
