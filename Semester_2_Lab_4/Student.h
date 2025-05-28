#pragma once
#include "Person.h"
#include <iostream>

class Student : public Person {
private:
    string group;

public:
    Student(string fn, string mn, string ln, time_t bd, PersonID pid, string group)
        : Person(fn, mn, ln, bd, pid), group(group) {}

    string GetGroup() const { return group; }
};

inline ostream& operator<<(ostream& os, const Student& student) {
    return os << student.GetFullName() << " (Group: " << student.GetGroup() << ")";
}
