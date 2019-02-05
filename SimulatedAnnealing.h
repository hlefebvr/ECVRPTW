#ifndef ECVRPTW_SIMULATEDANNEALING_H
#define ECVRPTW_SIMULATEDANNEALING_H

#include <random>
#include <chrono>

#include "SolutionCallback.h"

class SimulatedAnnealing {
    SolutionCallback& _cb;
    unsigned long int _nb_iter;
    RelaxedSolution _current_solution;
    double _current_objetive_value;
    double _temperature = 1000;

    const double _alpha = .7;

    void move(RelaxedSolution& x);
public:
    explicit SimulatedAnnealing(SolutionCallback& cb);
    void iter();
};


#endif //ECVRPTW_SIMULATEDANNEALING_H
