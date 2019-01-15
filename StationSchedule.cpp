#include "StationSchedule.h"

StationSchedule::Entry::Entry(const Interval &interval, const StationNode &station)
        : interval(Interval(interval)), station(&station)
{}

bool StationSchedule::Entry::operator<(const Entry &rhs) const {
    return interval < rhs.interval;
}

StationSchedule::StationSchedule(const StationNode &station) : station(station) {
    entries.resize(station.plug_count);
    const Instance& instance = Instance::get();
    Interval time_bound = Interval(instance.max_tour_time(), numeric_limits<double>::max());
    for (unsigned long int i = 0, n = entries.size() ; i < n ; i++) {
        Entry* new_entry = new Entry(time_bound, station);
        entries[i].insert(new_entry);
    }
}

StationSchedule::StationSchedule(const StationSchedule &copied) : StationSchedule(copied.station) {}

StationSchedule::Entry& StationSchedule::add_entry(const Interval &interval) {
    int plug;
    if (is_available(interval, &plug)) {
        auto new_entry = new Entry(interval, station);
        entries[plug].insert(new_entry);
        return *new_entry;
    }
    else throw runtime_error("Requested usage is not available");
}

StationSchedule::~StationSchedule() {
    for (auto p_schedule : entries)
        for (auto entry : p_schedule) delete entry;
}

vector<pair<Interval, int>> StationSchedule::availabilities_within(const Interval &interval) const {
    vector<pair<Interval, int>> availabilities;

    int plug_id = 0;
    for (auto plug_schedule : entries) {

        Entry zero = Entry(Interval(0,0), station);
        Entry* last_entry = &zero;
        for (auto entry : plug_schedule) {
            Interval space_in_between( last_entry->interval.to(), entry->interval.from() );
            if (space_in_between.span() > 0 && !interval.intersection_is_empty(space_in_between) ) {
                Interval availability = space_in_between - interval;
                availabilities.push_back({ availability, plug_id });
            }
            last_entry = entry;
        }

        plug_id++;
    }

    return availabilities;
}

bool StationSchedule::is_available(const Interval &interval, int *plug) {
    // first fit heuristic
    auto availabilities = availabilities_within(interval);

    for (auto availability : availabilities) {
        if (availability.first == interval) {
            if (plug != nullptr) *plug = availability.second;
            return true;
        }
    }

    return false;
}
