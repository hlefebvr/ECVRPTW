#include "Solution.h"

void Solution::print() const {
    Instance& instance = Instance::get();
    cout.precision(2);

    class SolutionWriter : public Explorer {
    protected:
        void on_arrival_to_station(const Node& previous, const StationNode& node, const Interval& interval) override {
            cout << fixed << "STATION \t" << node.id << "\t" << _t << "\t" << _d << "\t" << _b << "\t" << _q << "\t" << "from " << interval.from() << " to " << interval.to() << endl;
        }
        void on_departure_from_station(const Node& previous, const StationNode& node) override {
            cout << fixed << "STATION \t" << node.id << "\t" << _t << "\t" << _d << "\t" << _b << "\t" << _q << "\t" << endl;
        }
        void on_departure_from_customer(const Node& previous, const CustomerNode& node) override {
            cout << fixed << "CUSTOMER\t" << node.id << "\t" << _t << "\t" << _d << "\t" << _b << "\t" << _q << "\t"
                << "[" << node.release_date << ", " << node.deadline << "], st = " << node.service_time << "\t";
            if (_t > node.deadline) cout << "LATE";
            cout << endl;
        }
        void after_route(const Route& route) override {
            cout << fixed << "STATION \t" << 0 << "\t" << _t << "\t" << _d << "\t" << _b << "\t" << _q << "\t" << endl;
        }
        void before_route(const Route& route) override {
            cout << "Route " << route.id() << endl;
            cout << "type\t\tid\ttime\tdist.\tbat.\tcap.\t" << endl;
        }
    public:
        explicit SolutionWriter(const Solution& x) : Explorer(x) {}
    };

    cout << "Solution with " << _charging_decisions.size() << " detours" << endl;
    auto writer = SolutionWriter(*this);
    writer.explore();
}

void Solution::to_file(double exec_time, double obj) const {
    Instance& instance = Instance::get();
    const string result_filename = instance.get_result_filename();

    ofstream f;
    f.open(result_filename);

    f << "Obj;" << obj << endl;
    f << "CPU;" << exec_time << endl;
    f << "Vehicle;Orig;Dest;Dist;Rel;Dead;ServTime;DemDest;ArrTimeDest;ArrBattDest;ArrLoadDest;RecTime;PlugUsed;StartRecharging" << endl;

    class SolutionToFile : public Explorer {
        ostream& f;
        int route_id;
        double max_q;
    public:
        explicit  SolutionToFile(const Solution& x, ostream& f) : Explorer(x), f(f) {}
        void before_route(const Route& route) override {
            route_id = route.id() + 1;
            max_q = 0;
            for (const CustomerNode* node : route.customers()) max_q += node->demand;
        }
        void on_arrival_to_customer(const Node& previous, const CustomerNode& node) override {
            double d_ij = Node::d(previous, node);
            f << route_id << ";"
              << previous.id << ";"
              << node.id << ";"
              << d_ij << ";"
              << node.release_date << ";"
              << node.deadline - node.service_time << ";"
              << node.service_time << ";"
              << node.demand << ";"
              << max( t(), node.release_date ) << ";"
              << b() << ";"
              << max_q - q() << ";"
              << 0 << ";"
              << 0 << ";"
              << 0
              << endl;
        }
        void on_arrival_to_station(const Node& previous, const StationNode& node, const Interval& interval) override {
            if (interval.from() < t()) throw runtime_error("Charging before arrival");
            f << route_id << ";"
              << previous.id << ";"
              << node.id << ";"
              << Node::d(previous, node) << ";"
              << 0 << ";"
              << 0 << ";"
              << 0 << ";"
              << 0 << ";"
              << t() << ";"
              << b() << ";"
              << max_q - q() << ";"
              << interval.span() << ";" // recharging time
              << 1 << ";" // plug_used
              << interval.from() // start recharging
              << endl;
        }
        void on_arrival_to_node(const Node& previous, const Node& node) override {
            if (node.id != Instance::get().depot().id) return;
            f << route_id << ";"
              << previous.id << ";"
              << node.id << ";"
              << Node::d(previous, node) << ";"
              << 0 << ";"
              << 0 << ";"
              << 0 << ";"
              << 0 << ";"
              << t() << ";"
              << b() << ";"
              << max_q - q() << ";"
              << 0 << ";"
              << 0 << ";"
              << 0
              << endl;
        }
    };

    SolutionToFile(*this, f).explore();

    f.close();
}

