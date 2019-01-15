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

    map<const Route* const, const vector<double>*> _max_arrival;
    set<pair<int, int>> _active, _non_active;

    void random_move(set<pair<int, int>>& A, set<pair<int, int>>& B); // randomly move from A to B
    void remove_move();
    void add_move();
    double cost();
public:
    IteratedLocalSearch(SolutionCallback& cb, RelaxedSolution& x);
    ~IteratedLocalSearch();
    void run();
};


#endif //ECVRPTW_ITERATEDLOCALSEARCH_H
