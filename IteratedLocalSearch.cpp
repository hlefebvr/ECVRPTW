#include "IteratedLocalSearch.h"

IteratedLocalSearch::IteratedLocalSearch(SolutionCallback &cb, RelaxedSolution &x) : _cb(cb), _x(x) {
    Instance& instance = Instance::get();
    const double Tmax = instance.max_tour_time();

    for (const Route& route : _x.routes()) {
        const vector<const CustomerNode*>& customers = route.customers();

        // compute max_arrival and b_j
        vector<double>& max_arrival = _max_arrival.insert({ route.id(), vector<double>() }).first->second;

        unsigned long int i = customers.size();
        max_arrival.resize(i + 1);

        double t = Tmax;
        const Node* last_visited_node = &instance.depot();
        for (auto it = customers.rbegin(), end = customers.rend() ; it != end ; it++ ) {
            const CustomerNode* customer = *it;
            max_arrival.at(i) = t;
            double d_ij = Node::d(*last_visited_node, *customer);
            t = min(t - instance.distance_to_time(d_ij), customer->deadline) - customer->service_time;
            last_visited_node = customer;
            i -= 1;
        }
        max_arrival.at(i) = t;
    }

}

void IteratedLocalSearch::run() {
    Instance& instance = Instance::get();
    unsigned long int nb_customers = instance.customers().size();
    default_random_engine generator((unsigned long) chrono::system_clock::now().time_since_epoch().count());

    DetourPlan best_detour_plan;
    for (const Route& route : _x.routes()) {
        auto result = best_detour_plan.insert({ route.id(), vector<bool>() });
        result.first->second.assign(route.customers().size() + 1, true);
    }
    const unsigned long int max_nb_detours = nb_customers + instance.vehicle_count();
    unsigned long int best_nb_detours = max_nb_detours;
    double best_objective_value = cost(best_detour_plan);
    bool useless_perturbation = true;
    pair<int, unsigned long int> perturbation = { -1, -1 };

    unsigned long int nb_iteration_without_improvement = 0;
    unsigned long int nb_perturbation = 0;
    int i = 0, n_iter = 0;
    while (nb_iteration_without_improvement < 10 /* nb_perturbation < nb_customers */) {
        // Local Search
        pair<int, unsigned long int> ignored_detour;
        uniform_int_distribution<unsigned long int> among_active_detours(0, best_nb_detours);

        map<pair<int, int>, StationSchedule::Entry> detours;
        const double f = cost(best_detour_plan, among_active_detours(generator), &ignored_detour, &detours);

        if (f < best_objective_value) {
            best_objective_value = f;
            best_detour_plan.at(ignored_detour.first).at(ignored_detour.second) = false;
            nb_iteration_without_improvement = 0;
            best_nb_detours -= 1;
            useless_perturbation = false;

            if (best_objective_value < big_M) {
                cout << best_objective_value << " / " << f << endl;
                auto feasible_solution = Solution(_x, detours);
                _cb.callback(feasible_solution);
                feasible_solution.print();
                // throw runtime_error("FEASIBLE TRIGGER");
            }
        } else {
            nb_iteration_without_improvement += 1;
        }

        // Perturb
        /* if (nb_iteration_without_improvement == nb_customers) {
            nb_perturbation += 1;
            nb_iteration_without_improvement = 0;
            uniform_int_distribution<unsigned long int> among_inactive_detours(0, nb_customers - best_nb_detours);
            const unsigned long int detour_to_insert = among_inactive_detours(generator);
            unsigned long int inactive_detour_count = 0;
            for (const Route& route : _x.routes()) {
                int id = route.id();
                vector<bool>& route_detours = best_detour_plan.at(id);
                for (unsigned long int i = 0, n = route_detours.size() ; i < n ; i += 1) {
                    inactive_detour_count += !route_detours[i];
                    if (inactive_detour_count == detour_to_insert) {
                        route_detours[i] = true;
                        perturbation = { id, i };
                        break;
                    }
                }
            }
            best_nb_detours = min(best_nb_detours + 1, max_nb_detours);
            useless_perturbation = true;
        } */
        n_iter++;
    }

    if (useless_perturbation && perturbation != pair<int, unsigned long int> { -1, -1 })
        best_detour_plan.at(perturbation.first).at(perturbation.second) = false;

    /* if (true || best_objective_value < big_M) {
        map<pair<int, int>, StationSchedule::Entry> detours;
        const double f_opt = cost(best_detour_plan, -1, nullptr, &detours);
        cout << best_objective_value << " / " << f_opt << endl;
        auto feasible_solution = Solution(_x, detours);
        _cb.callback(feasible_solution);
        feasible_solution.print();
        throw runtime_error("manual interupt");
    } */
}


