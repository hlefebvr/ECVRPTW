#include <iostream>
#include <vector>

#include "Instance.h"
#include "Solver.h"

using namespace std;

int main() {
    vector<string> inputs = {
            "../input/Instance1_2s_30c_4v.txt",
            "../input/Instance2_2s_30c_4v.txt",
            "../input/Instance2_2s_40c_4v.txt",
            "../input/Instance3_2s_40c_4v.txt",
            "../input/Instance4_2s_40c_4v.txt",
            "../input/Instance4_3s_45c_6v.txt"
    };
    const string output = "./output.csv";
    const double max_execution_time = 120;

    Instance::load_from_file(inputs[0]);

    auto solver = Solver(max_execution_time);
    solver.run();

    return 0;
}
