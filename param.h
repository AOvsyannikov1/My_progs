
#pragma once
#include <iostream>

#include "diff.h"

typedef struct invertor_in {
	double U;
	double f;
} invertor_in_t;

double rms(double val, double f);
void diff_process(double t, double tk);
invertor_in_t start_el(double f0, double fk, double tk_f, double t);
double filter_cur(double a);
double filter_rms(double a);
