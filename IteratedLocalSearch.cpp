#include "IteratedLocalSearch.h"

IteratedLocalSearch::IteratedLocalSearch(SolutionCallback &cb, RelaxedSolution &x) : _cb(cb), _x(x) {
    Instance& instance = Instance::get();
    const double Tmax = instance.max_tour_time();
    const Node& depot = instance.depot();

    for (const Route& route : _x.routes()) {
        const vector<const CustomerNode*>& customers = route.customers();

        // computing max arrival for this route
        unsigned long int n_customers = customers.size();
        vector<double>& max_arrival = *new vector<double>();
        max_arrival.resize(n_customers + 1);

        double t = Tmax;
        unsigned long int i = n_customers;
        const Node* last_visited_node = &depot;
        for (auto it = customers.rbegin(), end = customers.rend(); it != end ; it++) {
            max_arrival[i] = t;

            const CustomerNode* customer = *it;
            const double t_ij = instance.distance_to_time(Node::d(*last_visited_node, *customer));

            t -= t_ij;
            if (t > customer->deadline) t = customer->deadline;
            t -= customer->service_time;

            last_visited_node = customer;
            i -= 1;
        }
        max_arrival[i] = t;

        _max_arrival.insert({ &route, &max_arrival });

        // computing candidate segments for this route
        t = 0;
        i = 0;
        last_visited_node = &depot;
        double detour;
        for (const CustomerNode* customer : customers) {
            const double available_time = max_arrival[i] - t;
            const StationNode& closest_station = instance.closest_station_between(*last_visited_node, *customer, &detour);
            if (instance.distance_to_time(detour) < available_time) _active.insert({ last_visited_node->id, customer->id});

            const double t_ij = instance.distance_to_time(Node::d(*last_visited_node, *customer));
            t += t_ij;
            if (t < customer->release_date) t = customer->release_date;
            t += customer->service_time;

            last_visited_node = customer;
            i += 1;
        }
        const double available_time = max_arrival[i] - t;
        const StationNode& closest_station = instance.closest_station_between(*last_visited_node, depot, &detour);
        if (instance.distance_to_time(detour) < available_time) _active.insert({ last_visited_node->id, depot.id});

    }

}

void IteratedLocalSearch::run() {
    for (int i = 0 ; i < 10 ; i++) {
        for (int j = 0; j < 50; j++) {
            remove_move();
            if (is_feasible()) cout << "feasible found !!!!" << endl;
        }
        add_move();
    }
}

IteratedLocalSearch::~IteratedLocalSearch() {
    for (auto iter : _max_arrival) delete iter.second;
}

void IteratedLocalSearch::remove_move() {
    return random_move(_active, _non_active);
}

void IteratedLocalSearch::add_move() {
    return random_move(_non_active, _active);
}

void IteratedLocalSearch::random_move(set<pair<int, int>>& A, set<pair<int, int>>& B) {
    unsigned long int n = A.size();
    if (n == 0) return;

    default_random_engine generator((unsigned long) chrono::system_clock::now().time_since_epoch().count());
    uniform_int_distribution<unsigned long int> uni_dist(0, n - 1);

    auto to_move= A.begin();
    advance(to_move, uni_dist(generator));

    pair<int, int> edge = *to_move;
    A.erase(to_move);
    B.insert(edge);
}
