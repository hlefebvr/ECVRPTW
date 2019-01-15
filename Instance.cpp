#include "Instance.h"


Instance* Instance::instance = nullptr;

Instance& Instance::load_from_file(const string &input_data_file) {
    if(instance) throw runtime_error("An instance has already been loaded");
    instance = new Instance(input_data_file);
    return *instance;
}

Instance& Instance::get() {
    if (!instance) throw runtime_error("Trying to access unloaded instance");
    return *instance;
}

Instance::Instance(const string& input_data_file) : _name(input_data_file) {
    ifstream reader;
    reader.open(input_data_file);

    enum SECTION { NONE, NODES, RECHARGERS, CUSTOMERS, INSTDATA };
    enum COL_NODES {NODE_ID, X, Y, TYPE};
    enum COL_RECHARGERS {RECHARGER_ID, NUMREC};
    enum COL_CUSTOMERS { CUSTOMER_ID, Q, ST, R, D };
    enum COL_INSTDATA { TMAX, NUMV, CAPACITY, SPEED, DISTCOST, BATT, RO, ALPHA };

    SECTION current_section = NONE;

    map<int, pair<double, double>> coords;
    bool new_section = false;

    while (!reader.eof()) {
        auto row = get_next_row(reader);

        if (row.size() == 0) continue; // skip empty lines

        if (row.size() == 1) {
            if (row[0] == "NODES") current_section = NODES;
            else if (row[0] == "RECHARGERS") current_section = RECHARGERS;
            else if (row[0] == "CUSTOMERS") current_section = CUSTOMERS;
            else if (row[0] == "INSTDATA") current_section = INSTDATA;
            else throw runtime_error("Unknown section title in input file.");
            new_section = true;
            continue;
        }

        if (new_section) {
            new_section = false;
            continue;
        }

        if (current_section == NODES) {
            int id = stoi(row[NODE_ID]);
            double x = stod(row[X]);
            double y=  stod(row[Y]);
            if (row[COL_NODES::TYPE] == "D") _depot = new Node(id, x, y);
            coords.insert(pair<int, pair<double, double>>(id, pair<double, double>(x, y)));
            continue;
        }

        if (current_section == RECHARGERS) {
            int id = stoi(row[RECHARGER_ID]);
            int n_plug = stoi(row[NUMREC]);
            auto coord = coords.at(id);
            auto new_station = new StationNode(id, coord.first, coord.second, n_plug);
            _vstations.push_back(new_station);
            _n_stations++;
            continue;
        }

        if (current_section == CUSTOMERS) {
            int id = stoi(row[CUSTOMER_ID]);
            auto coord = coords.at(id);
            double demand = stof(row[Q]);
            double service_time = stof(row[ST]);
            double release_date = stof(row[R]);
            double due_date = stof(row[D]);
            auto new_customer = new CustomerNode(id, coord.first, coord.second, demand, service_time, release_date, due_date);
            _vcustomers.push_back(new_customer);
            _overall_demand += demand;
            _n_customers++;
            continue;
        }

        if (current_section == INSTDATA) {
            _max_tour_time = stof(row[TMAX]);
            _vehicle_capacity = stof(row[CAPACITY]);
            _vehicle_count = stoi(row[NUMV]);
            _vehicle_speed = stof(row[SPEED]);
            _unitary_cost = stof(row[DISTCOST]);
            _battery_capacity = stof(row[BATT]);
            _battery_consumption_rate = stof(row[RO]);
            _battery_recharge_rate = stof(row[ALPHA]);
            continue;
        }

    }

    reader.close();
}

vector<string> Instance::get_next_row(ifstream& reader) {
    char* placeholder = new char[MAX_LINE_SIZE];
    reader.getline(placeholder, MAX_LINE_SIZE);
    string line(placeholder);

    regex reg = regex("[[:alnum:][:punct:]]+");
    smatch match;

    vector<string> cols;
    while( regex_search(line, match, reg) ) {
        cols.push_back(match[0]);
        line = match.suffix().str();
    }

    delete placeholder;

    return cols;
}

// Instance::Instance(const Instance& orig) {}

Instance::~Instance() {
    if (_depot != nullptr) delete _depot;

    for (auto customer : _vcustomers) delete customer;
    for (auto station : _vstations) delete station;
}

const StationNode &Instance::closest_station_between(const Node&A, const Node& B, double* detour) const {

    const StationNode* argmin = nullptr;
    double min = numeric_limits<double>::max();
    for (const StationNode* s : _vstations) {
        if (s->id == 0) continue; // TODO : use depot.id
        const double detour = Node::d(A, *s) + Node::d(*s, B);
        if (detour < min) {
            min = detour;
            argmin = s;
        }
    }

    if (argmin == nullptr) throw runtime_error("No station in the instance");
    if (detour != nullptr) *detour = min;

    return *argmin;
}

