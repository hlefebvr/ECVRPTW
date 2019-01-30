#ifndef ECVRPTW_INSTANCE_H
#define ECVRPTW_INSTANCE_H


#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <regex>

#include "Node.h"

#define MAX_LINE_SIZE 200

using namespace std;

class Instance {
private:
    Instance(const string& input_data_file);

    static Instance* instance;

    const string _name;

    double _max_tour_time;

    // vehicles
    int _vehicle_count;
    double _vehicle_capacity;
    double _vehicle_speed;

    // costs
    double _unitary_cost;

    // batteries
    double _battery_capacity;
    double _battery_consumption_rate;
    double _battery_recharge_rate;

    // nodes
    vector<const CustomerNode*> _vcustomers;
    vector<const StationNode*> _vstations;
    const Node* _depot = nullptr;

    // computed values
    int _n_stations = 0;
    int _n_customers = 0;
    double _overall_demand = 0;

    vector<string> get_next_row(ifstream& reader);

public:
    Instance(const Instance& orig) = delete;
    Instance& operator=(const Instance&) = delete;
    virtual ~Instance();

    static Instance& load_from_file(const string& input_data_file);
    static Instance& get();

    // getters
    int vehicle_count() const { return _vehicle_count; }
    double max_tour_time() const { return _max_tour_time; }
    double vehicle_capacity() const { return _vehicle_capacity; }
    double vehicle_speed() const { return _vehicle_speed; }
    double unitary_cost() const { return _unitary_cost; }
    double battery_capacity() const { return _battery_capacity; }
    double battery_consumption_rate() const { return _battery_consumption_rate; }
    double battery_recharge_rate() const { return _battery_recharge_rate; }
    int customer_count() const { return _n_customers; }
    int station_count() const { return _n_stations; }
    double overall_demand() const { return _overall_demand; }
    const string& name() const { return _name; }

    const vector<const CustomerNode*>& customers() const { return _vcustomers; }
    const vector<const StationNode*>& stations() const { return _vstations; }
    const Node& depot() const { return *_depot; }

    double distance_to_time(double d) const { return d / vehicle_speed(); }
    double time_to_distance(double t) const { return t * vehicle_speed(); }
    double distance_to_consumed_battery(double d) const { return d * battery_consumption_rate(); }
    double time_to_battery_gain(double t) const { return t * battery_recharge_rate(); }
    double time_needed_to_charge(double b) const { return b / battery_recharge_rate(); }

    string get_result_filename() const { return _name.substr(0, _name.find_last_of('.')) + "_Result.csv"; }

    const StationNode& closest_station_between(const Node& A, const Node& B, double* detour = nullptr) const;
    const StationNode& farthest_station_between(const Node& A, const Node& B, double* detour = nullptr) const;
};

#endif //ECVRPTW_INSTANCE_H
