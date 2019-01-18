#include "Solution.h"

void Solution::print() const {
    Instance& instance = Instance::get();
    cout.precision(2);

    class SolutionWriter : public Explorer {
    protected:
        void on_arrival_to_station(const StationNode& node) override {
            cout << fixed << "STATION \t" << node.id << "\t" << _t << "\t" << _d << "\t" << _b << "\t" << _q << "\t" << endl;
        }
        void on_departure_from_station(const StationNode& node) override {
            cout << fixed << "STATION \t" << node.id << "\t" << _t << "\t" << _d << "\t" << _b << "\t" << _q << "\t" << endl;
        }
        void on_departure_from_customer(const CustomerNode& node) override {
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

                on_arrival_to_node(*entry.station);
                on_arrival_to_station(*entry.station);

                _b += instance.time_to_battery_gain(entry.interval.span());
                _t = entry.interval.to();

                on_departure_from_station(*entry.station);
                on_departure_from_node(*entry.station);

                _d += d_sj;
                _t += t_sj;
                _b -= b_sj;

                on_arrival_to_node(*customer);
                on_arrival_to_customer(*customer);

                if (_t < customer->release_date) _t = customer->release_date;
                _t += customer->service_time;

                on_departure_from_customer(*customer);
                on_departure_from_node(*customer);

            } else {
                const double d_ij = Node::d(*last_visited_node, *customer);
                const double t_ij = instance.distance_to_time(d_ij);
                const double b_ij = instance.distance_to_consumed_battery(d_ij);

                _d += d_ij;
                _t += t_ij;
                _b -= b_ij;
                on_arrival_to_node(*customer);
                on_arrival_to_customer(*customer);

                if (_t < customer->release_date) _t = customer->release_date;
                _t += customer->service_time;

                _q += customer->demand;

                on_departure_from_customer(*customer);
                on_departure_from_node(*customer);
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
            on_arrival_to_node(depot);
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

            on_arrival_to_node(*entry.station);
            on_arrival_to_station(*entry.station);

            _b += instance.time_to_battery_gain(entry.interval.span());
            _t = entry.interval.to();

            on_departure_from_station(*entry.station);
            on_departure_from_node(*entry.station);

            _d += d_sj;
            _t += t_sj;
            _b -= b_sj;

            on_arrival_to_node(depot);
        }

        // remains from last_visited_node to depot
        after_route(route);
    }
}
