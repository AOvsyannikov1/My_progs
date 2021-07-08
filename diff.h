
#pragma once
#include <iostream>
										//Õ“¿ 1200			ƒÃ“F 012-06
#define rs          0.0298				//0.0298			
#define Lss         0.001345 			//0.001345			
#define rr          0.0255				//0.0255			
#define Lsr         0.001069			//0.001069		
#define Lm          0.044185			//0.044185			
#define p           3					//3					

#define pi          3.1415926
#define g			9.81
										//›œ10
#define gear_ratio  3.864				//3.864
#define kpd_ted     0.952				//0.952
#define kpd_red     0.98				//0.98
#define fric_coeff	0.01				//0.1

#define n_ted       6					//6
#define n_cars		5
#define J           (118.45 * n_ted)	//118.45
#define mass_el     (135000)			//135000
#define mass_car	(61500)				//61500
#define mass_train  (mass_car * n_cars)
#define mass        (mass_el + mass_train)
#define Db          1.25				//1.25

#define dt 0.00002

const double wk = 0;
const double Ls = Lss + Lm;
const double Lr = Lsr + Lm;
const double Lse = ((Ls * Lr - Lm * Lm) / Lr);

double v_to_w(double v);
double w_to_v(double w);
double osn_sopr_lok(double v);
unsigned char sign(double arg);
double dpsia_dt(double isa0, double isb0, double psira0, double psirb0, double wk, double w0);
double dpsib_dt(double isa0, double isb0, double psira0, double psirb0, double wk, double w0);
double disa_dt(double isa0, double isb0, double psira0, double psirb0, double wk, double w0, double Usa);
double disb_dt(double isa0, double isb0, double psira0, double psirb0, double wk, double w0, double Usb);
double dv_dt(double isa0, double isb0, double psira0, double psirb0, double wk, double w0, bool const_speed);
double osn_sopr_sost(double v);
void rk4(double t0, double tk);