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
            "../input/Instance4_3s_45c_6v.txt",
            "../input/NewInstance3_2s_40c_4v.txt",
            "../input/NewInstance4_2s_40c_4v.txt",
            "../input/NewInstance4_3s_45c_6v.txt"
    };
    const string output = "./output.csv";
    const double max_execution_time = 10;

    Instance::load_from_file(inputs[8]);

    unsigned long int feasible_found = 0, skipped = 0, n_try = 0;
    double sum_obj = 0, sum_exec_time = 0;
    while ( (feasible_found == 0) || (n_try < 5) ) {
        try {
            auto solver = Solver(max_execution_time);

            double exec_time, distance;
            const Solution &x_opt = solver.run(&distance, &exec_time);
            const double objective = distance * Instance::get().unitary_cost();

            x_opt.to_file(objective, exec_time);

            feasible_found += 1;
            sum_obj += objective;
            sum_exec_time += exec_time;
            cout << "Found one feasible solution in " << exec_time << "s (objective : " << objective << ")" << endl;
        } catch (runtime_error &err) {
            cout << err.what() << endl;
            skipped += 1;
        }
        n_try += 1;
    }

    cout << "Skipped : " << skipped << endl << endl;

    const double avg_obj = sum_obj / (double) feasible_found;
    const double avg_exec_time = sum_exec_time / (double) feasible_found;
    cout << "Average objective : " << avg_obj << endl;
    cout << "Average execution time : " << avg_exec_time << endl;

    return 0;
}
