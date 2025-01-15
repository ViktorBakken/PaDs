#pragma once

#include "adt.hpp"

struct TreiberStackNode {
  // A01: You can add or remove fields as needed.
  int value;
  TreiberStackNode *next = nullptr;

  /// Parameterized constructor for easier initialization.
  TreiberStackNode(int val = 0) : value(val), next(nullptr) {}
};

class TreiberStack : public Stack {
private:
  // A04: You can add or remove fields as needed.
  std::atomic<TreiberStackNode *> top;

  EventMonitor<TreiberStack, StdStack, StackOperator> *monitor;
  /// This lock can be used around the CAS operation, to insert the
  /// operation into the monitor at the linearization point. This is
  /// just one way to do it, you can also try alternative options.
  std::mutex cas_lock;

public:
  TreiberStack(EventMonitor<TreiberStack, StdStack, StackOperator> *monitor)
      : top(nullptr), monitor(monitor) {
    // initialize the stack
  }

  ~TreiberStack() {
    // A01: Cleanup any memory that was allocated
    TreiberStackNode *current = this->top.load();
    while (current != nullptr) {
      TreiberStackNode *toDelete = current;
      current = current->next;
      delete toDelete;
    }
  }

  int push(int value) override {
    int result = true;
    // A01: Add code to insert the element at the top of the stack.
    //      Make sure, to insert the event at the linearization point.
    //      You can use the `cas_lock` to ensure that the event is
    //      inserted at the linearization point.
    TreiberStackNode *n = new TreiberStackNode(value);

    while (true) {
      TreiberStackNode *t = top.load();
      n->next = t;

      this->cas_lock.lock();
      if (top.load() == t) {
        top.store(n);
        this->monitor->add(StackEvent(StackOperator::StackPush, value, result));
        this->cas_lock.unlock();
        break;
      }
      this->cas_lock.unlock();
    }

    return result;
  }

  int pop() override {
    int result = EMPTY_STACK_VALUE; // Default value for an empty stack.
    while (true) {
      TreiberStackNode *t = top.load(); // Load the current top node.
      if (t == nullptr) {
        // Stack is empty, return the default value.
        this->cas_lock.lock();
        this->monitor->add(
            StackEvent(StackOperator::StackPop, NO_ARGUMENT_VALUE, result));
        this->cas_lock.unlock();
        return result;
      }

      this->cas_lock.lock();
      if (top.load() == t) {
        // Atomically update the top pointer to the next node.
        TreiberStackNode *topNode = top.load();
        result = topNode->value;  // Capture the value to return.
        top.store(topNode->next); // Move the top pointer to the next node.

        this->monitor->add(
            StackEvent(StackOperator::StackPop, NO_ARGUMENT_VALUE, result));
        this->cas_lock.unlock();

        delete topNode; // Free the memory of the popped node.
        return result;
      }
      this->cas_lock.unlock();
    }
  }

  int size() override {
    int result = 0;

    while (true) {
      TreiberStackNode *t = top.load();
      TreiberStackNode *current = t;

      // Traverse the stack to count the nodes.
      int count = 0;
      while (current != nullptr) {
        count++;
        current = current->next;
      }

      // Lock and verify the top has not changed during traversal.
      this->cas_lock.lock();
      if (top.load() == t) {
        result = count;
        this->monitor->add(
            StackEvent(StackOperator::StackSize, NO_ARGUMENT_VALUE, result));
        this->cas_lock.unlock();
        break;
      }
      this->cas_lock.unlock();
    }

    return result;
  }

  void print_state() override {
    std::cout << "TreiberStack { ... }\n";

    TreiberStackNode *current = top.load();
    while (current != nullptr) {
      std::cout << current->value << "\n";
      current = current->next;
    }
  }
};
