#ifndef ECVRPTW_SOLVER_H
#define ECVRPTW_SOLVER_H

#include "Solution.h"
#include "SolutionCallback.h"
#include "SimulatedAnnealing.h"
#include "IteratedLocalSearch.h"

class Solver : public SolutionCallback {
    Solution best_solution;
    double best_objective_value;
    const double _max_execution_time;
public:
    Solver(double max_execution_time) : _max_execution_time(max_execution_time) {}
    void run();
    void callback(Solution& x) override;
    void relaxed_callback(RelaxedSolution& x) override;
};


#endif //ECVRPTW_SOLVER_H
