
#include <iostream>
#include <fstream>
#include <conio.h> 
#include "diff.h"
#include "coord_transform.h"
#include "svpwm.h"
#include "param.h"
#include "regulators.h"

const bool const_speed = true;

three_phase_t voltage;
three_phase_t current;
two_phase_ab_t voltage_clark;
two_phase_ab_t ab_volt;
two_phase_ab_t ab_current;
two_phase_dq_t dq_current;
invertor_in_t inv_in;

void rk4(double t0, double tk);

std::ofstream outf("Results.txt");

double Ud = 5000;                 //действующее напряжение
double f0 = 0.5;                  //частота напряжения
double v0 = 80;                   //начальная скорость локомотива
double w0 = v_to_w(v0);
double f_mod = 2500;
double tn = 0;
double tk = 3;

int main()
{
    rk4(tn, tk);
}


void rk4(double t0, double tk) {
    double t = t0;
    double k1;
    double k2;
    double k3;
    double k4;

    double l1;
    double l2;
    double l3;
    double l4;

    double m1;
    double m2;
    double m3;
    double m4;

    double n1;
    double n2;
    double n3;
    double n4;

    double o1;
    double o2;
    double o3;
    double o4;

    double isa0 = 0;
    double isb0 = 0;
    double psira0 = 0;
    double psirb0 = 0;
    double M = 0;
    double Fk;
    double Fkz = 0;
    double delta;
    double Izd = 200;

    double kp = 0.2;
    double ki = 50;
    double kd = 0;
    double Td = 1;


    while (t <= tk) {
        
        diff_process(t, tk);
        M = 1.5 * p * (Lm / Lr) * (isb0 * psira0 + -isa0 * psirb0);
        Fk = (n_ted * kpd_red * kpd_ted * 2 * M * gear_ratio) / Db;

        inv_in.f = 65.8;
        current = inv_clark(ab_current);
        double Iz = -Izd * sqrt(2) * sin(2* pi * f0 * t);
        delta = Iz - filter_cur(current.A);
        inv_in.U = PID(delta, kp, ki, kd, Td);
        
        /*if (w_to_v(w0) <= 70) Fkz = osn_sopr_lok(w_to_v(w0)) + osn_sopr_sost(w_to_v(w0));
        delta = Fkz - filter(Fk);
        inv_in.U = PID(delta, kp, ki, kd, Td);
        if (inv_in.U >= 3500) inv_in.U = 3500;
        if (inv_in.U <= -3500) inv_in.U = -3500;*/
        
        ab_volt = vector_pwm(inv_in.U, 1, inv_in.f, t, f_mod);

        m1 = dpsia_dt(isa0, isb0, psira0, psirb0, wk, w0) * dt;
        n1 = dpsib_dt(isa0, isb0, psira0, psirb0, wk, w0) * dt;
        k1 = disa_dt(isa0, isb0, psira0, psirb0, wk, w0, ab_volt.A) * dt;
        l1 = disb_dt(isa0, isb0, psira0, psirb0, wk, w0, ab_volt.B) * dt;
        o1 = dv_dt(isa0, isb0, psira0, psirb0, wk, w0, const_speed) * dt;

        m2 = dpsia_dt(isa0 + k1 / 2, isb0 + l1 / 2, psira0 + m1 / 2, psirb0 + n1 / 2, wk, w0 + o1 / 2) * dt;
        n2 = dpsib_dt(isa0 + k1 / 2, isb0 + l1 / 2, psira0 + m1 / 2, psirb0 + n1 / 2, wk, w0 + o1 / 2) * dt;
        k2 = disa_dt(isa0 + k1 / 2, isb0 + l1 / 2, psira0 + m1 / 2,psirb0 + n1 / 2, wk, w0 + o1 / 2, ab_volt.A) * dt;
        l2 = disb_dt(isa0 + k1 / 2, isb0 + l1 / 2, psira0 + m1 / 2, psirb0 + n1 / 2, wk, w0 + o1 / 2, ab_volt.B) * dt;
        o2 = dv_dt(isa0 + k1 / 2, isb0 + l1 / 2, psira0 + m1 / 2, psirb0 + n1 / 2, wk, w0 + o1 / 2, const_speed) * dt;
        
        m3 = dpsia_dt(isa0 + k2 / 2, isb0 + l2 / 2, psira0 + m2 / 2, psirb0 + n2 / 2, wk, w0 + o2 / 2) * dt;
        n3 = dpsib_dt(isa0 + k2 / 2, isb0 + l2 / 2, psira0 + m2 / 2, psirb0 + n2 / 2, wk, w0 + o2 / 2) * dt;
        k3 = disa_dt(isa0 + k2 / 2, isb0 + l2 / 2, psira0 + m2 / 2, psirb0 + n2 / 2, wk, w0 + o2 / 2, ab_volt.A) * dt;
        l3 = disb_dt(isa0 + k2 / 2, isb0 + l2 / 2, psira0 + m2 / 2, psirb0 + n2 / 2, wk, w0 + o2 / 2, ab_volt.B) * dt;
        o3 = dv_dt(isa0 + k2 / 2, isb0 + l2 / 2, psira0 + m2 / 2, psirb0 + n2 / 2, wk, w0 + o2 / 2, const_speed) * dt;
        
        m4 = dpsia_dt(isa0 + k3, isb0 + l3, psira0 + m3, psirb0 + n3, wk, w0 + o3) * dt;
        n4 = dpsib_dt(isa0 + k3, isb0 + l3, psira0 + m3, psirb0 + n3, wk, w0 + o3) * dt;
        k4 = disa_dt(isa0 + k3, isb0 + l3, psira0 + m3, psirb0 + n3, wk, w0 + o3, ab_volt.A) * dt;
        l4 = disb_dt(isa0 + k3, isb0 + l3, psira0 + m3, psirb0 + n3, wk, w0 + o3, ab_volt.B) * dt;
        o4 = dv_dt(isa0 + k3, isb0 + l3, psira0 + m3, psirb0 + n3, wk, w0 + o3, const_speed) * dt;
        

        isa0 += (k1 + 2 * k2 + 2 * k3 + k4)/6;
        isb0 += (l1 + 2 * l2 + 2 * l3 + l4)/6;
        psira0 += (m1 + 2 * m2 + 2 * m3 + m4)/6;
        psirb0 += (n1 + 2 * n2 + 2 * n3 + n4)/6;
        w0 += (o1 + 2 * o2 + 2 * o3 + o4) * p / 6;

        

        ab_current.A = (1 / Lr) * (psira0 - Lm * isa0);
        ab_current.B = (1 / Lr) * (psirb0 - Lm * isb0);

        

        
        outf << t << "\t" << (Fk) << "\t" << current.A  << "\t" << rms(current.A, inv_in.f) << std::endl;
        t += dt;
        
    }

}