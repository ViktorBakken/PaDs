#pragma once

#include "adt.hpp"

struct TreiberStackNode {
  int value;
  TreiberStackNode *next = nullptr;

  TreiberStackNode(int val = 0) : value(val), next(nullptr) {}
};

class TreiberStack : public Stack {
private:
  std::atomic<TreiberStackNode *> top{nullptr}; // Initialize to nullptr
  EventMonitor<TreiberStack, StdStack, StackOperator> *monitor;
  std::mutex cas_lock;

public:
  TreiberStack(EventMonitor<TreiberStack, StdStack, StackOperator> *monitor)
      : monitor(monitor) {}

  ~TreiberStack() {
    TreiberStackNode *current = top.load();
    while (current != nullptr) {
      TreiberStackNode *toDelete = current;
      current = current->next;
      delete toDelete;
    }
  }

  int push(int value) override {
    int result = true;
    TreiberStackNode *n = new TreiberStackNode(value);

    while (true) {
      TreiberStackNode *t = top.load();
      n->next = t;

      cas_lock.lock();
      if (top.compare_exchange_strong(t, n)) { // Use atomic CAS
        monitor->add(StackEvent(StackOperator::StackPush, value, result));
        cas_lock.unlock();
        break;
      }
      cas_lock.unlock();
      // Retry
    }

    return result;
  }

  int pop() override {
    int result = EMPTY_STACK_VALUE;

    while (true) {
      cas_lock.lock();
      TreiberStackNode *t = top.load();

      if (t == nullptr) {
        monitor->add(
            StackEvent(StackOperator::StackPop, NO_ARGUMENT_VALUE, result));
        cas_lock.unlock();
        return result;
      }

      if (top.compare_exchange_strong(t, t->next)) { // Use atomic CAS
        result = t->value;
        delete t;
        monitor->add(
            StackEvent(StackOperator::StackPop, NO_ARGUMENT_VALUE, result));
        cas_lock.unlock();
        return result;
      }
      cas_lock.unlock();
    }
  }

  int size() override {
    int result = 0;

    cas_lock.lock();
    TreiberStackNode *t = top.load();
    while (t != nullptr) {
      result++;
      t = t->next;
    }
    monitor->add(
        StackEvent(StackOperator::StackSize, NO_ARGUMENT_VALUE, result));
    cas_lock.unlock();

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
