#ifndef STACK_H
#define STACK_H

#include <stdexcept> // For std::out_of_range

/**
 * A stack implementation with a fixed maximum size.
 *
 * @tparam T        The type of elements stored in the stack.
 * @tparam MaxSize  The maximum number of elements the stack can hold.
 */
template <typename T, size_t MaxSize>
class Stack
{
private:
    T elements[MaxSize]; // Array to store stack elements
    size_t currentSize;  // Current number of elements in the stack

public:
    Stack() : currentSize(0) {}

    // Push an item onto the stack
    void push(const T &item)
    {
        if (isFull())
            throw std::out_of_range("Stack overflow");
        elements[currentSize++] = item;
    }

    // Pop the top item from the stack
    void pop()
    {
        if (isEmpty())
            throw std::out_of_range("Stack underflow");
        --currentSize;
    }

    // Access the top item of the stack
    T &top()
    {
        if (isEmpty())
            throw std::out_of_range("Stack is empty");
        return elements[currentSize - 1];
    }

    // Check if the stack is empty
    bool isEmpty() const
    {
        return currentSize == 0;
    }

    // Check if the stack is full
    bool isFull() const
    {
        return currentSize >= MaxSize;
    }

    // Get the current size of the stack
    size_t size() const
    {
        return currentSize;
    }
};

#endif // STACK_H