cmake_minimum_required(VERSION 3.13)
project(ECVRPTW)

set(CMAKE_CXX_STANDARD 14)

add_executable(ECVRPTW main.cpp Instance.cpp Instance.h Node.cpp Node.h Solver.cpp Solver.h SimulatedAnnealing.cpp SimulatedAnnealing.h IteratedLocalSearch.cpp IteratedLocalSearch.h RelaxedSolution.cpp RelaxedSolution.h Route.cpp Route.h Solution.cpp Solution.h SolutionCallback.cpp SolutionCallback.h StationSchedule.cpp StationSchedule.h Interval.cpp Interval.h Utils.h)