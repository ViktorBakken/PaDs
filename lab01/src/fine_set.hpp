#pragma once

#include "set.hpp"
#include "std_set.hpp"

#include <limits>
#include <memory> // For smart pointers
#include <mutex>

/// The node used for the linked list implementation of a set in the [`FineSet`]
/// class. This struct is used for task 4.
struct FineSetNode {
  int value;
  FineSetNode *next;
  std::mutex lock;
};

/// A set implementation using a linked list with fine grained locking.
class FineSet : public Set {
private:
  FineSetNode *head;

  EventMonitor<FineSet, StdSet, SetOperator> *monitor; ///< Event monitor

public:
  /// Initiate the internal state
  FineSet(EventMonitor<FineSet, StdSet, SetOperator> *monitor)
      : monitor(monitor) {
    head = new FineSetNode(INT_MIN);
    head->next = nullptr;
  }

  /// Destructor to clean up allocated nodes.
  ~FineSet() override {
    FineSetNode *current = head;
    while (current) {
      FineSetNode *toDelete = current;
      current = current->next;
      delete toDelete;
    }
  }

  bool add(int elem) override {
    bool result = false;

    head->lock.lock();
    FineSetNode *current = head;
    FineSetNode *next = head->next;
    if (next != nullptr)
      next->lock.lock();

    // Traversing list to correct position
    while (next != nullptr && next->value < elem) {
      current->lock.unlock();
      current = next;
      next = next->next;
      if (next != nullptr)
        next->lock.lock();
    }

    if (next == nullptr || next->value > elem) {
      // Add element if element exist
      current->next = new FineSetNode(elem, current->next);
      result = true;
    }

    monitor->add(SetEvent(SetOperator::Add, elem, result));

    if (next != nullptr)
      next->lock.unlock();
    current->lock.unlock();

    return result;
  }

  bool rmv(int elem) override {
    bool result = false;

    head->lock.lock();
    FineSetNode *current = head;
    FineSetNode *next = head->next;
    if (next != nullptr)
      next->lock.lock();

    // Traversing list to correct position
    while (next != nullptr && next->value < elem) {
      current->lock.unlock();
      current = next;
      next = next->next;
      if (next != nullptr)
        next->lock.lock();
    }

    if (next != nullptr && next->value == elem) {
      // Remove element if element exist
      current->next = next->next;
      delete next;
      result = true;
    }

    monitor->add(SetEvent(SetOperator::Remove, elem, result));

    if (next != nullptr)
      next->lock.unlock();
    current->lock.unlock();

    return result;
  }

  bool ctn(int elem) override {
    bool result = false;

    head->lock.lock();
    FineSetNode *current = head;
    FineSetNode *next = head->next;
    if (next != nullptr)
      next->lock.lock();

    // Traversing list to correct position 
    while (next != nullptr && next->value < elem) {
      current->lock.unlock();
      current = next;
      next = next->next;
      if (next != nullptr)
        next->lock.lock();
    }

    if (next != nullptr && next->value == elem) {
      // Set true if value exist in list
      result = true;
    }

    monitor->add(SetEvent(SetOperator::Contains, elem, result));

    if (next != nullptr)
      next->lock.unlock();
    current->lock.unlock();
    
    return result;
  }
};
