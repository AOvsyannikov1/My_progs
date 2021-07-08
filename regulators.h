#pragma once

#include "diff.h"
#include <iostream>

double PI(double delta, double K_p, double K_i);
double PID(double delta, double K_p, double K_i, double K_d, double T_d);
double inert_aper_1(double K, double T, double u);