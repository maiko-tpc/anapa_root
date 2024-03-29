#include <math.h>

#include "nrutil.h"

#define R 0.61803399
#define C (1.0 - R)
#define SHFT2(a, b, c) (a)=(b); (b)=(c);
#define SHFT3(a, b, c, d) (a)=(b); (b)=(c); (c)=(d);

float golden_section(float ax, float bx, float cx, float (*f)(float, float[]), float tol, float *xmin, float *a){
  float f1, f2, x0, x1, x2, x3;
  x0 = ax;
  x3 = cx;
  if(fabs(cx - bx) > fabs(bx - ax)){
    x1 = bx;
    x2 = bx + C*(cx - bx);
  }else{
    x2 = bx;
    x1 = bx - C*(bx - ax);
  }
  f1 = (*f)(x1, a);
  f2 = (*f)(x2, a);
  while(fabs(x3 - x0) > tol*(fabs(x1) + fabs(x2))){
    if(f2 < f1){
      SHFT3(x0, x1, x2, R*x1 + C*x3)
	SHFT2(f1, f2, (*f)(x2, a))
    }else{
      SHFT3(x3, x2, x1, R*x2 + C*x0)
	SHFT2(f2, f1, (*f)(x1, a))
    }
  }
  if(f1 < f2){
    *xmin = x1;
    return f1;
  }else{
    *xmin = x2;
    return f2;
  }
}
