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

double RelaxedSolution::evaluate() {
    double value = 0;

    for (Route& route : _routes)
        value += route.evaluate();

    return value;
}

bool RelaxedSolution::feasible() {
    for (Route& route : _routes) {
        if (!route._evaluated) route.evaluate();
        if (!route.feasible()) return false;
    }

    return true;
}

void RelaxedSolution::one_point_move() {
    // generate move
    const Instance& instance = Instance::get();
    int n_routes = instance.vehicle_count();
    int n_customers = instance.customer_count();

    default_random_engine generator((unsigned long) chrono::system_clock::now().time_since_epoch().count());
    uniform_int_distribution<unsigned long int> among_routes(0, n_routes-1);
    uniform_int_distribution<unsigned long int> among_customers(0, n_customers);

    unsigned long int k1 = among_routes(generator);
    unsigned long int k2 = among_routes(generator);
    unsigned long int n1 = among_customers(generator);
    unsigned long int n2 = among_customers(generator);

    // apply move
    vector<const CustomerNode*>& route_1 = _routes[k1]._customers;
    vector<const CustomerNode*>& route_2 = _routes[k2]._customers;

    unsigned long int m1 = route_1.size();
    unsigned long int m2 = route_2.size();

    if (m1 < 1 || m2 < 1) return;

    unsigned long int node_to_move_index = n1 % m1;
    unsigned long int index_to_insert_node = n2 % m2;


    const CustomerNode* node_to_move = route_1[node_to_move_index];

    route_1.erase(route_1.begin() + node_to_move_index);
    route_2.insert(route_2.begin() + index_to_insert_node, node_to_move);

    _routes[k1]._evaluated = false;
    _routes[k2]._evaluated = false;
}

void RelaxedSolution::two_opt_move() {
    // generate move
    const Instance& instance = Instance::get();
    int n_routes = instance.vehicle_count();
    int n_customers = instance.customer_count();

    default_random_engine generator((unsigned long) chrono::system_clock::now().time_since_epoch().count());
    uniform_int_distribution<unsigned long int> among_routes(0, n_routes-1);
    uniform_int_distribution<unsigned long int> among_customers(0, n_customers);

    unsigned long int k1 = among_routes(generator);
    unsigned long int k2 = among_routes(generator);
    unsigned long int n1 = among_customers(generator);
    unsigned long int n2 = among_customers(generator);

    // apply move
    vector<const CustomerNode*>& route_1 = _routes[k1]._customers;
    vector<const CustomerNode*>& route_2 = _routes[k2]._customers;

    unsigned long int m1 = route_1.size() - 1;
    unsigned long int m2 = route_2.size() - 1;

    if (m1 < 2 || m2 < 2) return;

    const unsigned long int e1 = n1 % m1; // (e1, e1+1)
    const unsigned long int e2 = n2 % m2; // (e2, e2+1)

    swap(k1, e1, k2, e2);
    swap(k1, e1+1, k2, e2+1);
}

void RelaxedSolution::swap(unsigned long int k1, unsigned long int n1, unsigned long int k2, unsigned long int n2) {
    vector<const CustomerNode*>& route_1 = _routes[k1]._customers;
    vector<const CustomerNode*>& route_2 = _routes[k2]._customers;

    const unsigned long int m1 = route_1.size();
    const unsigned long int m2 = route_2.size();

    if (m1 < 1 || m2 < 1) return;

    const unsigned long int i1 = n1 % m1;
    const unsigned long int i2 = n2 % m2;

    const CustomerNode* tmp = route_1[i1];
    route_1[i1] = route_2[i2];
    route_2[i2] = tmp;

    _routes[k1]._evaluated = false;
    _routes[k2]._evaluated = false;

}

string RelaxedSolution::to_string() const {
    stringstream ss;
    unsigned int i = 0;
    for (const Route& route : _routes) {
        ss << "Route " << i << route.to_string() << endl;
        i += 1;
    }

    return ss.str();
}
