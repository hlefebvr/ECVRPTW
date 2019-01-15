#ifndef ECVRPTW_STATIONSCHEDULE_H
#define ECVRPTW_STATIONSCHEDULE_H
#include <set>
#include <vector>

#include "Node.h"
#include "Interval.h"
#include "Instance.h"
#include "Utils.h"


class StationSchedule {
public:
    struct Entry {
        Entry(const Interval& interval, const StationNode& station);
        Interval interval;
        const StationNode* station;
        bool operator<(const Entry& rhs) const;
    };

    const StationNode& station;
    explicit StationSchedule(const StationNode& station);
    StationSchedule(const StationSchedule& copied);
    Entry& add_entry(const Interval& interval);
    vector<pair<Interval, int>> availabilities_within(const Interval& interval) const;
    bool is_available(const Interval& interval, int* plug = nullptr);
    ~StationSchedule();

    void add_entry(const Interval& interval, int plug) {
        entries[plug].insert(new Entry(interval, station));
    }

private:
    vector<set<Entry*, ptr_less<Entry> >> entries;
};


#endif //ECVRPTW_STATIONSCHEDULE_H
