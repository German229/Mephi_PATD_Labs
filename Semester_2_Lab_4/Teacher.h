#pragma once
#include "Person.h"
#include <iostream>

class Teacher : public Person {
private:
    string department;

public:
    Teacher(string fn, string mn, string ln, time_t bd, PersonID pid, string dept)
        : Person(fn, mn, ln, bd, pid), department(dept) {}

    string GetDepartment() const { return department; }
};

inline ostream& operator<<(ostream& os, const Teacher& teacher) {
    return os << teacher.GetFullName() << " (Department: " << teacher.GetDepartment() << ")";
}