#ifndef ECVRPTW_RELAXEDSOLUTION_H
#define ECVRPTW_RELAXEDSOLUTION_H

#include <vector>

#include "Node.h"
#include "Route.h"

using namespace std;

class RelaxedSolution {
    vector<Route> _routes;

    void build_initial_solution();
    void apply_edd_rule();
public:
    RelaxedSolution();
    RelaxedSolution(const RelaxedSolution& origin);

    const vector<Route>& routes() const { return _routes; }
};


#endif //ECVRPTW_RELAXEDSOLUTION_H
