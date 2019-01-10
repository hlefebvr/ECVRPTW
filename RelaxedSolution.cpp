#include "RelaxedSolution.h"

RelaxedSolution::RelaxedSolution() {
    Instance& instance = Instance::get();
    auto nb_routes = (unsigned long int) instance.vehicle_count();

    _routes.resize(nb_routes);
    build_initial_solution();
}

void RelaxedSolution::build_initial_solution() {
    Instance& instance = Instance::get();
    const vector<const CustomerNode*>& customers = instance.customers();
    const int n_customers = instance.customer_count(), n_routes = instance.vehicle_count();

    vector<const CustomerNode*> customers_to_insert;
    copy(customers.begin(), customers.end(), back_inserter(customers_to_insert));

    vector<const CustomerNode*> nearest_neighbhour_tour;

    // nearest neighbour insertion
    const Node* last_inserted_node = &instance.depot();
    int n_inserted = 0;
    while (n_inserted++ < n_customers) {
        auto nearest = min_element(customers_to_insert.begin(), customers_to_insert.end(),
                                   [last_inserted_node](const CustomerNode* A, const CustomerNode* B) -> bool {
                                       return Node::d(*A, *last_inserted_node) < Node::d(*B, *last_inserted_node);
                                   });
        nearest_neighbhour_tour.push_back(*nearest);
        last_inserted_node = *nearest;
        customers_to_insert.erase(nearest);
    }

    // dispatch in routes
    const double mean_demand = instance.overall_demand() / instance.vehicle_count();
    int k = 0;
    double q = 0;
    for (const CustomerNode* node : nearest_neighbhour_tour) {
        if (q + node->demand > mean_demand && k < n_routes - 1) { k++; q = 0; }
        q += node->demand;
        _routes[k].push_back(*node);
    }

    // apply EDD rule on every route
    for (Route& route : _routes) route.apply_edd_rule();
}
