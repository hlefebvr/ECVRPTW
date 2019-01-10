#ifndef ECVRPTW_NODE_H
#define ECVRPTW_NODE_H


#include <math.h>

class Node {
public:
    Node(int id, double x, double y);
    Node(const Node& orig);

    const int id;
    const double x;
    const double y;

    static double d(const Node& A, const Node& B);
    bool operator==(const Node& other);
    bool operator!=(const Node& other);
};

class CustomerNode : public Node {
public:
    CustomerNode(int id, double x, double y, double demand, double service_time, double release_date, double deadline);
    CustomerNode(const CustomerNode& orig);

    const double demand;
    const double service_time;
    const double release_date;
    const double deadline;
};

class StationNode : public Node {
public:
    StationNode(int id, double x, double y, int plug_count);
    StationNode(const StationNode& orig);

    const int plug_count;
};



#endif //ECVRPTW_NODE_H
