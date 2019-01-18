#ifndef ECVRPTW_SOLUTION_H
#define ECVRPTW_SOLUTION_H

#include <map>
#include <vector>

#include "StationSchedule.h"
#include "RelaxedSolution.h"

using namespace std;

class Solution {
    const map<pair<int, int>, StationSchedule::Entry> _charging_decisions;
    const RelaxedSolution _fixed_routes;
public:
    class Explorer {
    protected:
        double _t = 0, _b = 0, _q = 0, _d = 0, _D = 0;
        const Solution& _x;
    public:
        explicit Explorer(const Solution& x) : _x(x) {}
        void explore();
        virtual void before_route(const Route& route) {}
        virtual void after_route(const Route& route) {}
        virtual void on_arrival_to_node(const Node& node) {}
        virtual void on_arrival_to_customer(const CustomerNode& customer){}
        virtual void on_arrival_to_station(const StationNode& station) {}
        virtual void on_departure_from_node(const Node& node) {}
        virtual void on_departure_from_customer(const CustomerNode& customer) {}
        virtual void on_departure_from_station(const StationNode& station) {}
        double t() const { return _t; }
        double b() const { return _b; }
        double q() const { return _q; }
        double d() const { return _d; }
        double D() const { return _D; }
    };
    Solution(const RelaxedSolution& fixed_routes, const map<pair<int, int>, StationSchedule::Entry>& charging_decisions) : _fixed_routes(fixed_routes), _charging_decisions(charging_decisions) {}
    Solution(const Solution&) = default;
    void print() const;
};


#endif //ECVRPTW_SOLUTION_H
