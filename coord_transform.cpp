
#include "coord_transform.h"


three_phase_t three_phase_voltage(double t, double w, double Ud) {
    three_phase_t vect;
    vect.A = Ud * sqrt(2) * sin(w * t);
    vect.B = Ud * sqrt(2) * sin(w * t - 2 * pi / 3);
    vect.C = Ud * sqrt(2) * sin(w * t + 2 * pi / 3);
    return vect;
}

two_phase_ab clark(three_phase_t vect) {
    two_phase_ab vect1;
    vect1.A = vect.A;
    vect1.B = (vect.B - vect.C) / sqrt(3);
    return vect1;
}

three_phase_t inv_clark(two_phase_ab vect) {
    three_phase_t vect1;
    vect1.A = vect.A;
    vect1.B = (sqrt(3) / 2) * vect.B - 0.5 * vect.A;
    vect1.C = -0.5 * vect.A - (sqrt(3) / 2) * vect.B;
    return vect1;
}

two_phase_dq park(two_phase_ab vect_ab, double angle) {
    two_phase_dq vect_dq;
    vect_dq.d = vect_ab.B * sin(angle) + vect_ab.A * cos(angle);
    vect_dq.q = vect_ab.B * cos(angle) - vect_ab.A * sin(angle);
    return vect_dq;
}

two_phase_ab inv_park(two_phase_dq vect_dq, double angle) {
    two_phase_ab vect_ab;
    vect_ab.A = vect_dq.d * cos(angle) - vect_dq.q * sin(angle);
    vect_ab.B = vect_dq.d * sin(angle) + vect_dq.q * cos(angle);
    return vect_ab;
}