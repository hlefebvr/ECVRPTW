#include "IteratedLocalSearch.h"

IteratedLocalSearch::IteratedLocalSearch(SolutionCallback &cb, RelaxedSolution &x) : _cb(cb), _x(x) {
    Instance& instance = Instance::get();
    const double Tmax = instance.max_tour_time();

    for (const Route& route : _x.routes()) {
        auto max_arrival = new vector<double >;
        const vector<const CustomerNode*>& customers = route.customers();
        unsigned long int i = customers.size();
        max_arrival->resize(i + 1);

        double t = Tmax;
        const Node* last_visited_node = &instance.depot();
        for (auto it = customers.rbegin(), end = customers.rend() ; it != end ; it++ ) {
            const CustomerNode* customer = *it;
            max_arrival->at(i) = t;
            t = min(t - instance.distance_to_time(Node::d(*last_visited_node, *customer)), customer->deadline) - customer->service_time;
            last_visited_node = customer;
            i -= 1;
        }
        max_arrival->at(i) = t;

        _max_arrival.insert({ route.id(), max_arrival });
    }

}

void IteratedLocalSearch::run() {
    Instance& instance = Instance::get();
    const double b_cap = instance.battery_capacity();
    unsigned int recharged_route = 0;

    map<int, StationSchedule> schedules;
    map<pair<int, int>, StationSchedule::Entry> detours;

    for (const StationNode* station : instance.stations())
        schedules.insert({station->id, StationSchedule(*station)});

    vector<Route> routes = _x.routes();
    sort(routes.begin(), routes.end(), [](const Route& A, const Route& B){ return A.free_time() < B.free_time(); });

    for (auto& route : routes) {
        double t = 0, b = b_cap, max_b = 0, arg_b, arg_t;
        Interval arg_I;
        unsigned long int arg_e;
        const StationNode* arg_station = nullptr;
        unsigned long int n = route.customers().size() + 1;
        if (n == 1) continue;
        for (unsigned long int e = 0 ; e <= n ; e += 1) {
            if (b < 0) {
                if (arg_station == nullptr) break;
                Segment u = segment(route, arg_e);
                StationSchedule& schedule = schedules.at(arg_station->id);
                StationSchedule::Entry entry = schedules.at(arg_station->id).add_entry(arg_I);
                detours.insert({ { u.i.id, u.j.id }, entry });
                t = arg_t;
                b = arg_b;
                e = arg_e;
                arg_station = nullptr;
                max_b = 0;
            } else if (e != n) {
                Segment u = segment(route, e);
                for (const StationNode* s : instance.stations()) {
                    if (s->id == 0) continue;
                    const double d_is = Node::d(u.i, *s);
                    const double t_is = instance.distance_to_time(d_is);
                    const double b_is = instance.distance_to_consumed_battery(d_is);
                    const double d_sj = Node::d(u.j, *s);
                    const double t_sj = instance.distance_to_time(d_sj);
                    const double b_sj = instance.distance_to_consumed_battery(d_sj);
                    const double free_time = (u.max_arrival_j - t_sj) - (t + t_is);

                    if ( free_time > 0 ) {
                        StationSchedule& schedule = schedules.at(s->id);
                        auto availabilities = schedule.availabilities_within(Interval(t + t_is, u.max_arrival_j - t_sj));
                        if (!availabilities.empty()) {
                            Interval first_availability = availabilities.front().first;
                            double max_gain = min(b_cap - b - b_is, instance.time_to_battery_gain(first_availability.span()));

                            // compute b_to_depot
                            double b_depot = b_sj;
                            for (unsigned long int l = e + 1 ; l < n ; l++) {
                                Segment arc = segment(route, l);
                                b_depot += instance.distance_to_consumed_battery(Node::d(arc.i, arc.j));
                            }
                            double just_in_time = b_depot - (b - b_is);
                            if (0 < just_in_time && just_in_time < max_gain) max_gain = just_in_time;

                            const double b_j = b - b_is + max_gain - b_sj;
                            const double t_b = instance.time_needed_to_charge(max_gain);

                            if (first_availability.span() >= t_b && max_gain >= max_b && (b - b_is) >= 0 && max_gain > 0) {
                                max_b = max_gain;
                                arg_station = s;
                                arg_I = Interval(first_availability.from(), first_availability.from() + t_b);
                                arg_b = b_j;
                                arg_t = max(first_availability.to() + t_sj, u.release_date_j) + u.service_time_j;
                                arg_e = e;
                            }
                        }
                    }

                }

                const double d_ij = Node::d(u.i, u.j);
                const double t_ij = instance.distance_to_time(d_ij);
                const double b_ij = instance.distance_to_consumed_battery(d_ij);
                t = max(t + t_ij, u.release_date_j) + u.service_time_j;
                b = b - b_ij;
            }
        }

        if (b >= 0) recharged_route += 1;
    }

    if (recharged_route == instance.vehicle_count()) {
        auto feasible_solution = Solution(_x, detours);
        _cb.callback(feasible_solution);
    }
}

IteratedLocalSearch::~IteratedLocalSearch() {
    for (auto iter : _max_arrival) delete iter.second;
}


IteratedLocalSearch::Segment IteratedLocalSearch::segment(const Route& route, unsigned long int i) const {
    Instance& instance = Instance::get();
    const vector<const CustomerNode*>& customers = route.customers();
    unsigned long int nb_segment = customers.size() + 1;

    if (i >= nb_segment) throw runtime_error("Invalid segment index");

    const Node* depot = &instance.depot();

    double max_arrival_j = _max_arrival.at(route.id())->at(i);
    double release_date_j, service_time_j;
    const Node *x_i, *x_j;

    if (i == 0) {
        x_i = depot;
        x_j = customers[0];
        release_date_j = customers[0]->release_date;
        service_time_j = customers[0]->service_time;
    } else if (i == nb_segment - 1) {
        x_i = customers[i-1];
        x_j = depot;
        release_date_j = 0;
        service_time_j = 0;
    } else {
        x_i = customers[i-1];
        x_j = customers[i];
        release_date_j = customers[i]->release_date;
        service_time_j = customers[i]->service_time;
    }

    return { *x_i, *x_j, max_arrival_j, release_date_j, service_time_j };

}
