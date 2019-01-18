#ifndef ECVRPTW_ITERATEDLOCALSEARCH_H
#define ECVRPTW_ITERATEDLOCALSEARCH_H

#include <vector>
#include <set>

#include "SolutionCallback.h"
#include "StationSchedule.h"

using namespace std;

class IteratedLocalSearch {
    SolutionCallback& _cb;
    RelaxedSolution& _x;

    struct Segment {
        Segment(const Node& i, const Node& j, double max_arrival_j, double release_date_j, double st_j)
            : i(i), j(j), max_arrival_j(max_arrival_j), release_date_j(release_date_j), service_time_j(st_j) {}
        const Node& i;
        const Node& j;
        double max_arrival_j, release_date_j, service_time_j;
    };

    map<int, const vector<double>*> _max_arrival;

    Segment segment(const Route& route, unsigned long int i) const;
public:
    IteratedLocalSearch(SolutionCallback& cb, RelaxedSolution& x);
    ~IteratedLocalSearch();
    void run();
};


#endif //ECVRPTW_ITERATEDLOCALSEARCH_H
