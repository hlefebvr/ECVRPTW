#include "Solver.h"

void Solver::run() {
    SimulatedAnnealing(*this);
}

void Solver::callback(const Solution& x) {
    // compare with best found so far
}

void Solver::relaxed_callback(const RelaxedSolution& x) {
    IteratedLocalSearch(*this, x);
}
