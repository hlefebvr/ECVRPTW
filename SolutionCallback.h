#ifndef ECVRPTW_SOLUTIONCALLBACK_H
#define ECVRPTW_SOLUTIONCALLBACK_H


#include "RelaxedSolution.h"
#include "Solution.h"

class SolutionCallback {
public:
    virtual void relaxed_callback(const RelaxedSolution&) = 0;
    virtual void callback(const Solution&) = 0;
};


#endif //ECVRPTW_SOLUTIONCALLBACK_H
