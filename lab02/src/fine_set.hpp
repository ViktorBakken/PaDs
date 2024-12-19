#pragma once

#include "set.hpp"
#include "std_set.hpp"

#include <mutex>

// A03: Copy your `FineSet` implementation from Lab 01 into this file and
// remove all references to the monitor. We want to benchmark the data
// structure and monitoring the performed operation would influence the
// results.

struct FineSetNode {
  int value;
  FineSetNode *next;
  std::mutex lock;

  FineSetNode(int elem, FineSetNode *nextN) : value(elem), next(nextN) {}
};

class FineSet : public Set {
private:
  FineSetNode *head;

public:
  /// Initiate the internal state
  FineSet() {
    head = new FineSetNode(INT_MIN, nullptr);
  }

  /// Destructor to clean up allocated nodes.
  ~FineSet() override {
    FineSetNode *current = head;
    while (current != nullptr) {
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

    if (next != nullptr)
      next->lock.unlock();
    current->lock.unlock();

    return result;
  }

  void print_state() override { std::cout << "FineSet {...}"; }
};
