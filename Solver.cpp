#include "Solver.h"

void Solver::run() {
    auto sa = SimulatedAnnealing(*this);

    auto start = chrono::steady_clock::now();
    while ( chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() < _max_execution_time )
        sa.iter();
}

void Solver::callback(Solution& x) {
    // compare with best found so far
}

void Solver::relaxed_callback(RelaxedSolution& x) {
    IteratedLocalSearch(*this, x);
}
