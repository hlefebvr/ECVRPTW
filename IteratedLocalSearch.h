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
        Segment(const Node& i, const Node& j, double min_departure_i, double deadline_i, double st_i)
            : i(i), j(j), min_departure_i(min_departure_i), deadline_i(deadline_i), service_time_i(st_i) {}
        const Node& i;
        const Node& j;
        double min_departure_i, deadline_i, service_time_i;
    };

    map<int, const vector<double>*> _min_departure;
    set<pair<int, int>> _best_active, _best_non_active;
    double _best_objective_value;

    void random_move(set<pair<int, int>>& from, set<pair<int, int>>& to);
    void relocate_move(set<pair<int, int>>& active, set<pair<int, int>>& non_active);
    double cost(set<pair<int, int>>& detours, map<pair<int, int>, StationSchedule::Entry>* saver = nullptr) const;
    Segment segment(const Route& route, unsigned long int i) const;
public:
    IteratedLocalSearch(SolutionCallback& cb, RelaxedSolution& x);
    ~IteratedLocalSearch();
    void run();
};


#endif //ECVRPTW_ITERATEDLOCALSEARCH_H
