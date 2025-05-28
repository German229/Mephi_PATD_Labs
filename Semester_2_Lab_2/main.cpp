#include <iostream>
#include "TESTS.h"

int main() {
    try {
        TestDynamicArray();
        TestLinkedList();
        TestArraySequence();
        TestListSequence();
        std::cout << "\nAll tests passed successfully.\n";
    } catch (const std::exception& ex) {
        std::cerr << "Test failed: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}
