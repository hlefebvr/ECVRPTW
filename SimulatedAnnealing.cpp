#include "SimulatedAnnealing.h"

SimulatedAnnealing::SimulatedAnnealing(SolutionCallback &cb)
    : _cb(cb), _nb_iter(0), _current_solution(RelaxedSolution()), _current_objetive_value(_current_solution.evaluate())
    {}

void SimulatedAnnealing::iter() {
    default_random_engine generator((unsigned long) chrono::system_clock::now().time_since_epoch().count());
    uniform_real_distribution<double> uni_dist(0, 1);

    RelaxedSolution x = _current_solution;
    move(x);

    const double objective_value = x.evaluate();
    const bool is_feasible = x.feasible();

    if (is_feasible) _cb.relaxed_callback(x);

    const bool objective_value_improvement = objective_value <= _current_objetive_value;
    const bool accepted_anyway = uni_dist(generator) <= exp( -( objective_value - _current_objetive_value ) / _temperature );

    if (objective_value_improvement || accepted_anyway) {
        _current_solution = x;
        _current_objetive_value = objective_value;
        _temperature *= _alpha;
    }

    _nb_iter += 1;
}

void SimulatedAnnealing::move(RelaxedSolution& x) {

    if (_nb_iter <= 400) return x.one_point_move();

    if (_nb_iter <= 800) return x.two_opt_move();

    _nb_iter = 0;
    move(x);
}

