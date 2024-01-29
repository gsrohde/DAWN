#ifndef SOLVER_H
#define SOLVER_H

#include <string>

struct Solver {
    std::string name {"boost_rkck54"};
    double step_size {1.0};
    double relative_tolerance {1e-4};
    double absolute_tolerance {1e-4};
    int max_steps {200};
};


#endif
