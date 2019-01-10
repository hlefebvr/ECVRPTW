#ifndef ECVRPTW_SOLVER_H
#define ECVRPTW_SOLVER_H

#include "Solution.h"
#include "SolutionCallback.h"
#include "SimulatedAnnealing.h"
#include "IteratedLocalSearch.h"

class Solver : public SolutionCallback {
    Solution best_solution;
    double best_objective_value;
public:
    void run();
    void callback(const Solution& x) override;
    void relaxed_callback(const RelaxedSolution& x) override;
};


#endif //ECVRPTW_SOLVER_H
