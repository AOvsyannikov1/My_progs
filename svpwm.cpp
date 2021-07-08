
#include "svpwm.h"

two_phase_ab vector_pwm(double Ud, double k_mod, double freq, double t, double pwm_freq) {
	three_phase_t vect;
	two_phase_ab vect1;
	double d_mod = 2 * pwm_freq * dt;
	static double mod = -1;

	if (k_mod > 1) k_mod = 1;
	if (k_mod < 0) k_mod = 0;

	double ua = k_mod * sin(2 * pi * freq * t);
	double ub = k_mod * sin(2 * pi * freq * t - 2 * pi / 3);
	double uc = k_mod * sin(2 * pi * freq * t + 2 * pi / 3);

	if (ua >= mod) vect.A = 1;
	else vect.A = 0;

	if (ub >= mod) vect.B = 1;
	else vect.B = 0;

	if (uc >= mod) vect.C = 1;
	else vect.C = 0;

	vect1.A = 0.408 * (vect.A - 0.5 * vect.B - 0.5 * vect.C) * Ud;
	vect1.B = 0.408 * 0.866 * (vect.B - vect.C) * Ud;

	if (mod >= 1) mod = -1;
	else mod += d_mod;

	return vect1;
}