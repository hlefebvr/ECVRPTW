#ifndef ECVRPTW_SOLVER_H
#define ECVRPTW_SOLVER_H

#include "Solution.h"
#include "SolutionCallback.h"
#include "SimulatedAnnealing.h"
#include "IteratedLocalSearch.h"

class Solver : public SolutionCallback {
    const double _max_execution_time;
    Solution* _best_solution = nullptr;
    double _best_objective_value = numeric_limits<double>::max();
    unsigned long int _nb_relaxed_solutions = 0, _nb_solutions = 0;
public:
    explicit Solver(double max_execution_time) : _max_execution_time(max_execution_time) {}
    ~Solver();
    const Solution& run(double* distance = nullptr, double* execution_time = nullptr);
    void callback(Solution& x) override;
    void relaxed_callback(RelaxedSolution& x) override;
};


#endif //ECVRPTW_SOLVER_H
