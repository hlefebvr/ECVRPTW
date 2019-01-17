#include "Route.h"

bool Route::feasible() const {
    if (!_evaluated) throw runtime_error("This route has not been evaluated. Cannot call feasible()");

    Instance& instance = Instance::get();

    const bool overcapacitated = _capacity > instance.vehicle_capacity();
    const bool overtime = _travel_time >  instance.max_tour_time();

    return !overcapacitated && (_nb_tardy_deliveries == 0) && !overtime;
}

double Route::free_time() const {
    if (!_evaluated) throw runtime_error("This route has not been evaluated. Cannot call free_time()");

    Instance& instance = Instance::get();

    return instance.max_tour_time() - ( _travel_time + _service_time );
}

double Route::distance() const {
    if (!_evaluated) throw runtime_error("This route has not been evaluated. Cannot call distance()");

    return _distance;
}

void Route::push_back(const CustomerNode &customer) {
    _evaluated = false;
    _customers.push_back(&customer);
}

void Route::apply_edd_rule() {
    _evaluated = false;
    sort(_customers.begin(), _customers.end(), [](const CustomerNode* A, const CustomerNode* B){ return A->deadline < B->deadline; });
}

const vector<const CustomerNode *> &Route::customers() const {
    return _customers;
}

double Route::evaluate() {
    Instance& instance = Instance::get();

    _distance = 0;
    _travel_time = 0;
    _capacity = 0;
    _service_time = 0;
    _nb_tardy_deliveries = 0;

    double hypo = instance.time_needed_to_charge(instance.distance_to_consumed_battery(Node::d(instance.depot(), *_customers[0])));
    double t = 0; // TODO : or better t = time needed to charge to go to the first node ?

    const Node* last_visited_node = &instance.depot();
    for (const CustomerNode* customer : _customers) {

        const double d_ij = Node::d(*last_visited_node, *customer);
        const double t_ij = instance.distance_to_time(d_ij);

        _distance += d_ij;
        _travel_time += t_ij;
        _capacity += customer->demand;
        _service_time += customer->service_time;

        t += t_ij;
        if (t < customer->release_date) t = customer->release_date;
        t += customer->service_time;

        if (t > customer->deadline) _nb_tardy_deliveries += 1;

        last_visited_node = customer;
    }
    // way back to depot :

    const double d_ij = Node::d(*last_visited_node, instance.depot());
    const double t_ij = instance.distance_to_time(d_ij);

    _distance += d_ij;
    _travel_time += t_ij;

    _evaluated = true;
    return cost();
}

double Route::cost() const {
    if (!_evaluated) throw runtime_error("This route has not been evaluated. Cannot call cost()");

    Instance& instance = Instance::get();

    const bool overcapacitated = _capacity > instance.vehicle_capacity();
    const bool overtime = _travel_time >  instance.max_tour_time();

    return _distance + 1000 * (overtime + overcapacitated + _nb_tardy_deliveries);
}

string Route::to_string() const {

    stringstream ss;
    ss << "(0, ";
    for (const CustomerNode* customer : _customers) ss << customer->id << ", ";
    ss << "0)";

    return ss.str();
}
