#include "Interval.h"

Interval::Interval() {}

Interval::Interval(double a, double b) : _from(a), _to(b) {
    check();
}

Interval Interval::operator-(const Interval &rhs) const {
    return Interval( max(_from, rhs._from), min(_to, rhs._to) );
}

bool Interval::operator==(const Interval &rhs) const {
    return _to == rhs._to && _from == rhs._from;
}

void Interval::check() const {
    if (_from > _to) throw runtime_error("Wrong interval : from > to");
}

bool Interval::operator<(const Interval &rhs) const {
    return _from < rhs._from;
}

bool Interval::operator<<(const Interval &rhs) const {
    return rhs._from <= _from && _to <= rhs._to;
}

double Interval::span() const {
    return _to - _from;
}

double Interval::from() const {
    return _from;
}

double Interval::from(double a) {
    _from = a;
    check();
    return _from;
}

double Interval::to() const {
    return _to;
}

double Interval::to(double b) {
    _to = b;
    check();
    return _to;
}

bool Interval::intersection_is_empty(const Interval &rhs) const {
    return ( _to <= rhs._from ) || ( rhs._to <= _from );
}

Interval& Interval::operator=(const Interval &rhs) {
    _from = rhs._from;
    _to = rhs._to;
    return *this;
}
