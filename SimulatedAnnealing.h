#ifndef ECVRPTW_SIMULATEDANNEALING_H
#define ECVRPTW_SIMULATEDANNEALING_H


#include "SolutionCallback.h"

class SimulatedAnnealing {
    SolutionCallback& _cb;
public:
    explicit SimulatedAnnealing(SolutionCallback& cb);
};


#endif //ECVRPTW_SIMULATEDANNEALING_H
