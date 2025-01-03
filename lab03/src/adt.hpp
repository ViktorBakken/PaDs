#pragma once

class Set {
   public:
    /// The deconstructor can be used to deallocate memory, once the Set is no
    /// longer needed.
    virtual ~Set() {};

    /// This method adds the given element into the set. It will return
    /// `true`, if the insertion was successful, `false` otherwise. An
    /// insertion might fail, if the set already contains the given element.
    virtual bool add(int elem) = 0;

    /// This method removes the given element from the set. It will return
    /// `true`, if the element was in the set and was removed, `false` otherwise.
    virtual bool rmv(int elem) = 0;

    /// This method checks if a given element is inside the set. It returns
    /// `true` if the element is present, `false` otherwise.
    virtual bool ctn(int elem) = 0;

    /// This method can be overwritten to provide a better debug message.
    /// This is not part of the assignment and only intended to help with debugging.
    virtual void print_state() {};
};

class Multiset {
   public:
    /// The deconstructor can be used to deallocate memory, once the Set is no
    /// longer needed.
    virtual ~Multiset() {};

    /// This method adds the given element into the set and returns `true`
    virtual int add(int elem) = 0;

    /// This method removes the given element from the set. It will return
    /// `true`, if the element was in the set and was removed, `false` otherwise.
    virtual int rmv(int elem) = 0;

    /// Returns the count of how many elements are in the set.
    virtual int ctn(int elem) = 0;

    /// This method can be overwritten to provide a better debug message.
    /// This is not part of the assignment and only intended to help with debugging.
    virtual void print_state() {};
};

const int EMPTY_STACK_VALUE = -1;

class Stack {
   public:
    /// The deconstructor can be used to deallocate memory, once the Stack is no
    /// longer needed.
    virtual ~Stack() {};

    /// This method adds the given element to the top of the stack and returns `true` (1).
    virtual int push(int elem) = 0;

    /// Removes the top-most stack element and returns it. If the stack is empty, the
    /// special value `EMPTY_STACK_VALUE` (-1) is returned.
    virtual int pop() = 0;

    /// Returns the size of the stack, meaning how many elements are currently in the stack.
    virtual int size() = 0;

    /// This method can be overwritten to provide a better debug message.
    /// This is not part of the assignment and only intended to help with debugging.
    virtual void print_state() {};
};
