#include "IteratedLocalSearch.h"

IteratedLocalSearch::IteratedLocalSearch(SolutionCallback &cb, RelaxedSolution &x) : _cb(cb), _x(x) {
    Instance& instance = Instance::get();

    for (const Route& route : _x.routes()) {
        auto min_departure = new vector<double >;

        // computing minimum departure
        double t = 0;
        const Node* last_visited_node = &instance.depot();
        for (const CustomerNode* customer : route.customers()) {
            min_departure->push_back(t);

            t += instance.distance_to_time(Node::d(*last_visited_node, *customer));
            if (t < customer->release_date) t = customer->release_date;
            t += customer->service_time;

            if(last_visited_node->id != 0) _best_non_active.insert({ last_visited_node->id, customer->id });

            last_visited_node = customer;
        }
        min_departure->push_back(t);

        _best_non_active.insert({ last_visited_node->id, 0 });

        _min_departure.insert({ route.id(), min_departure });
    }

}

void IteratedLocalSearch::run() {
    Instance& instance = Instance::get();

    set<pair<int, int>> detours;
    double best_cost = cost(detours);

    for (auto& route : _x.routes()) {
        unsigned long int n = route.customers().size() + 1;
        unsigned long int arg_i = n - 1;

        for (int n_try = 0 ; n_try < 5 ; n_try++) {
            pair<int, int> arg_edge;
            bool improved = false;
            for (unsigned long int i = arg_i; i != 0; i -= 1) {
                Segment u = segment(route, i);

                set<pair<int, int>> detours_tilde = detours;
                detours_tilde.insert({u.i.id, u.j.id});

                double f = cost(detours_tilde);
                if (f <= best_cost) {
                    if (f < best_cost) improved = true;
                    arg_edge = { u.i.id, u.j.id };
                    arg_i = i;
                    best_cost = f;
                }
            }

            if (improved) {
                detours.insert(arg_edge);
            }
        }
    }

    double f = cost(detours);
    cout << f << endl;

    if (f == 6) {
        map<pair<int, int>, StationSchedule::Entry> saver;
        cost(detours, &saver);
        auto feasible_solution = Solution(_x, saver);
        feasible_solution.print();
        throw runtime_error("manual interuption");
    }
}

IteratedLocalSearch::~IteratedLocalSearch() {
    for (auto iter : _min_departure) delete iter.second;
}

