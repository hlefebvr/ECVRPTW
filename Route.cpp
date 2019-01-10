#include "Route.h"

bool Route::feasible() const {
    Instance& instance = Instance::get();
    return
        _capacity <= instance.vehicle_capacity()
        && _nb_tardy_deliveries == 0
        && _travel_time <= instance.max_tour_time()
        ;
}

double Route::free_time() const {
    Instance& instance = Instance::get();
    return instance.max_tour_time() - ( _travel_time + _service_time );
}

double Route::distance() const {
    return _distance;
}

void Route::push_back(const CustomerNode &customer) {
    _customers.push_back(&customer);
}

void Route::apply_edd_rule() {
    sort(_customers.begin(), _customers.end(), [](const CustomerNode* A, const CustomerNode* B){ return A->deadline < B->deadline; });
}

const vector<const CustomerNode *> &Route::customers() const {
    return _customers;
}
