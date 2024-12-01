#pragma once

#include "set.hpp"
#include "std_set.hpp"

/// The node used for the linked list implementation of a set in the
/// [`SimpleSet`] class. This struct is used for task 2
struct SimpleSetNode {
  // A02: You can add or remove fields as needed.
  int value;
  SimpleSetNode *next;
};

/// A simple set implementation using a linked list. This class shouldn't have
// any synchronization yet.
class SimpleSet : public Set {
private:
  // A02: You can add or remove fields as needed. Just having the `head`
  // pointer should be sufficient for task 2
  SimpleSetNode *head;
  EventMonitor<SimpleSet, StdSet, SetOperator> *monitor;

public:
  SimpleSet(EventMonitor<SimpleSet, StdSet, SetOperator> *monitor)
      : monitor(monitor) {
    this->head = new SimpleSetNode(INT_MIN,nullptr);
  }

  ~SimpleSet() override {
    // A02: Cleanup any memory that was allocated
    SimpleSetNode *current = this->head;
    while (current != nullptr) {
      SimpleSetNode *old = current;
      current = current->next;
      delete old;
    }
  }


  bool add(int elem) override {
    bool result = false;
    SimpleSetNode *current = this->head;

    while (current->next != nullptr && current->value != elem) {
      int nextValue = current->next->value;
      if (elem < nextValue) {
        // Insert new element in the correct position
        current->next = new SimpleSetNode(elem, current->next);
        result = true;
        break;
      }
      current = current->next;
    }

    // If we reach the end of the list, insert the new element
    if (!result && current->value != elem) {
      current->next = new SimpleSetNode(elem, nullptr);
      result = true;
    }

    // Add the event to the monitor
    this->monitor->add(SetEvent(SetOperator::Add, elem, result));
    return result;
  }

  SimpleSetNode *RemoveNode(SimpleSetNode *remNode) {
    SimpleSetNode *nextNode = remNode->next;
    delete (remNode);
    return nextNode;
  }

  bool rmv(int elem) override {
    bool result = false;
    SimpleSetNode *current = this->head;
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
    return result;
  }

  bool ctn(int elem) override {
    bool result = false;
    SimpleSetNode *current = this->head;

    while (current != nullptr && elem >= current->value) {
      if (current->value == elem) {
        // Set true if value exist in list
        result = true;
        break;
      }
      current = current->next;
    }

    this->monitor->add(SetEvent(SetOperator::Contains, elem, result));
    return result;
  }

  void print_state() override {
    // A02: Optionally, add code to print the state. This is useful for
    // debugging, but not part of the assignment.
    // std::cout << ;
  }
};
