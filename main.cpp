#include <iostream>
#include <vector>

#include "Instance.h"
#include "InstanceVerifier.h"
#include "Solver.h"

using namespace std;

int main() {
    vector<string> inputs = {
            "../input/Instance1_2s_30c_4v.txt",
            "../input/Instance2_2s_30c_4v.txt",
            "../input/Instance2_2s_40c_4v.txt",
            "../input/Instance3_2s_40c_4v.txt", //
            "../input/Instance4_2s_40c_4v.txt",
            "../input/Instance4_3s_45c_6v.txt"
    };
    const string output = "./output.csv";
    const double max_execution_time = 60;

    Instance::load_from_file(inputs[3]); // ok for 0, 1, 2 ko for 3, 4, 5

    auto solver = Solver(max_execution_time);

    double exec_time, distance;
    const Solution& x_opt = solver.run(&distance, &exec_time);
    x_opt.print();

    cout << "Execution time = " << exec_time << endl;
    cout << "Distance       = " << distance << endl;

    return 0;
}
