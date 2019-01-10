#ifndef ECVRPTW_ITERATEDLOCALSEARCH_H
#define ECVRPTW_ITERATEDLOCALSEARCH_H


#include "SolutionCallback.h"

class IteratedLocalSearch {
public:
    IteratedLocalSearch(SolutionCallback& cb, const RelaxedSolution& x);
};


#endif //ECVRPTW_ITERATEDLOCALSEARCH_H
