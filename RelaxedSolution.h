#ifndef ECVRPTW_RELAXEDSOLUTION_H
#define ECVRPTW_RELAXEDSOLUTION_H

#include <vector>
#include <random>
#include <chrono>
#include <string>

#include "Node.h"
#include "Route.h"

using namespace std;

class RelaxedSolution {
    vector<Route> _routes;

    void build_initial_solution();
    void swap(unsigned long int k1, unsigned long int n1, unsigned long int k2, unsigned long int n2);
public:
    RelaxedSolution();
    // RelaxedSolution(const RelaxedSolution& origin);

    const vector<Route>& routes() const { return _routes; }
    double evaluate();
    bool feasible();

    void one_point_move();
    void two_opt_move();

    string to_string() const;
};


#endif //ECVRPTW_RELAXEDSOLUTION_H
