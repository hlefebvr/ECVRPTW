#include "Solver.h"

const Solution& Solver::run(double* distance, double* execution_time) {
    auto sa = SimulatedAnnealing(*this);

    auto start = chrono::steady_clock::now();
    while ( chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() < _max_execution_time )
        sa.iter();

    if (execution_time != nullptr) *execution_time = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count();
    if (distance != nullptr) *distance = _best_objective_value;

    /* cout << "Relaxed  = " << _nb_relaxed_solutions << endl;
    cout << "Feasible = " << _nb_solutions << endl; */

    if (_best_solution == nullptr) throw runtime_error("No feasible solution could be find.");

    return *_best_solution;
}

void Solver::callback(Solution& x) {
    _nb_solutions += 1;
    auto evaluator = Solution::Explorer(x);
    evaluator.explore();
    const double objective_value = evaluator.D();
    if (objective_value < _best_objective_value) {
        delete _best_solution;
        _best_objective_value = objective_value;
        _best_solution = new Solution(x);
    }
}

bool Solver::relaxed_callback(RelaxedSolution& x) {
    _nb_relaxed_solutions += 1;
    auto ils = IteratedLocalSearch(*this, x);
    return ils.run();
}

Solver::~Solver() {
    delete _best_solution;
}
