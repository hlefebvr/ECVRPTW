#include <iostream>
#include <vector>

#include "Instance.h"
#include "Solver.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Usage : ECVRPTW <instance_file.txt> <max_execution_time>" << endl;
        return -1;
    }

    const Instance& instance = Instance::load_from_file(argv[1]);
    const double max_execution_time = atoi(argv[2]);

    unsigned long int feasible_found = 0, skipped = 0, n_try = 0;
    double sum_obj = 0, sum_exec_time = 0;
    while ( (feasible_found == 0) || (n_try < 5) ) {
        try {
            auto solver = Solver(max_execution_time);

            double exec_time, distance;
            const Solution &x_opt = solver.run(&distance, &exec_time);
            const double objective = distance * instance.unitary_cost();

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

    ofstream f;
    f.open(instance.folder() + "/output.csv", ios::out | ios::app);
    f << instance.pure_name() << ";" << avg_exec_time << ";" << avg_obj << endl;
    f.close();

    return 0;
}
