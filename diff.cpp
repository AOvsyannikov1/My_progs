
#include "diff.h"

double v_to_w(double v) {
    return (2 * gear_ratio / (3.6 * Db)) * v;
}

double w_to_v(double w) {
    return (3.6 * Db / (2 * gear_ratio)) * w;
}

double osn_sopr_lok(double v) {
    //if(abs(v) >= 0.5) return ((1.9 + 0.01 * abs(v) + 0.0003 * v * v) * g * mass_el / 1000) * sign(v); 
    /*else*/ return 0;
}

double osn_sopr_sost(double v) {
    if (abs(v) >= 0.5) return ((0.7 + ((8 + 0.16 * abs(v) + 0.0023 * v * v)) / (mass_car / 4000)) * g * mass_train / 1000) * sign(v);
    else if (v < 0.5 && v >= 0.05) return ((28 / ((mass_car / 4000) + 7)) * g * mass_train / 1000) * sign(v);
    else return 0;
}

double dpsia_dt(double isa0, double isb0, double psira0, double psirb0, double wk, double w0) {                                             //потокосцепление ротора а
    return ((Lm * rr / Lr) * isa0 + (wk - w0 * p) * psirb0 - (rr / Lr) * psira0);
}

double dpsib_dt(double isa0, double isb0, double psira0, double psirb0, double wk, double w0) {                                             //потокосцепление ротора b
    return ((Lm * rr / Lr) * isb0 - (wk - w0 * p) * psira0 - (rr / Lr) * psirb0);
}

double disa_dt(double isa0, double isb0, double psira0, double psirb0, double wk, double w0, double Usa) {
    return ((-rs * isa0 - (Lm / Lr) * dpsia_dt(isa0, isb0, psira0, psirb0, wk, w0) + wk * (Lse * isb0 + (Lm / Lr) * psirb0) + Usa) / Lse);  //ток статора а
}

double disb_dt(double isa0, double isb0, double psira0, double psirb0, double wk, double w0, double Usb) {
    return ((-rs * isb0 - (Lm / Lr) * dpsib_dt(isa0, isb0, psira0, psirb0, wk, w0) - wk * (Lse * isa0 + (Lm / Lr) * psira0) + Usb) / Lse);  //ток статора b
}

unsigned char sign(double arg) {
    if (arg > 0) return 1;
    else if (arg == 0) return 0;
    else return -1;
}

double dv_dt(double isa0, double isb0, double psira0, double psirb0, double wk, double w0, bool const_speed) {
    if (const_speed == false) {
        double M_tr = fric_coeff * w0;
        double W = 0.5 * Db * (osn_sopr_lok(w_to_v(w0)) + osn_sopr_sost(w_to_v(w0))) * sign(w0) / gear_ratio;
        double Mk = 1.5 * p * (Lm / Lr) * (isb0 * psira0 + -isa0 * psirb0) * kpd_red * kpd_ted * n_ted;
        double Jdob = mass * Db * Db / (4 * gear_ratio * gear_ratio);
        return (Mk - (M_tr + W)) / (J + Jdob);
    }
    else return 0;
}