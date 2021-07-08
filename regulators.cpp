
#include "regulators.h"

double proporc(double delta, double K_p) {
	return K_p * delta;
}

double integr(double delta, double K_i) {
	static double sum = 0;
	sum += K_i * delta * dt;
	return sum;
}

double diff(double delta, double K_d, double T_d) {
	static double prev_delta = 0;
	static double out = 0;
	double deriv = (delta - prev_delta) / dt;
	out += dt * (K_d * deriv - out) / T_d;
	prev_delta = delta;
	return out;
}

double PI(double delta, double K_p, double K_i) {
	return (proporc(delta, K_p) + integr(delta, K_i));
}

double	PID(double delta, double K_p, double K_i, double K_d, double T_d) {
	return (proporc(delta, K_p) + integr(delta, K_i) + diff(delta, K_d, T_d));
}

double inert_aper_1(double K, double T, double u) {
	static double y = 0;
	y += dt * (K * u - y) / T;
	return y;
}