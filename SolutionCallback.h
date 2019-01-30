#ifndef ECVRPTW_SOLUTIONCALLBACK_H
#define ECVRPTW_SOLUTIONCALLBACK_H


#include "RelaxedSolution.h"
#include "Solution.h"

class SolutionCallback {
public:
    virtual bool relaxed_callback(RelaxedSolution&) = 0;
    virtual void callback(Solution&) = 0;
};


#endif //ECVRPTW_SOLUTIONCALLBACK_H
