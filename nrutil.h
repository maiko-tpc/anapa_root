#ifndef _NRUTIL_H
#define _NRUTIL_H

/* golden sention method */
/* ax, bx, cx : xrange which must contain local minimum point */
/* f : user defined function */
/* tol : acceptable cal error (float : ~10e-4, double : ~10e-07) */
/* xmin : x-value giving local minimum of f */
/* a[] : if function needs some parameter, you can specify with a[] */
float golden_section(float ax, float bx, float cx, float (*f)(float, float[]), float tol, float *xmin, float *a);

#endif

