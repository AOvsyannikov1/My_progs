#pragma once

#include <iostream>
#include <stdlib.h>
#include "diff.h"
#include "coord_transform.h"


two_phase_ab vector_pwm(double Ud, double k_mod, double freq, double t, double pwm_freq);
