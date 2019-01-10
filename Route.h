#ifndef ECVRPTW_ROUTE_H
#define ECVRPTW_ROUTE_H

#include <vector>
#include <limits>

#include "Instance.h"
#include "Node.h"

using namespace std;

class Route {
    friend class RelaxedSolution;

    vector<const CustomerNode*> _customers;
    double _distance, _capacity, _nb_tardy_deliveries, _travel_time, _service_time;

    void push_back(const CustomerNode& customer);
    void apply_edd_rule();
public:
    bool feasible() const;
    double distance() const;
    double free_time() const;
    const vector<const CustomerNode*>& customers() const;
};


#endif //ECVRPTW_ROUTE_H
