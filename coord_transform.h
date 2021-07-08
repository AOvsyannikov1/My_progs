#pragma once

#include <iostream>
#include "diff.h"

typedef struct three_phase {
    double A;
    double B;
    double C;
} three_phase_t;

typedef struct two_phase_ab {
    double A;
    double B;
} two_phase_ab_t;

typedef struct two_phase_dq {
    double d;
    double q;
} two_phase_dq_t;

three_phase_t three_phase_voltage(double t, double w, double Ud);
two_phase_ab clark(three_phase_t vect);
three_phase_t inv_clark(two_phase_ab vect);
two_phase_dq park(two_phase_ab vect_ab, double angle);
two_phase_ab inv_park(two_phase_dq vect_dq, double angle);