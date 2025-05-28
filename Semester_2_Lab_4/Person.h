#pragma once
#include <string>
#include <ctime>
#include <tuple>
using namespace std;

struct PersonID {
    int series;
    int number;

    bool operator<(const PersonID& other) const {
        return tie(series, number) < tie(other.series, other.number);
    }

    bool operator==(const PersonID& other) const {
        return tie(series, number) == tie(other.series, other.number);
    }
};

class Person {
protected:
    PersonID id;
    string firstName;
    string middleName;
    string lastName;
    time_t birthDate;

public:
    Person(string fn, string mn, string ln, time_t bd, PersonID pid)
        : firstName(fn), middleName(mn), lastName(ln), birthDate(bd), id(pid) {}

    virtual ~Person() = default;

    virtual string GetFullName() const {
        return lastName + " " + firstName + " " + middleName;
    }

    PersonID GetID() const { return id; }

    virtual bool operator<(const Person& other) const {
        return id < other.id;
    }

    virtual bool operator==(const Person& other) const {
        return id == other.id;
    }

    virtual bool operator>(const Person& other) const {
        return other < *this;
    }
};