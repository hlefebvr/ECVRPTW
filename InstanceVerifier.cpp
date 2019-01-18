#include "InstanceVerifier.h"
#include "Instance.h"

void InstanceVerifier::run() {
    Instance& instance = Instance::get();

    const vector<const CustomerNode*>& customers = instance.customers();

    const CustomerNode* min_deadline = *min_element(customers.begin(), customers.end(), [](const CustomerNode* A, const CustomerNode* B){
        return A->deadline < B->deadline;
    });

    const double max_arrival = min_deadline->deadline - min_deadline->service_time; // because when instance is loaded deadline = D + ST
    const double d_ij = Node::d(instance.depot(), *min_deadline);
    const double t_ij = instance.distance_to_time(d_ij);
    const double b_ij = instance.distance_to_consumed_battery(d_ij);
    const double time_to_charge = instance.time_needed_to_charge(b_ij);

    const double time_needed_before_arriving_at_customer = t_ij + time_to_charge;

    cout << "Time to travel : " << t_ij << endl;
    cout << "Time to charge : " << time_to_charge << endl;
    cout << "Time needed    : " << time_needed_before_arriving_at_customer << endl;
    cout << "Max arrival    : " << max_arrival << endl;

    if (time_needed_before_arriving_at_customer > max_arrival) {
        cout << "INFEASIBLE" << endl;
        throw runtime_error("ABORT INFEASIBLE");
    }
}