double IteratedLocalSearch::cost(set<pair<int, int>> &detours, map<pair<int, int>, StationSchedule::Entry>* saver) const {
    Instance& instance = Instance::get();
    unsigned long int k = 0, route_charged = 0, score = 0;

    map<int, StationSchedule> schedules;
    for (const StationNode* station : instance.stations()) schedules.insert({ station->id, StationSchedule(*station) });

    vector<Route> routes = _x.routes();
    sort(routes.begin(), routes.end(), [](const Route& A, const Route& B){
        return A.free_time() < B.free_time();
    });

    for (const Route& route : routes) {
        const vector<const CustomerNode*>& customers = route.customers();
        unsigned long int nb_segments = customers.size() + 1;
        if (nb_segments == 1) { k++; continue; }

        double t = instance.max_tour_time();
        double b = 0;
        unsigned long int l = nb_segments - 1;
        for (unsigned long int i = nb_segments - 1 ; i < nb_segments ; i--) {
            Segment u = segment(route, i);

            if (detours.count({ u.i.id, u.j.id }) > 0 || i == 0) { // if we have to charge in that segment
                double min_detour = numeric_limits<double>::max();
                const StationNode* arg_station = nullptr;
                Interval arg_I;
                for (const StationNode* station : instance.stations()) {
                    if (i == 0 && station->id != 0) continue;
                    // if (i != 0 && station->id == 0) continue;

                    const double d_is = Node::d(u.i, *station);
                    const double b_is = instance.distance_to_consumed_battery(d_is);
                    const double t_is = instance.distance_to_time(d_is);
                    const double d_sj = Node::d(u.j, *station);
                    const double b_sj = instance.distance_to_consumed_battery(d_sj);
                    const double t_sj = instance.distance_to_time(d_sj);
                    const double detour = d_sj + d_is;

                    if ( (t - t_sj) - (u.min_departure_i + t_is) <= 0 ) continue;
                    Interval max_presence_in_station = Interval(u.min_departure_i + t_is, t - t_sj);
                    const double b_tilde = abs(b - b_sj);
                    const double time_to_charge = instance.time_needed_to_charge(b_tilde);

                    const StationSchedule& schedule = schedules.at(station->id);
                    bool can_recharge = false;
                    Interval I_sr = Interval(0, 0);
                    for (pair<Interval, int> availability : schedule.availabilities_within(max_presence_in_station)) {
                        if (availability.first.span() >= time_to_charge && I_sr.to() < availability.first.to()) {
                            can_recharge = true;
                            I_sr = Interval( availability.first.to() - time_to_charge , availability.first.to());
                        }
                    }

                    if (can_recharge && detour < min_detour) {
                        min_detour = detour;
                        arg_station = station;
                        arg_I = I_sr;
                    }
                }

                if (arg_station != nullptr && arg_I.span() > 0) {
                    k += 1;
                    const double d_is = Node::d(u.i, *arg_station);
                    const double b_is = instance.distance_to_consumed_battery(d_is);
                    const double t_is = instance.distance_to_time(d_is);
                    StationSchedule& schedule = schedules.at(arg_station->id);
                    StationSchedule::Entry entry = schedule.add_entry(arg_I);
                    if (saver != nullptr) saver->insert({ { u.i.id, u.j.id }, entry });
                    b = -b_is;
                    t = min(arg_I.from() - t_is, u.deadline_i) - u.service_time_i;
                    if (arg_station->id == 0) route_charged++;
                    l = i;
                } else {
                    break;
                }

            } else { // otherwise, travel as usual
                const double d_ij = Node::d(u.i, u.j);
                const double b_ij = instance.distance_to_consumed_battery(d_ij);
                const double t_ij = instance.distance_to_time(d_ij);
                t = min(t - t_ij, u.deadline_i) - u.service_time_i;
                b -= b_ij;
            }
        }
        score += l;

    }

    return score;
    return route_charged;
    return (double) k / (detours.size() + instance.vehicle_count() );
}

IteratedLocalSearch::Segment IteratedLocalSearch::segment(const Route& route, unsigned long int i) const {
    Instance& instance = Instance::get();
    const vector<const CustomerNode*>& customers = route.customers();
    unsigned long int nb_segment = customers.size() + 1;

    if (i >= nb_segment) throw runtime_error("Invalid segment index");

    const Node* depot = &instance.depot();

    double min_departure = _min_departure.at(route.id())->at(i);
    double deadline_i, service_time_i;
    const Node *x_i, *x_j;

    if (i == 0) {
        x_i = depot;
        x_j = customers[0];
        deadline_i = instance.max_tour_time();
        service_time_i = 0;
    } else if (i == nb_segment - 1) {
        x_i = customers[i-1];
        x_j = depot;
        deadline_i = customers[i-1]->deadline;
        service_time_i = customers[i-1]->service_time;
    } else {
        x_i = customers[i-1];
        x_j = customers[i];
        deadline_i = customers[i-1]->deadline;
        service_time_i = customers[i-1]->service_time;
    }

    return { *x_i, *x_j, min_departure, deadline_i, service_time_i };

}

void IteratedLocalSearch::random_move(set<pair<int, int>> &from, set<pair<int, int>> &to) {
    unsigned long int n = from.size();
    if (n == 0) return;

    default_random_engine generator((unsigned long) chrono::system_clock::now().time_since_epoch().count());
    uniform_int_distribution<unsigned long int> uni_dist(0, n - 1);

    auto to_move= from.begin();
    advance(to_move, uni_dist(generator));

    pair<int, int> edge = *to_move;
    from.erase(to_move);
    to.insert(edge);
}

void IteratedLocalSearch::relocate_move(set<pair<int, int>> &from, set<pair<int, int>> &to) {
    random_move(from, to);
    random_move(to, from);
}

