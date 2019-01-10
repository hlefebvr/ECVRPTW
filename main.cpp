#include <iostream>
#include <vector>

#include "Instance.h"
#include "RelaxedSolution.h"

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
    const double max_execution_time = 10;

    Instance::load_from_file(inputs[0]);

    auto x = RelaxedSolution();

    return 0;
}
