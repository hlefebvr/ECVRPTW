#ifndef ECVRPTW_ROUTE_H
#define ECVRPTW_ROUTE_H

#include <vector>
#include <string>
#include <limits>

#include "Instance.h"
#include "Node.h"

using namespace std;

static int route_id = 0;

class Route {
    friend class RelaxedSolution;

    vector<const CustomerNode*> _customers;
    double _distance, _capacity, _travel_time, _service_time;
    unsigned int _nb_tardy_deliveries;
    bool _evaluated = false;
    int _id = route_id++ % Instance::get().vehicle_count();

    void push_back(const CustomerNode& customer);
    void apply_edd_rule();
public:
    bool feasible() const;
    double distance() const;
    double free_time() const;
    double cost() const;
    double evaluate();
    const vector<const CustomerNode*>& customers() const;
    int id() const { return _id; }
    string to_string() const;
};

#endif //ECVRPTW_ROUTE_H
