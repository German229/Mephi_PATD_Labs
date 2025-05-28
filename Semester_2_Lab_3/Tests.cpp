#include "Tests.h"
#include "dynamic_array.h"
#include "Lists.h"
#include "sequence.h"
#include <cassert>
#include <iostream>
#include "Matrix_Mas.h"


void TestDynamicArray() {
    DynamicArray<int> arr(3);
    arr.Set(0, 1);
    arr.Set(1, 2);
    arr.Set(2, 3);
    assert(arr.Get(0) == 1);
    assert(arr.Get(1) == 2);
    assert(arr.Get(2) == 3);
    assert(arr.GetSize() == 3);
    std::cout << "DynamicArray tests passed.\n";
}

void TestLinkedList() {
    int items[] = {1, 2, 3};
    LinkedList<int> list(items, 3);
    assert(list.Get(0) == 1);
    assert(list.Get(1) == 2);
    assert(list.Get(2) == 3);
    assert(list.GetFirst() == 1);
    assert(list.GetLast() == 3);
    assert(list.GetLength() == 3);
    std::cout << "LinkedList tests passed.\n";
}

void TestArraySequence() {
    int items[] = {10, 15, 30};
    MutableArraySequence<int> seq(items, 3);
    assert(seq.Get(0) == 10);
    assert(seq.Get(2) == 30);
    assert(seq.GetFirst() == 10);
    assert(seq.GetLast() == 30);
    assert(seq.GetLength() == 3);

    Sequence<int>* appended = seq.Append(20);
    assert(appended->GetLength() == 4);
    assert(appended->Get(3) == 20);
    delete appended;

    Sequence<int>* prepended = seq.Prepend(1);
    assert(prepended->GetLength() == 4);
    assert(prepended->Get(0) == 1);
    delete prepended;

    Sequence<int>* inserted = seq.InsertAt(1, 99);
    assert(inserted->GetLength() == 4);
    assert(inserted->Get(1) == 99);
    delete inserted;

    Sequence<int>* sub = seq.GetSubsequence(0, 2);
    assert(sub->GetLength() == 3);
    delete sub;

    int other[] = {40, 50};
    MutableArraySequence<int> otherSeq(other, 2);
    Sequence<int>* concat = seq.Concat(otherSeq);
    assert(concat->GetLength() == 5);
    assert(concat->Get(3) == 40);
    delete concat;

    std::cout << "ArraySequence tests passed.\n";
}

void TestListSequence() {
    int items[] = {1, 2, 3};
    MutableListSequence<int> seq(items, 3);
    assert(seq.Get(0) == 1);
    assert(seq.Get(2) == 3);
    assert(seq.GetFirst() == 1);
    assert(seq.GetLast() == 3);
    assert(seq.GetLength() == 3);

    Sequence<int>* appended = seq.Append(4);
    assert(appended->GetLength() == 4);
    assert(appended->Get(3) == 4);
    delete appended;

    Sequence<int>* prepended = seq.Prepend(0);
    assert(prepended->GetLength() == 4);
    assert(prepended->Get(0) == 0);
    delete prepended;

    Sequence<int>* inserted = seq.InsertAt(2, 99);
    assert(inserted->GetLength() == 4);
    assert(inserted->Get(2) == 99);
    delete inserted;

    Sequence<int>* sub = seq.GetSubsequence(0, 2);
    assert(sub->GetLength() == 3);
    delete sub;

    int other[] = {5, 6};
    MutableListSequence<int> otherSeq(other, 2);
    Sequence<int>* concat = seq.Concat(otherSeq);
    assert(concat->GetLength() == 5);
    assert(concat->Get(4) == 6);
    delete concat;

    std::cout << "ListSequence tests passed.\n";
}

void TestMatrixIdentity() {
    int items[] = {1, 2, 3,
                   4, 5, 6};
    Matrix<int> A(2, 3, items);

    assert(A.getKNorm() == 9);

    Matrix<int> B = A.Transpose();
    assert(B.getKNorm() == 15);

    std::cout << "Matrix test passed.\n";
}






