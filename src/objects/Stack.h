#ifndef STACK_H
#define STACK_H

#include <stdexcept>  // For std::out_of_range

/**
 * A stack implementation using a linked list with no fixed size.
 *
 * @tparam T  The type of elements stored in the stack.
 */
template <typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& data, Node* next = nullptr) : data(data), next(next) {}
    };
    
    Node* topNode;   // Pointer to the top node of the stack
    size_t currentSize;  // Current number of elements in the stack

public:
    Stack() : topNode(nullptr), currentSize(0) {}

    // Destructor to free all nodes
    ~Stack() {
        while (!empty()) {
            pop();
        }
    }

    // Copy constructor
    Stack(const Stack& other) : topNode(nullptr), currentSize(0) {
        if (!other.empty()) {
            Node* otherCurrent = other.topNode;
            Stack temp;
            while (otherCurrent != nullptr) {
                temp.push(otherCurrent->data);
                otherCurrent = otherCurrent->next;
            }
            // Reverse the temp stack to get original order
            while (!temp.empty()) {
                push(temp.top());
                temp.pop();
            }
        }
    }

    // Assignment operator
    Stack& operator=(const Stack& other) {
        if (this != &other) {
            Stack temp(other);
            std::swap(topNode, temp.topNode);
            std::swap(currentSize, temp.currentSize);
        }
        return *this;
    }

    // Push an item onto the stack
    void push(const T &item) {
        topNode = new Node(item, topNode);
        ++currentSize;
    }

    // Pop the top item from the stack
    void pop() {
        if (empty()) throw std::out_of_range("Stack underflow");
        Node* toDelete = topNode;
        topNode = topNode->next;
        delete toDelete;
        --currentSize;
    }

    // Access the top item of the stack
    T &top() {
        if (empty()) throw std::out_of_range("Stack is empty");
        return topNode->data;
    }

    // Const version of top()
    const T &top() const {
        if (empty()) throw std::out_of_range("Stack is empty");
        return topNode->data;
    }

    // Check if the stack is empty
    bool empty() const { return topNode == nullptr; }

    // Get the current size of the stack
    size_t size() const { return currentSize; }
};

#endif  // STACK_H
