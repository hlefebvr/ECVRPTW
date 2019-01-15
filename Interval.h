#ifndef ECVRPTW_INTERVAL_H
#define ECVRPTW_INTERVAL_H

#include <iostream>
#include <algorithm>

using namespace std;

class Interval {
    double _from = 0;
    double _to = numeric_limits<double>::max();
    void check() const;
public:
    Interval();
    Interval(double a, double b);
    Interval operator-(const Interval& rhs) const;
    bool operator<(const Interval& rhs) const;
    bool operator<<(const Interval& rhs) const;
    bool operator==(const Interval& rhs) const;
    Interval& operator=(const Interval& rhs);
    bool intersection_is_empty(const Interval& rhs) const;
    double span() const;
    double from() const;
    double from(double a);
    double to() const;
    double to(double b);
};



#endif //ECVRPTW_INTERVAL_H