IteratedLocalSearch::Segment IteratedLocalSearch::segment(const Route& route, unsigned long int i) const {
    Instance& instance = Instance::get();
    const vector<const CustomerNode*>& customers = route.customers();
    unsigned long int nb_segment = customers.size() + 1;

    if (i >= nb_segment) throw runtime_error("Invalid segment index");

    const Node* depot = &instance.depot();

    double max_arrival_j = _max_arrival.at(route.id()).at(i);
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

double IteratedLocalSearch::cost(const IteratedLocalSearch::DetourPlan &detours, unsigned long ignore_detour,
                                 pair<int, unsigned long> *ignored_detour,
                                 map<pair<int, int>, StationSchedule::Entry>* saver) const {
    Instance& instance = Instance::get();
    const double b_cap = instance.battery_capacity();
    map<int, StationSchedule> schedules;
    for (const StationNode* station : instance.stations()) schedules.insert({station->id, StationSchedule(*station)});
    unsigned long int nb_underbattery_node = 0, detour_count = 0;
    double detoured_distance = 0;

    const vector<Route>& routes = _x.routes();
    // sort(routes.begin(), routes.end(), [](const Route& A, const Route& B){ return A.free_time() < B.free_time(); });

    for (const Route& route : routes) {
        const int route_id = route.id();
        const unsigned long int nb_segment = route.customers().size() + 1;
        double t = 0;
        double b = b_cap;
        for (unsigned long int e = 0 ; e < nb_segment ; e += 1) {
            const Segment u = segment(route, e);
            nb_underbattery_node += b < 0;
            const bool is_a_detour = detours.at(route_id).at(e);
            detour_count += is_a_detour;

            if (is_a_detour && ignore_detour == detour_count) {
                if (ignored_detour != nullptr) *ignored_detour = { route_id, e };
            } else if (is_a_detour) {
                const StationNode& closest_station = instance.closest_station_between(u.i, u.j);
                const double d_is = Node::d(u.i, closest_station);
                const double t_is = instance.distance_to_time(d_is);
                const double b_is = instance.distance_to_consumed_battery(d_is);
                const double d_sj = Node::d(u.j, closest_station);
                const double t_sj = instance.distance_to_time(d_sj);
                const double b_sj = instance.distance_to_consumed_battery(d_sj);
                detoured_distance += d_is + d_sj;

                // nb_underbattery_node += (b - b_is < 0);

                if (b - b_is >= 0 && t + t_is < u.max_arrival_j - t_sj) {

                    // compute battery needed from j to next_detour_j
                    double b_needed = b_sj;
                    for (unsigned long int l = e + 1; l < nb_segment; l += 1) {
                        const Segment arc = segment(route, l);
                        if (detours.at(route_id).at(l) && ignore_detour != l) {
                            b_needed += instance.distance_to_consumed_battery(Node::d(arc.i, instance.closest_station_between(arc.i, arc.j)));
                            break;
                        }
                        b_needed += instance.distance_to_consumed_battery(Node::d(arc.i, arc.j));
                    }
                    b_needed = b_needed - (b - b_is) + .000000001;
                    if (b_needed > 0) {
                        const double t_needed = instance.time_needed_to_charge(b_needed);
                        auto max_presence = Interval(t + t_is, u.max_arrival_j - t_sj);
                        StationSchedule &schedule = schedules.at(closest_station.id);
                        auto availabilities = schedule.availabilities_within(max_presence);

                        if (!availabilities.empty()) {
                            Interval first_availability = availabilities.front().first;
                            if (first_availability.span() >= t_needed) {
                                b = min(b - b_is + b_needed, b_cap) - b_sj;
                                t = first_availability.from() + t_needed + t_sj;
                                StationSchedule::Entry entry = schedule.add_entry(Interval(first_availability.from(), first_availability.from() + t_needed));
                                if (saver != nullptr) saver->insert({{u.i.id, u.j.id}, entry});
                                continue;
                            }
                        }

                    }
                }

            }

            const double d_ij = Node::d(u.i, u.j);
            const double t_ij = instance.distance_to_time(d_ij);
            const double b_ij = instance.distance_to_consumed_battery(d_ij);
            t = max(t + t_ij, u.release_date_j) + u.service_time_j;
            b -= b_ij;
        }
        nb_underbattery_node += b < 0;
    }

    return detoured_distance + big_M * nb_underbattery_node;
}
