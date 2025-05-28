#include <cassert>
#include "dynamic_array.h"
#include "Lists.h"
#include "sequence.h"

void TestDynamicArray() {
    DynamicArray<int> arr(3);
    arr.Set(0, 10);
    arr.Set(1, 20);
    arr.Set(2, 30);

    assert(arr.GetSize() == 3);
    assert(arr.Get(0) == 10);
    assert(arr.Get(1) == 20);
    assert(arr.Get(2) == 30);

    arr.Resize(5);
    assert(arr.GetSize() == 5);
    arr.Set(3, 40);
    arr.Set(4, 50);
    assert(arr.Get(3) == 40);
    assert(arr.Get(4) == 50);
}

void TestLinkedList() {
    int data[] = {1, 2, 3};
    LinkedList<int> list(data, 3);

    assert(list.GetLength() == 3);
    assert(list.Get(0) == 1);
    assert(list.Get(1) == 2);
    assert(list.Get(2) == 3);

    list.Append(4);
    assert(list.GetLength() == 4);
    assert(list.Get(3) == 4);

    list.Prepend(0);
    assert(list.GetLength() == 5);
    assert(list.Get(0) == 0);

    list.InsertAt(2, 99);
    assert(list.GetLength() == 6);
    assert(list.Get(2) == 99);

    LinkedList<int>* sub = list.GetSubList(1, 3);
    assert(sub->GetLength() == 3);
    assert(sub->Get(0) == 1);
    assert(sub->Get(1) == 99);
    assert(sub->Get(2) == 2);
    delete sub;
}

void TestArraySequence() {
    int items[] = {5, 10, 15};
    ArraySequence<int> seq(items, 3);

    assert(seq.GetLength() == 3);
    assert(seq.Get(0) == 5);
    assert(seq.Get(2) == 15);

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
    assert(sub->Get(0) == 5);
    assert(sub->Get(1) == 10);
    assert(sub->Get(2) == 15);
    delete sub;

    int other[] = {100, 200};
    ArraySequence<int> otherSeq(other, 2);
    Sequence<int>* concat = seq.Concat(otherSeq);
    assert(concat->GetLength() == 5);
    assert(concat->Get(3) == 100);
    assert(concat->Get(4) == 200);
    delete concat;
}

void TestListSequence() {
    int items[] = {1, 2, 3};
    ListSequence<int> seq(items, 3);

    assert(seq.GetLength() == 3);
    assert(seq.Get(1) == 2);

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
    assert(sub->Get(0) == 1);
    assert(sub->Get(1) == 2);
    assert(sub->Get(2) == 3);
    delete sub;

    int other[] = {10, 11};
    ListSequence<int> otherSeq(other, 2);
    Sequence<int>* concat = seq.Concat(otherSeq);
    assert(concat->GetLength() == 5);
    assert(concat->Get(3) == 10);
    assert(concat->Get(4) == 11);
    delete concat;
}