void Solution::Explorer::explore() {
    Instance& instance = Instance::get();

    for (const Route& route : _x._fixed_routes.routes()) {
        _t = 0;
        _b = instance.battery_capacity();
        _d = 0;
        _q = 0;
        before_route(route);
        const Node* last_visited_node = &instance.depot();
        for (const CustomerNode* customer : route.customers()) {
            if (_x._charging_decisions.count({ last_visited_node->id, customer->id }) > 0) {
                StationSchedule::Entry entry = _x._charging_decisions.at({ last_visited_node->id, customer->id });
                const double d_is = Node::d(*last_visited_node, *entry.station);
                const double d_sj = Node::d(*entry.station, *customer);
                const double t_is = instance.distance_to_time(d_is);
                const double t_sj = instance.distance_to_time(d_sj);
                const double b_is = instance.distance_to_consumed_battery(d_is);
                const double b_sj = instance.distance_to_consumed_battery(d_sj);

                _d += d_is;
                _b -= b_is;
                _t += t_is;

                on_arrival_to_node(*last_visited_node, *entry.station);
                on_arrival_to_station(*last_visited_node, *entry.station, entry.interval);

                _b += instance.time_to_battery_gain(entry.interval.span());
                _t = entry.interval.to();

                on_departure_from_station(*last_visited_node, *entry.station);
                on_departure_from_node(*last_visited_node, *entry.station);

                _d += d_sj;
                _t += t_sj;
                _b -= b_sj;

                on_arrival_to_node(*entry.station, *customer);
                on_arrival_to_customer(*entry.station, *customer);

                if (_t < customer->release_date) _t = customer->release_date;
                _t += customer->service_time;

                on_departure_from_customer(*entry.station, *customer);
                on_departure_from_node(*entry.station, *customer);

            } else {
                const double d_ij = Node::d(*last_visited_node, *customer);
                const double t_ij = instance.distance_to_time(d_ij);
                const double b_ij = instance.distance_to_consumed_battery(d_ij);

                _d += d_ij;
                _t += t_ij;
                _b -= b_ij;
                on_arrival_to_node(*last_visited_node, *customer);
                on_arrival_to_customer(*last_visited_node, *customer);

                if (_t < customer->release_date) _t = customer->release_date;
                _t += customer->service_time;

                _q += customer->demand;

                on_departure_from_customer(*last_visited_node, *customer);
                on_departure_from_node(*last_visited_node, *customer);
            }

            last_visited_node = customer;
        }
        const Node& depot = instance.depot();
        if (_x._charging_decisions.count({ last_visited_node->id, depot.id }) == 0) {
            const double d_ij = Node::d(*last_visited_node, instance.depot());
            const double t_ij = instance.distance_to_time(d_ij);
            const double b_ij = instance.distance_to_consumed_battery(d_ij);
            _d += d_ij;
            _t += t_ij;
            _b -= b_ij;
            on_arrival_to_node(*last_visited_node, depot);
        } else {
            StationSchedule::Entry entry = _x._charging_decisions.at({ last_visited_node->id, depot.id });
            const double d_is = Node::d(*last_visited_node, *entry.station);
            const double d_sj = Node::d(*entry.station, depot);
            const double t_is = instance.distance_to_time(d_is);
            const double t_sj = instance.distance_to_time(d_sj);
            const double b_is = instance.distance_to_consumed_battery(d_is);
            const double b_sj = instance.distance_to_consumed_battery(d_sj);

            _d += d_is;
            _b -= b_is;
            _t += t_is;

            on_arrival_to_node(*last_visited_node, *entry.station);
            on_arrival_to_station(*last_visited_node, *entry.station, entry.interval);

            _b += instance.time_to_battery_gain(entry.interval.span());
            _t = entry.interval.to();

            on_departure_from_station(*last_visited_node, *entry.station);
            on_departure_from_node(*last_visited_node, *entry.station);

            _d += d_sj;
            _t += t_sj;
            _b -= b_sj;

            on_arrival_to_node(*entry.station, depot);
        }

        // remains from last_visited_node to depot
        after_route(route);
        _D += _d;
    }
}
