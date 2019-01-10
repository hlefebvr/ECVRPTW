#include "Node.h"

// Nodes
Node::Node(const int id, const double x, const double y) : id(id), x(x), y(y) {}
Node::Node(const Node& orig) : id(orig.id), x(orig.x), y(orig.y) {}
bool Node::operator==(const Node& other) { return id == other.id; }
bool Node::operator!=(const Node& other) { return id != other.id; }

double Node::d(const Node& A, const Node& B) {
    return sqrt( pow(A.x - B.x, 2) + pow(A.y - B.y, 2) );
}

// Customer nodes
CustomerNode::CustomerNode(int id, double x, double y, double demand, double service_time, double release_date, double deadline)
        : Node(id, x, y), demand(demand), service_time(service_time), release_date(release_date), deadline(deadline) {}
CustomerNode::CustomerNode(const CustomerNode& orig)
        : Node(orig), demand(orig.demand), service_time(orig.service_time), release_date(orig.release_date), deadline(orig.deadline) {}

// Station nodes
StationNode::StationNode(int id, double x, double y, int plug_count)
        : Node(id, x, y), plug_count(plug_count) {}
StationNode::StationNode(const StationNode& orig)
        : Node(orig), plug_count(orig.plug_count) {}

