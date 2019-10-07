/** kinema.h ************/
/* 00/05/15 add new macro _KINE to avoid doublicate inclusion. */
/* 03/07/14 add new function calcp3lab. */

#ifndef _KINE
#define _KINE 1
#define AMU 931.50
#define MP 938.272
#define CV 299792458

int calccmang(double *cmang,double labang,double gamma,double delta);
double calclabk(double cmang_r,double gamma2,double p3c,double m3);
double calcfact(double cm_ang_r,double lab_ang_r,double gamma2,double delta);
double calcex4(double lab_ang_r,double m1,double m2,
                double m3,double m4,double K1,double K3);
double calcp3lab(double lab_ang_r,double m1,double m2,
				 double m3,double m4,double K1);
#endif

