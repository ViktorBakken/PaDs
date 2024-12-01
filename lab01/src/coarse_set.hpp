#pragma once

#include "set.hpp"
#include "std_set.hpp"

#include <mutex>

/// The node used for the linked list implementation of a set in the
/// [`CoarseSet`] class. This struct is used for task 3
struct CoarseSetNode {
  // A03: You can add or remove fields as needed.
  int value;
  CoarseSetNode *next;
};

/// A set implementation using a linked list with coarse grained locking.
class CoarseSet : public Set {
private:
  // A03: You can add or remove fields as needed. Just having the `head`
  // pointer and the `lock` should be sufficient for task 3
  CoarseSetNode *head;
  std::mutex lock;
  EventMonitor<CoarseSet, StdSet, SetOperator> *monitor;

public:
  CoarseSet(EventMonitor<CoarseSet, StdSet, SetOperator> *monitor)
      : monitor(monitor) {
    // A03: Initiate the internal state
    this->head = new CoarseSetNode(INT_MIN,nullptr);
  }

  ~CoarseSet() override {
    // A03: Cleanup any memory that was allocated
    CoarseSetNode *current = this->head;
    while (current != nullptr) {
      CoarseSetNode *old = current;
      current = current->next;
      delete old;
    }
  }


  bool add(int elem) override {
    bool result = false;

    this->lock.lock();
    CoarseSetNode *current = this->head;

    while (current->next != nullptr && current->value != elem) {
      int nextValue = current->next->value;
      if (elem < nextValue) {
        // Insert new element in the correct position
        current->next = new CoarseSetNode(elem, current->next);
        result = true;
        break;
      }
      current = current->next;
    }

    // If we reach the end of the list, insert the new element
    if (!result && current->value != elem) {
      current->next = new CoarseSetNode(elem, nullptr);
      result = true;
    }

    this->monitor->add(SetEvent(SetOperator::Add, elem, result));
    this->lock.unlock();
    return result;
  }

  CoarseSetNode *RemoveNode(CoarseSetNode *remNode) {
    CoarseSetNode *nextNode = remNode->next;
    delete (remNode);
    return nextNode;
  }

  bool rmv(int elem) override {
    bool result = false;
    this->lock.lock();
    CoarseSetNode *current = this->head;
    int nextValue = INT_MIN;

    while (current->next != nullptr && elem >= nextValue) {
      nextValue = current->next->value;
      if (nextValue == elem) {
        // Remove element in the correct position
        current->next = RemoveNode(current->next);
        result = true;
        break;
      }
      current = current->next;
    }

    this->monitor->add(SetEvent(SetOperator::Remove, elem, result));
    this->lock.unlock();

    return result;
  }

  bool ctn(int elem) override {
    bool result = false;
    this->lock.lock();
    CoarseSetNode *current = this->head;

    while (current != nullptr && elem >= current->value) {
      if (current->value == elem) {
        // Set true if value exist in list
        result = true;
        break;
      }
      current = current->next;
    }

    this->monitor->add(SetEvent(SetOperator::Contains, elem, result));
    this->lock.unlock();

    return result;
  }

  void print_state() override {
    std::cout << "CoarseSet {...}";
  }
};
