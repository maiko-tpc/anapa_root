#ifndef _MYFUNC_H
#define _MYFUNC_H

//evaluate function for track fitting 
float evalfunc_tpc_fitting(float theta, float **a);
float evalfunc_tpc_fitting2(float theta, float *a);

//get min value & point by mesh calc
/* xmin, xmax : the range where min value will be calculated */
/* steps : tne numer of mesh size, steps for calc */
/* *f : the evaluete function */
/* x0 : the x-value where min is given */
/* a : the parameters of eval function */
float mesh_min(float xmin, float xmax, int steps, float (*f)(float, float *), float *x0, float *a);

//return flag by whether iev is TARGET evt
int evt_select(int iev);
#endif
