
#include "param.h"

#define N_cur 10
#define N_rms 5


double rms(double val, double f) {
	static double tmp = 0;
	static double sum = 0;
    static double prev_val = 0;
	static double time = 0;
    double T = 0.5 / f;
    double integral = 0.5 * (val * val + prev_val * prev_val) * dt;
    sum += integral;
    
    if (time >= T) {
        tmp = sqrt(sum / T);
        time = 0;
        sum = 0;
    }
    time += dt;
    prev_val = val;
	return tmp;
}

double filter_cur(double a) {
    static double m[N_cur];
    static int n;
    m[n] = a;
    n = (n + 1) % N_cur;
    a = 0;
    for (int j = 0; j < N_cur; j++) { a = a + m[j]; }
    return a / N_cur;
}

double filter_rms(double a) {
    static double m[N_rms];
    static int n;
    m[n] = a;
    n = (n + 1) % N_rms;
    a = 0;
    for (int j = 0; j < N_cur; j++) { a = a + m[j]; }
    return a / N_rms;
}

void diff_process(double t, double tk) {
    static unsigned char flags[10] = { 0,0,0,0,0,0,0,0,0,0 };
    if (t >= 0.1 * tk && flags[0] == 0) {
        flags[0] = 1;
        std::cout << "10%" << "\n\t";
    }
    if (t >= 0.2 * tk && flags[1] == 0) {
        std::cout << "20%" << "\n\t";
        flags[1] = 1;
    }
    if (t >= 0.3 * tk && flags[2] == 0) {
        std::cout << "30%" << "\n\t";
        flags[2] = 1;
    }
    if (t >= 0.4 * tk && flags[3] == 0) {
        flags[3] = 1;
        std::cout << "40%" << "\n\t";
    }
    if (t >= 0.5 * tk && flags[4] == 0) {
        std::cout << "50%" << "\n\t";
        flags[4] = 1;
    }
    if (t >= 0.6 * tk && flags[5] == 0) {
        std::cout << "60%" << "\n\t";
        flags[5] = 1;
    }
    if (t >= 0.7 * tk && flags[6] == 0) {
        flags[6] = 1;
        std::cout << "70%" << "\n\t";
    }
    if (t >= 0.8 * tk && flags[7] == 0) {
        std::cout << "80%" << "\n\t";
        flags[7] = 1;
    }
    if (t >= 0.9 * tk && flags[8] == 0) {
        std::cout << "90%" << "\n\t";
        flags[8] = 1;
    }
    if (t >= 0.99 * tk && flags[9] == 0) {
        std::cout << "99%" << std::endl;
        flags[9] = 1;
    }
}

invertor_in_t start_el(double f0, double fk, double tk_f, double t) {
    invertor_in_t vect;
    int Umax = 3500;
    static double local_time = 0;
    static double f = f0;
    static double mod = (Umax / 65.8) * f0;
    double df = 0.001;
    double N = (fk - f0) / df;
    double dt_f = tk_f / N;
    

    if (local_time >= dt_f) {
        local_time = 0;
        f += df;
        mod = (Umax / 65.8) * f;
    }
    if (t >= tk_f) {
        f = 2 * (fk - f0);
        mod = (Umax / 65.8) * f;
    }
    vect.f = f;
    vect.U = mod;
    local_time += dt;
    return vect;
}