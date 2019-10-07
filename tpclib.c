#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <memory.h>

/* babirl */
#include <bi-config.h>
#include <bi-common.h>
#include <ridf.h>

//#include "cfortran.h"
//#include "hbook.h"
#include "anapa.h"
#include "tpclib.h"


#define FADC_NOISE 1


/***************************************/
/* function library for MAIKo TPC



             2014/12/16 M.Murata       */
/***************************************/

/*
"init_p_2dim"
Initializing array of 2 d. points : p_2dim 
*/

int init_p_2dim(struct p_2dim *p){ //Intiallize p_2dim
  int i,j;
  for(i=0; i<MAX_POINT_COMP; i++){ //Number of point
    for(j=0; j<2; j++){ //X,Y cordinate
      p->points[i][j] = 0; //All points set at (0, 0)
    }
  }
  p->n_comp = 0; //Initialize number of component
  return 0;
}

int init_p_2dim_dyn(struct p_2dim_dyn *p, unsigned int hitcnt){ //Intiallize p_2dim
  int i,j;
  for(i=0; i<hitcnt; i++){ //Number of point
    for(j=0; j<2; j++){ //X,Y cordinate
      p->points[i][j] = 0; //All points set at (0, 0)
    }
  }
  p->n_comp = 0; //Initialize number of component
  return 0;
}

/*
"init_fadc_a_p_2dim"
Initializing array of array of 2 d. points for FADC : fadc_a_p_2dim
*/
int init_fadc_a_p_2dim(struct fadc_a_p_2dim *a_p){ //Intiallize fadc_a_p_2dim
  int i,j,k;

  for(i=0; i<N_AC; i++){      //i = 0 (anode), 1 (cathode)
    for(j=0; j<N_FADC; j++){  //j = 0 - 7 (ch of anode & cathode)

      for(k=0; k<N_ACLK; k++){ //K = 0 - 255 (clock of FADC)
 	a_p->p[i][j].points[k][0] = 0; //init. values of x-axis
  	a_p->p[i][j].points[k][1] = 0; //init. values of y-axis
      }

      a_p->p[i][j].n_comp = 0; //init. number of component
    }
  }
  return 0;
}

/*
"init_tdc_a_p_2dim"
Initializing array of array of 2 d. points for FADC : tdc_a_p_2dim
*/
int init_tdc_a_p_2dim(struct tdc_a_p_2dim *a_p){ //Intiallize tdc_a_p_2dim
  int i,k;

  for(i=0; i<N_AC; i++){      //i = 0 (anode), 1 (cathode)
      for(k=0; k<MAX_POINT_COMP; k++){ //K : (MAX # of point comp.)
 	a_p->p[i].points[k][0] = 0; //init. values of x-axis
  	a_p->p[i].points[k][1] = 0; //init. values of y-axis
      }
      a_p->p[i].n_comp = 0; //init. number of component
  }

  return 0;
}

/* for malloc version */
int init_tdc_a_p_2dim_dyn(struct tdc_a_p_2dim *a_p, unsigned int hitcnt){ 
  int i,k;

  for(i=0; i<N_AC; i++){      //i = 0 (anode), 1 (cathode)
      for(k=0; k<hitcnt; k++){ //K : (MAX # of point comp.)
 	a_p->p[i].points[k][0] = 0; //init. values of x-axis
  	a_p->p[i].points[k][1] = 0; //init. values of y-axis
      }
      a_p->p[i].n_comp = 0; //init. number of component
  }

  return 0;
}

/*
"init_area_2dim"
Initializing array of border lines defineing AREA : area_2dim
*/
int init_area_2dim(struct area_2dim *a){ //Intiallize area_2dim
  int i,j;

  for(i=0; i<MAX_LINE_COMP; i++){ //i : max number of border LINEs
      for(j=0; j<4; j++){ //j : number of parameters for 1 line
 	a->lines[i][j] = 0;
      }
      a->n_comp = 0; //init. number of component
  }

  return 0;
}

/*
"cp_p_2dim"
Copying array of 2 d. points : p_2dim 
 p -> p_cp
*/

int cp_p_2dim(struct p_2dim *p,
              struct p_2dim *p_cp){ //COPY of p_2dim "p"

  init_p_2dim(p_cp);

  int i,j;
  for(i=0; i<p->n_comp; i++){ //Number of point
    for(j=0; j<2; j++){ //X,Y cordinate
      p_cp->points[i][j] = p->points[i][j]; //All points in "p" are copied
    }
  }
  p_cp->n_comp = p->n_comp; //Number of components is also copied
  return 0;
}

int cp_p_2dim_dyn(struct p_2dim_dyn *p,
              struct p_2dim_dyn *p_cp){ //COPY of p_2dim "p"

  int i,j;
  for(i=0; i<p->n_comp; i++){ //Number of point
    for(j=0; j<2; j++){ //X,Y cordinate
      p_cp->points[i][j] = p->points[i][j]; //All points in "p" are copied
    }
  }
  p_cp->n_comp = p->n_comp; //Number of components is also copied
  return 0;
}

/*
"cp_area_2dim"
Copying array of border lines defineing AREA : area_2dim
*/
int cp_area_2dim(struct area_2dim *a,
                 struct area_2dim *a_cp){ //Intiallize area_2dim
  init_area_2dim(a_cp);
  int i,j;

  for(i=0; i<a->n_comp; i++){ //i : max number of border LINEs
      for(j=0; j<4; j++){ //j : number of parameters for 1 line
 	a_cp->lines[i][j] = a->lines[i][j];
      }
  }

  a_cp->n_comp = a->n_comp; //copy number of components
  return 0;
}


/*
"refill_fadc"
Refilling FADC values into xy-points format
*/

int refill_fadc(float fadc[N_AC][N_FADC][N_ACLK], //INPUT : FADC DATA
                struct fadc_a_p_2dim *a_p){ //OUTPUT : Array of points array
/*
int refill_fadc(float ***fadc, //INPUT : FADC DATA[N_AC][N_FADC][N_ACLK]
                struct fadc_a_p_2dim *a_p){ //OUTPUT : Array of points array
*/
  int i,j,k;

  //  printf("enter refill_fadc\n");

  for(i=0; i<N_AC; i++){      //i = 0 (anode), 1 (cathode)
    for(j=0; j<N_FADC; j++){  //j = 0 - 7 (ch of anode & cathode)
      for(k=0; k<N_ACLK; k++){ //K = 0 - 255 (clock of FADC)
	//	printf("loop\n");
	if(a_p->p[i][j].n_comp >=  MAX_POINT_COMP){
	  printf("Error in refill_fadc : too many points\n");
	  return 1;
	}

 	a_p->p[i][j].points[k][0] = k; //Put values of x-axis (FADC sampling clock)
       	a_p->p[i][j].points[k][1] = fadc[i][j][k]; //Put values of y-axis (FADC value)
	//       	a_p->p[i][j].points[k][1] = (*(*(*fadc))); //Put values of y-axis (FADC value)
	a_p->p[i][j].n_comp++; //Increment number of components each time

      }
    }
  }
  //  printf("REF : FADC C-4 100 : %f\n",fadc[1][4][100]);
  return 0;
}

/*
"refill_tdc"
Refilling TDC hit pattern into xy-points format
*/

int refill_tdc(unsigned char tdc[N_AC][N_TCLK][N_STRP], //INPUT : TDC DATA
                struct tdc_a_p_2dim *a_p){ //OUTPUT : Array of points array

  int i,j,k,l;
  l = 0;
  //  printf("enter refill_fadc\n");
  //  printf("refill_tdc : start\n");
  
  for(i=0; i<N_AC; i++){      //i = 0 (anode), 1 (cathode)
    l = 0;
    for(j=0; j<N_TCLK; j++){  //j = 0 - 1023 (clock of TDC)
      for(k=0; k<N_STRP; k++){ //K = 0 - 255 (strip of TDC)
	if(tdc[i][j][k] > 0){
	  
	  if(a_p->p[i].n_comp >=  MAX_POINT_COMP){
	    printf("Error in refill_tdc : too many points\n");
	    printf("@ A/C : %d\n",i);
	    return i+1;
	  }
	  //	printf("refill : A/C %d, x %d, y %d\n",i,k,j);
	  
	  a_p->p[i].points[l][0] = k; //Put values of x-axis (FADC sampling clock)
	  a_p->p[i].points[l][1] = j; //Put values of y-axis (FADC value)
	  //       	a_p->p[i][j].points[k][1] = (*(*(*fadc))); //Put values of y-axis (FADC value)
	  a_p->p[i].n_comp++; //Increment number of components each time
	  l++;
	}
	
      }
    }
  }
  
  //  printf("anode : %d points , cathode : %d points \n",
  //         a_p->p[0].n_comp, a_p->p[1].n_comp);
  //  printf("REF : FADC C-4 100 : %f\n",fadc[1][4][100]);

  //       	printf("refill_tdc : success\n");
  return 0;
}

int refill_tdc_dyn(unsigned char tdc[N_AC][N_TCLK][N_STRP], //INPUT : TDC DATA
		   struct tdc_a_p_2dim *a_p, //OUTPUT : Array of points array
		   unsigned int hitcnt){

  int i,j,k,l;
  l = 0;
  //  printf("enter refill_fadc\n");
  //  printf("refill_tdc : start\n");

  for(i=0; i<N_AC; i++){      //i = 0 (anode), 1 (cathode)
    l = 0;
    for(j=0; j<N_TCLK; j++){  //j = 0 - 1023 (clock of TDC)
      for(k=0; k<N_STRP; k++){ //K = 0 - 255 (strip of TDC)
	if(tdc[i][j][k] > 0){

	  if(a_p->p[i].n_comp >=  hitcnt){
	    printf("Error in refill_tdc : too many points\n");
	    printf("@ A/C : %d\n",i);
	    return i+1;
	  }
	  //	printf("refill : A/C %d, x %d, y %d\n",i,k,j);
	  
	  a_p->p[i].points[l][0] = k; //Put values of x-axis (FADC sampling clock)
	  a_p->p[i].points[l][1] = j; //Put values of y-axis (FADC value)
	  //       	a_p->p[i][j].points[k][1] = (*(*(*fadc))); //Put values of y-axis (FADC value)
	  a_p->p[i].n_comp++; //Increment number of components each time
	  l++;
	}
	
      }
    }
  }
  
  //  printf("anode : %d points , cathode : %d points \n",
  //         a_p->p[0].n_comp, a_p->p[1].n_comp);
  //  printf("REF : FADC C-4 100 : %f\n",fadc[1][4][100]);
  
  //       	printf("refill_tdc : success\n");
  return 0;
}

/*
  "add_p"
   ADDing Points for p_2dim

   Point is designated as
   (x, y)
   x = p_in[0], y= p_in[1]

   p_in[] is returned to be (0, 0),
   if correctly processed.
*/

int add_p(struct p_2dim *p, //2 d. points format
	  float *p_in){ //INPUT value : point p_in[2]

  int i;

  if(p->n_comp == MAX_POINT_COMP){
    printf("Error : No ROOM for new point\n");
    return 1;
  }

  for(i=0; i<2; i++){
   p->points[p->n_comp][i] = p_in[i];
  }

  p->n_comp++;

  for(i=0; i<2; i++){
    p_in[i] = 0;;
  }

  return 0;
}

/*
  "add_brd"
   ADDing BoRDer for area_2dim

   Border is designated as
   ax + by + c = 0
   a = l_para[0], b= l_para[1], c = l_para[2]
   Left Hand Side of this formula is Positive (if p_n > 0) or
                                     Negative (if p_n < 0)
   for the points in the area.
*/

int add_brd(struct area_2dim *a, //2 d. area format
	    double *l_para, //INPUT value : Parameter of bordr line 
	    int p_n){ //INPUT value : Parameter to define "INSIDE"
  int i;

  if(a->n_comp == MAX_LINE_COMP){
    printf("Error : No ROOM for new border\n");
    return 1;
  }

  for(i=0; i<3; i++){
   a->lines[a->n_comp][i] = l_para[i];
  }

  if(p_n > 0){
    a->lines[a->n_comp][3] = 1;
  }

  else if(p_n < 0){
    a->lines[a->n_comp][3] = -1;
  }

  else{
    a->lines[a->n_comp][3] = 0;
  }

  a->n_comp++;

  return 0;
}

/*
  "sumy_p"
   Calculating sum of y cordinate of"2_dim" point format
*/
double sumy_p(struct p_2dim *p){//2d. point format
  int i;
  double sum = 0;

  for(i=0;i<p->n_comp;i++){
    sum += p->points[i][1];
  }

  return sum;

}

/*
  "peaky_p"
   Searching peak of y cordinate in "2_dim" point format
   POLarity of the peak is designated with "pol"
   if pol > 0 Positive peak == greatest y
      pol < 0 Negative peak == smallest y
   Peak height is returned
*/

double peak_p(struct p_2dim *p, //2d. point format
	      int pol){ //Greatest (pol >= 0) or Smallest (pol < 0) value returned
  int i;
  double peak_h = 0;

  if(pol >= 0 ){
    for(i = 0; i<p->n_comp; i++){
      if(peak_h < p->points[i][1]){
	peak_h = p->points[i][1];
      }
    }
  }

  else if(pol < 0 ){
    for(i = 0; i<p->n_comp; i++){
      if(peak_h > p->points[i][1]){
	peak_h = p->points[i][1];
      }
    }
  }

  else{
    printf("bad polarity input");
    return 0;
  }
  return peak_h;

}

/*
  "mm_p"
  calculating mimimum and maximum of "p_2dim" 2d. points
  simplified version of stat_p
  INPUT  value : struct p_2dim p
  RETURN value : float min[2], float max[2] 
*/

int mm_p(struct p_2dim p, //2 d. points format
	 float *min, //RETURN value : 2 para. min. of point min[0] <= x, min[1] <= y
	 float *max){//RETURN value : 2 para. max. of point max[0] >= x, max[1] >= y

  int i;
  float max_x, max_y, min_x, min_y;

  max_x = 0;
  max_y = 0;

  min_x = 1024;
  min_y = 1024;

  if(p.n_comp == 0){
    printf("Error : No component ! No statistics can be defined.\n");
    return 1;
  }

  /* 1st loop for c.m. calculation */
  for(i=0; i<p.n_comp; i++){

    if(p.points[i][0] > max_x){
      max_x = p.points[i][0];
    }
    if(p.points[i][0] < min_x){
      min_x = p.points[i][0];
    }
    if(p.points[i][1] > max_y){
      max_y = p.points[i][1];
    }
    if(p.points[i][1] < min_y){
      min_y = p.points[i][1];
    }
  }

  printf("x = %.1f ~ %.1f, y= %.1f ~ %.1f\n",min_x,max_x,min_y,max_y);

  min[0] = min_x;
  min[1] = min_y;

  max[0] = max_x;
  max[1] = max_y;

  return 0;

}

/*
  "stat_p"
  calculating statistics of "p_2dim" 2d. points
  INPUT  value : struct p_2dim p
  RETURN value : double *cm[2], double *st_dv[2], double *crl_cef 
  cm[0]    =  sum{ x } / N , cm[1] = sum{ y } / N
  st_dv[0] =  sqrt[ sum{ (x - cm[0])^2 }/ N-1 ] ,...
  crl_cef  =  sum{(x - cm[0]) * (y - cm[1])} / (st_dv[0] * st_dv[1])
  N = p.n_comp (# of points)
*/
int stat_p(struct p_2dim p, //2 d. points format
	   float *min, //RETURN value : 2 para. min. of point min[0] <= x, min[1] <= y
	   float *max, //RETURN value : 2 para. max. of point max[0] >= x, max[1] >= y
	   double *cm, //RETURN value : 2 para. Center of Mass (x,y)=(cm[0],cm[1])
      	   double *st_dv, //RETURN value : 2 para. STandard DeViation for x & y
	   double *crl_cef){ //RETURN value : CoRreLation CoEFficient between x-y
  int i;
  float max_x, max_y, min_x, min_y;

  max_x = 0;
  max_y = 0;

  min_x = 1024;
  min_y = 1024;

  if(p.n_comp == 0){
    printf("Error : No component ! No statistics can be defined.\n");
    return 1;
  }

  /* 1st loop for c.m. calculation */
  for(i=0; i<p.n_comp; i++){
    cm[0] += p.points[i][0];
    cm[1] += p.points[i][1];
    if(p.points[i][0] > max_x){
      max_x = p.points[i][0];
    }
    if(p.points[i][0] < min_x){
      min_x = p.points[i][0];
    }
    if(p.points[i][1] > max_y){
      max_y = p.points[i][1];
    }
    if(p.points[i][1] < min_y){
      min_y = p.points[i][1];
    }
  }

  printf("x = %.1f ~ %.1f, y= %.1f ~ %.1f\n",min_x,max_x,min_y,max_y);

  min[0] = min_x;
  min[1] = min_y;

  max[0] = max_x;
  max[1] = max_y;

  /* Center of Mass */
  cm[0] /= p.n_comp;
  cm[1] /= p.n_comp;

  if(p.n_comp == 1){
    printf("Error : Only 1 component ! No deviation can be defined.\n");
    return 1;
  }

  /* 2nd loop for st_dv calculation */
  for(i=0; i<p.n_comp; i++){
    st_dv[0] += (p.points[i][0] - cm[0]) * (p.points[i][0] - cm[0]);
    st_dv[1] += (p.points[i][1] - cm[1]) * (p.points[i][1] - cm[1]);
  }
  /*
  st_dv[0] /= (p.n_comp);
  st_dv[1] /= (p.n_comp);
  */

    st_dv[0] /= (p.n_comp-1);
    st_dv[1] /= (p.n_comp-1);

  /* STandard DeViation */
  st_dv[0] = sqrt(st_dv[0]);
  st_dv[1] = sqrt(st_dv[1]);


  /* 3rd loop for crl_cef calculation */
  for(i=0; i<p.n_comp; i++){
    *crl_cef += (p.points[i][0] - cm[0]) * (p.points[i][1] - cm[1]);
  }

  /* CoReLation CoEFficient */
  *crl_cef /= (st_dv[0] * st_dv[1] * p.n_comp);


  //  crl_cef  =  sum{(x - cm[0]) * (y - cm[1])} / (st_dv[0] * st_dv[1])

  return 0;
}


/*
"fit_baseline"
fitting FADC 
*/

/*
  "fit_p_lin"
  fitting TDC hit pattern into linear function
  INPUT  value : int tdc_hit[][][], int ac
  RETURN value : double *fit_para, double *res
  ax + by + c = 0
  a = fit_para[0], b= fit_para[1], c = fit_para[2]
*/
int fit_p_lin(int tdc_hit[N_AC][N_STRP][N_TCLK], //TDC hit pattern
              int ac, //Anode(0) or Cathode(1)
              double *fit_para, //RETURN value : fit parameter //DAME
              double *res){ //RETURN value : residual of fitting
  int i, j, k;
  double sum_x, sum_y;
  double sum_xx, sum_xy;
  int n_point; //Number of data Points

  i = ac; //Specify Anode(0) or Cathode(1)
  if (i != 0 && i != 1){
    printf("BAD spacification\n");
    printf("Anode = 0, Cathode = 1\n");
    return 1; //Failure to fit because of bad input(A/C)
  }

  sum_x = 0;
  sum_y = 0;
  sum_xx = 0;
  sum_xy = 0;
  n_point = 0;

  /* Sampling TDC hit pattern */
  for(j = 0; j< N_STRP; j ++){
    for(k = 0; k<N_TCLK; k ++){

      /* Sum up position data when tdc HIT */
      if(tdc_hit[i][j][k] > 0){
	sum_x +=j;
	sum_y += k;
	sum_xx += j*j;
	sum_xy = j*k;
	n_point++;
      }

    }//loop : k (TDC_CLOCK) end
  }//loop : j (TDC_STRP) end

  fit_para[0] = n_point * sum_xy - sum_x * sum_y;
  fit_para[1] = sum_x * sum_x - n_point * sum_xx;
  fit_para[2] = sum_xx * sum_y - sum_xy * sum_x;
  for(j = 0; j< N_STRP; j ++){
    for(k = 0; k<N_TCLK; k ++){

      /* Caluculate residul when tdc HIT */
      if(tdc_hit[i][j][k] > 0){
	*res += pow(fit_para[0] *k + fit_para[1] * j + fit_para[2], 2.0);
      }

    }//loop : k (TDC_CLOCK) end
  }//loop : j (TDC_STRP) end


  *res = *res / n_point;

  return 0;

}


/*
  "fit_lin"
  fitting "p_2dim" 2d. points into linear function
  INPUT  value : struct p_2dim p
  RETURN value : double *fit_para, double *res
  ax + by + c = 0
  a = fit_para[0], b= fit_para[1], c = fit_para[2]
*/
int fit_lin(struct p_2dim p, //2d. points format (DEFINED in tpclib.h)
              double *fit_para, //RETURN value : fit parameter
              double *res){ //RETURN value : residual of fitting
  int i;
  double sum_x, sum_y;
  double sum_xx, sum_xy;
  int n_point; //Number of data Points

  //    i = 0; //tmp.
  /*
  i = ac; //Specify Anode(0) or Cathode(1)
  if (i != 0 && i != 1){
    printf("BAD spacification\n");
    printf("Anode = 0, Cathode = 1\n");
    return 1; //Failure to fit because of bad input(A/C)
  }
  */
  //    printf("enter fit_lin\n");
  sum_x = 0;
  sum_y = 0;
  sum_xx = 0;
  sum_xy = 0;
  n_point = 0;

  /* Caluculate values for fitting */
  for(i = 0; i < p.n_comp; i++){
    sum_x += p.points[i][0];
    sum_y += p.points[i][1];
    sum_xx += p.points[i][0] * p.points[i][0];
    sum_xy += p.points[i][0] * p.points[i][1];
    n_point++;
  }
    //    printf("sum loop end\n");

    //    *res = 0;
    //    printf("res ha ok\n");

  /******************* Legacy from fit_p_lin *********************/
  /* Sampling TDC hit pattern */
  //  for(j = 0; j< N_STRP; j ++){
  //    for(k = 0; k<N_TCLK; k ++){
  //      /* Sum up position data when tdc HIT */
  //      if(tdc_hit[i][j][k] > 0){
  //	sum_x +=j;
  //	sum_y += k;
  //	sum_xx += j*j;
  //	sum_xy = j*k;
  //	n_point++;
  //      }
  //    }//loop : k (TDC_CLOCK) end
  //  }//loop : j (TDC_STRP) end
  /******************* Legacy from fit_p_lin end *********************/

    //    printf("fit_para caluculation start\n");
    //    printf("sum_x = %f, sum_y = %f\n",sum_x,sum_y);
    //    printf("sum_xx = %f, sum_yy = %f\n",sum_xx,sum_xy);
    //    printf("n_point = %d\n",n_point);
                fit_para[0] = n_point * sum_xy - sum_x * sum_y;
	        fit_para[1] = sum_x * sum_x - n_point * sum_xx;
	        fit_para[2] = sum_xx * sum_y - sum_xy * sum_x;

		//    printf("fit_para determined\n");

    if(fit_para[1] != 0){
      //    printf("y = %f x + %f\n",-fit_para[0]/fit_para[1], -fit_para[2]/fit_para[1]);
    }
    else {
      //      printf("perpendicular to x-axis\n");
    }

    /* init. residual */
    *res =0;

    /* calc. reidual */
  for(i = 0; i < p.n_comp; i++){
     *res += fabs (fit_para[0] /fit_para[1] * p.points[i][0] + p.points[i][1] + fit_para[2] /fit_para[1] );
  }


  /******************* Legacy from fit_p_lin *********************/
  //  for(j = 0; j< N_STRP; j ++){
  //    for(k = 0; k<N_TCLK; k ++){
  //      /* Caluculate residul when tdc HIT */
  //      if(tdc_hit[i][j][k] > 0){
  //	*res += pow(*fit_para[0] * k  + *fit_para[1] * j + *fit_para[2], 2.0);
  //      }
  //    }//loop : k (TDC_CLOCK) end
  //  }//loop : j (TDC_STRP) end
  /******************* Legacy from fit_p_lin end *********************/

  //   printf("n_point = %d\n",n_point);

  *res = *res / n_point; //Deviding by number of points

  //  printf("residual = %f\n",*res);

  return 0;

}

/*
  "fit_con"
  fitting "p_2dim" 2d. points into constant value
  INPUT  value : struct p_2dim p
  RETURN value : double *fit_para, double *res
  y = const
  const = fit_para
*/
int fit_con(struct p_2dim p, //2d. points format (DEFINED in tpclib.h)
              double *fit_para, //RETURN value : fit parameter (fit_lin compatible)
              double *res){ //RETURN value : residual of fitting
  int i;
  double sum_y;
  int n_point = 0;

  //    i = 0; //tmp.
  /*
  i = ac; //Specify Anode(0) or Cathode(1)
  if (i != 0 && i != 1){
    printf("BAD spacification\n");
    printf("Anode = 0, Cathode = 1\n");
    return 1; //Failure to fit because of bad input(A/C)
  }
  */
  //    printf("enter fit_con\n");
    sum_y = 0;


  /* Caluculate values for fitting */
  for(i = 0; i < p.n_comp; i++){
    sum_y += p.points[i][1];
    n_point++;
  }
    //    printf("sum loop end\n");

    //    *res = 0;
    //    printf("res ha ok\n");

  /******************* Legacy from fit_p_lin *********************/
  /* Sampling TDC hit pattern */
  //  for(j = 0; j< N_STRP; j ++){
  //    for(k = 0; k<N_TCLK; k ++){
  //      /* Sum up position data when tdc HIT */
  //      if(tdc_hit[i][j][k] > 0){
  //	sum_x +=j;
  //	sum_y += k;
  //	sum_xx += j*j;
  //	sum_xy = j*k;
  //	n_point++;
  //      }
  //    }//loop : k (TDC_CLOCK) end
  //  }//loop : j (TDC_STRP) end
  /******************* Legacy from fit_p_lin end *********************/

    //    printf("fit_para caluculation start\n");
    //    printf("sum_x = %f, sum_y = %f\n",sum_x,sum_y);
    //    printf("sum_xx = %f, sum_yy = %f\n",sum_xx,sum_xy);
    //    printf("n_point = %d\n",n_point);
                 fit_para[0] = 0;
		 fit_para[1] = 1;
                 fit_para[2] = - sum_y / n_point;

		//    printf("fit_para determined\n");

		 //    printf("y = %f\n",-fit_para[2]);

    /* init. residual */
    *res =0;

    /* calc. residual */
  for(i = 0; i < p.n_comp; i++){
     *res += fabs ( - fit_para[2] - p.points[i][1]);
  }


  /******************* Legacy from fit_p_lin *********************/
  //  for(j = 0; j< N_STRP; j ++){
  //    for(k = 0; k<N_TCLK; k ++){
  //      /* Caluculate residul when tdc HIT */
  //      if(tdc_hit[i][j][k] > 0){
  //	*res += pow(*fit_para[0] * k  + *fit_para[1] * j + *fit_para[2], 2.0);
  //      }
  //    }//loop : k (TDC_CLOCK) end
  //  }//loop : j (TDC_STRP) end
  /******************* Legacy from fit_p_lin end *********************/

  //   printf("n_point = %d\n",n_point);

  *res = *res / n_point; //Deviding by number of points

  //  printf("residual = %f\n",*res);

  return 0;

}

/*
  "elim_p"
  Eliminating deviating values of p_2dim and refilling rest values
  If deviation in y-cordinate is LARGER than "lat" then ELIMINATED
  INPUT  value : struct p_2dim p
  RETURN value : double *l_para, double lat
  ax + by + c = 0
  a = l_para[0], b= l_para[1], c = l_para[2]
*/

int elim_p(struct p_2dim *p, //2d. points format
	   double *l_para, //INPUT : standard linear function for eliminating
	   double lat){    //INPUT : lattitude of preservation

  int i,j;

  //  printf("/nenter elim_p\n");
  /*
   Assuming linear function of format below
   l_para[0] * x + l_para[1] * y  + l_para[2] = 0
   This is equivalent to
   y = - l_para[0] / l_para[1] * x - l_para[2] /l_para[1]
   if l_para[1] != 0.
   If linear function is perpendicular to x-axis,
   residual should be evaluated another way.
   (deviation in X-cordinate ?)
  */

  if( l_para[1] == 0){
    printf("toriaezu ima ha zissou dekitenai\n");
        return 1;
  }

  double a,b;
  /*
    y = ax +b
  */
  a = - l_para[0]/l_para[1];
  b = -l_para[2]/l_para[1];

  double dvt; //DeViaTion from linear function
  int n_out; //Number of points should be eliminated
  n_out = 0;
  /*** Judgement ***/
  for (i = 0; i < p->n_comp; i++){
    dvt = fabs (p->points[i][1] - a * p->points[i][0] - b);
    /*
    If DeViaTion from lin. fun. > lat
    position is replaced to NULL value
    and elliminated next loop.
    */
    if (dvt > lat){
      p->points[i][1] = -9999; //effective NULL value
      p->points[i][0] = -9999; //effective NULL value
      n_out ++;
    }
  }

  //  printf("n_out = %d\n",n_out);


  int n_elm; //Number of elimination
  n_elm =0;
  /*** Elimination ***/
  for (i = 0; i < p->n_comp - n_out; i++){
    //If i th components should be eliminated,
    if (p->points[i][0] == -9999 && p->points[i][1] == -9999){
    //search for normal value in latter components
      for(j = i+1; j < p->n_comp; j++){
	//If j th component is normal
	if(p->points[j][0] != -9999 && p->points[j][1] != -9999){
	  //Swap j th < - > i th
	  p->points[i][0] = p->points[j][0];
	  p->points[i][1] = p->points[j][1];

	  p->points[j][0] = -9999;
	  p->points[j][1] = -9999;

	  n_elm++;

	  break; //Break from normal value search loop : j
	}//Swap
      }//Search loop
  }//If should be eliminated value

    /*
  if(n_elm == n_out){ //If replacement finished
    //Init. rest NULL values in latter components
    for(j = i+1; j < p->n_comp; j++)
      p->points[j][0] = 0;
      p->points[j][1] = 0
      }
    break; //Break from components loop : i
    }
    */
  }//Loop around all the components of p_2dim

  p->n_comp = p->n_comp - n_out;
  //  printf("n_elm = %d\n",n_elm);
  //  printf("p->n_comp = %d\n",p->n_comp);
  return 0;
}

/*
  "pick_p_o"
  Picking up subset of p_2dim.
  target components is designeted with Order.
  "p_sub" consists of "cp_ll th" to "cp_ul th" components of "p"
    !!! cp_ll & cp_ul are ZERO-BASED (zero origin) !!! 
  INPUT  value : struct p_2dim *p, int cp_ll, int cp_ul
  RETURN value : struct p_2dim *p_sub
*/

int pick_p_o(struct p_2dim *p, //2d. points format
	   struct p_2dim *p_sub, //RETURN : subset of p
	   int cp_ll,     //INPUT : lower limit of subset
           int cp_ul){    //INPUT : upper limit of subset

  int i,j;

  //  printf("enter pick_p_o\n");
  /*
  Designated cp_ll should be ... larger than 0, smaller than cp_ul and p->n_comp
  Designated cp_ul should be ... (larger than 0), smaller than p->n_comp
  else BAD INPUT
  */
  if(cp_ll < 0 || cp_ul < cp_ll || cp_ul > p->n_comp || cp_ll > p->n_comp){
    printf("bad input\n");
    return 1;
  }

  //  printf("pick_p_o : input check done\n");
    init_p_2dim(p_sub);
    //    printf("pick_p_o : init. p_sub done\n");

  j = 0;
    p_sub->n_comp = 0;
  for(i = cp_ll; i < cp_ul; i++){
        p_sub->points[j][0] = p->points[i][0];
        p_sub->points[j][1] = p->points[i][1];
     p_sub->n_comp++;
    j++;
    //  printf("looooop");
  }

  //  printf("p_sub->n_comp = %d", p_sub->n_comp);
  return 0;
}


/*
  "pick_p_a"
  Picking up subset of p_2dim.
  target components is designeted with Area ( area_2dim ).
  "p_sub" consists of the components of "p" INSIDE  (if i_o > 0) or
                                            OUTSIDE (if i_o < 0) of "area_2dim."
  INPUT  value : struct p_2dim *p, struct area_2dim *a, int i_o
  RETURN value : struct p_2dim *p_sub
*/

int pick_p_a(struct p_2dim *p, //2d. points format
             struct area_2dim *a, //2d. area format
	     struct p_2dim *p_sub, //RETURN : subset of p
	     int i_o){    //INPUT : Inside or Outside of the area

  int i,j,k;

  int s_flag; //flag for selection

  s_flag = 0;

  //  printf("enter pick_p_o\n");
  /*
  Designated cp_ll should be ... larger than 0, smaller than cp_ul and p->n_comp
  Designated cp_ul should be ... (larger than 0), smaller than p->n_comp
  else BAD INPUT

  if(cp_ll < 0 || cp_ul < cp_ll || cp_ul > p->n_comp || cp_ll > p->n_comp){
    printf("bad input\n");
    return 1;
  }
  */
    init_p_2dim(p_sub);
//    printf("pick_p_o : init. p_sub done\n");


    k = 0; //"pointer" of points in p_sub

  for(i=0; i<p->n_comp; i++){ //Loop for points in struct "p"

    s_flag = 0; //init s_flag

    if(i_o > 0){ //Points "Inside" the area is designated.
       for(j=0;j<a->n_comp;j++){ //Loop for border lines in struct "a"

	   if(a->lines[j][3] > 0){
	       /*
                points(x,y)
                a->lines[j][0] * x + a->lines[j][1] * y + a->lines[j][2] > 0
                should be selected
	       */
	       if(a->lines[j][0] * p->points[i][0] +
                  a->lines[j][1] * p->points[i][1] + a->lines[j][2] >= 0){
         	         s_flag = 1; //Clear the "j" th border line
	       }
	       else{
	         s_flag = 0; //NOT clear "j" th border line
	         break; //break from loop for border lines
	       }
	   }

	   else if(a->lines[j][3] < 0){
	       /*
                points(x,y)
                a->lines[j][0] * x + a->lines[j][1] * y + a->lines[j][2] < 0
                should be selected
	       */
	       if(a->lines[j][0] * p->points[i][0] +
                  a->lines[j][1] * p->points[i][1] + a->lines[j][2] <= 0){
         	         s_flag = 1; //Clear the "j" th border line
	       }
	       else{
	         s_flag = 0;
	         break;
	       }
	     }
	   else{ //Nothing to do if a->lines[j][3] = 0
	       s_flag = 1;
	   }
         }//Loop for border lines in struct "a" end

        if(s_flag){ //If clear all the border lines
	  p_sub->points[k][0] = p->points[i][0];
	  p_sub->points[k][1] = p->points[i][1];
	  p_sub->n_comp++;

	  printf("%d th comp. %f %f\n",p_sub->n_comp,p->points[i][0],p->points[i][1]);
	  k++;
        }

      }//Selecting INSIDE region of the area "a"

    else if(i_o < 0){ //Points "Outside" the area is designeted.
       for(j=0;j<a->n_comp;j++){

	  if(a->lines[j][3] > 0){
	      /*
               points(x,y)
               a->lines[j][0] * x + a->lines[j][1] * y + a->lines[j][2] "<" 0
               should be selected
	      */
	      if(a->lines[j][0] * p->points[i][0] +
                 a->lines[j][1] * p->points[i][1] + a->lines[j][2] <= 0){
         	        s_flag = 1; //Clear the "j" th border line
	      }
	      else{
	        s_flag = 0; //NOT clear "j" th border line
	        break; //break from loop for border lines
	      }
	  }

	  else if(a->lines[j][3] > 0){
	      /*
               points(x,y)
               a->lines[j][0] * x + a->lines[j][1] * y + a->lines[j][2] ">" 0
               should be selected
	      */
	      if(a->lines[j][0] * p->points[i][0] +
                 a->lines[j][1] * p->points[i][1] + a->lines[j][2] >= 0){
         	        s_flag = 1; //Clear the "j" th border line
	      }
	      else{
	        s_flag = 0;
	        break;
	      }
	    }
	  else{ //Nothing to do if a->lines[j][3] = 0
	      s_flag = 1;
	  }
        }
        if(s_flag){ //If clear all the border lines
	  p_sub->points[k][0] = p->points[i][0];
	  p_sub->points[k][1] = p->points[i][1];
	  p_sub->n_comp++;
	  printf("%d th comp. %f %f\n",p_sub->n_comp,p->points[i][0],p->points[i][1]);
	  k++;
        }
    }//Selecting OUTSIDE region of the area "a"

    else{ //Nothing to do if i_o = 0
    }

  }//Loop for points in struct "p" end


  //  printf("p_sub->n_comp = %d", p_sub->n_comp);
  return 0;
}


/*
  "mhit_x"
  Judging whether p_2dim consists of multi-hit in the same "x" or not.
  If some multi hits exist, returned "int" is number of multi-hits.

  INPUT  value : struct p_2dim *p
  RETURN value : "int", struct p_2dim *p_sub
*/
/*
int mhit_x(struct p_2dim *p,
           double *b_para){

  float tmp_p[N_MULT][2];
  int i;
  for(i=0;i<p->n_comp;i++){


  }


}
*/


/* added 2015/01/04 by cotemba 
      return the baseline of FADC 
      repeat fitting up to 10 times
      
      - First input is array of each FADC data. 
      - Second input is 0(anode) or 1(cathode)
      - Third input is 0--7 (FADC ch number)
      - Fourth input is debug level
      
      example of usage: 
      "base[1][3] = getfadc_bl(a_p.p[1][3], 1, 3, 1); "
*/
float get_fadc_bl(struct p_2dim p, int iac, int ifadc, int DBG_LEVEL){
  double fit_para[3], fit_para_sub_end[3];
  double res, res_sub_end;
  int i;
  int good_result;
  float fadc_bl;
  int n_try;

  /* Initialization of parameters */
  fadc_bl=0;
  good_result=0;
  n_try=0;

  for(i=0; i<3; i++){
    fit_para[i]=0;
    fit_para_sub_end[i]=0;
  }
  
  struct p_2dim p_sub; //Temporal buffer for FADC 0 - 25 th clock                      
  struct p_2dim p_sub_end; //Temporal buffer for FADC 230 - 255 th clock

  init_p_2dim(&p_sub); //Init. p_sub               
  init_p_2dim(&p_sub_end); //Init. p_sub

  //  pick_p_o(&(p.points), &p_sub, 0, 25); 
  //  pick_p_o(&(p.points), &p_sub_end, 230, 255);
  pick_p_o(&p, &p_sub, 0, 25); 
  pick_p_o(&p, &p_sub_end, 230, 255);
  
  fit_con(p_sub, fit_para, &res);
  fit_con(p_sub_end, fit_para_sub_end, &res_sub_end);

  if(DBG_LEVEL >= 3){
    printf("%d:%d front ave= %f \n", iac, ifadc, fabs(fit_para[2]));
    printf("%d:%d back  ave= %f \n", iac, ifadc, fabs(fit_para_sub_end[2]));
  }

  if(fabs(fit_para[2]-fit_para_sub_end[2]) < FADC_NOISE){
    good_result=1;
    fadc_bl = fabs(fit_para[2] + fit_para_sub_end[2])/2.0;

    if(DBG_LEVEL >= 3){
      printf("%d:%d FADC baseline from first calc= %f \n", iac, ifadc, fadc_bl);
    }

  }
  
  if(good_result==0){
    //    elim_p(&(p.points), fit_para, 2*res); //Eliminating more-than-2-residual-deviating points
    elim_p(&p, fit_para, 2*res); //Eliminating more-than-2-residual-deviating points
    

    /* Fit the data to a*x+b*y+c=0
       Derive the baseline from y-value of 128ch 
       Repeat up to 10 times
    */
    while(1){
      n_try++;
      if(res < FADC_NOISE){
	fadc_bl = -1.0*fit_para[0]/fit_para[1] * N_ACLK / 2.0 - fit_para[2] /fit_para[1];

	if(DBG_LEVEL >= 2){
	  printf("%d:%d Finished at loop %d; baseline= %f \n", iac, ifadc, n_try, fadc_bl);
	}
	break;
      }

      if(n_try>10){
	if(DBG_LEVEL >= 1){
	  printf("%d:%d  Baseline did not convergent after trying 10 times!", iac, ifadc);
	  fadc_bl = -100;
	  break;
	}
      }

      fit_lin(p, fit_para, &res);
      elim_p(&p, fit_para, res); //Eliminating deviating points
      if(DBG_LEVEL >= 3){
	printf("%d:%d loop=%d, res=%f \n", iac, ifadc, n_try, res);
	printf("para[0]=%f, para[1]=%f, para[2]=%f \n", fit_para[0], fit_para[1], fit_para[2]);
      }
    }

  }
  return fadc_bl;
}

int msk_tpc(unsigned char tmp_tpc[N_AC][N_TCLK][N_STRP], 
	    float re_fadc[N_AC][N_FADC][N_ACLK], 
	    int fadc_th, 
	    int msk[N_AC][N_FADC][N_ACLK],
	    unsigned char tpc2[N_AC][N_TCLK][N_STRP]){

  int i,j,k,l,m;

  for(i=0; i<N_AC; i++){
    for(j=0; j<N_FADC; j++){
      for(k=0; k<N_ACLK; k++){

	/* compare FADC to threshold and make mask */
	if(re_fadc[i][j][k] > FADC_TH){
	  msk[i][j][k]=1;
	}
      }
    }
  }

  /* coincidence of input TPC data & mask */
  for(i=0; i<N_AC; i++){
    for(j=0; j<N_FADC; j++){
      for(k=0; k<N_ACLK; k++){

	for(l=0; l<4; l++){
	  for(m=0; m<32; m++){
	    if(msk[i][j][k]==1 && tmp_tpc[i][k*4+l][j*32+m]==1){
	      tpc2[i][k*4+l][j*32+m]=1;
	    }
	  }
	}

      }
    }
  }


  return 0;
}


int get_tpcinfo(struct tdc_a_p_2dim *a_p,
		int tpc_nhit[N_AC],
		float tpc_min[N_AC][2],
		float tpc_max[N_AC][2],
		float tpc_wid[N_AC][2],
		int DBG_LEVEL){
  

  int i,j,n;
  int tpc_hitflag[N_AC][N_STRP];
  int nhit[N_AC];
  int x_i;
  //int y_i;
  float x_f, y_f;
  float min_x[N_AC], min_y[N_AC], max_x[N_AC], max_y[N_AC];
  int tmp_wid_x[N_AC], max_wid_x[N_AC];
  

  for(i=0; i<N_AC; i++){
    min_x[i]=10000;
    min_y[i]=10000;
    max_x[i]=-1;
    max_y[i]=-1;
    tmp_wid_x[i]=0;
    max_wid_x[i]=0;
    nhit[i]=0;

    for(j=0; j<N_STRP; j++){
      tpc_hitflag[i][j]=0;
    }
  }

  for(i=0; i<N_AC; i++){
    for(n=0; n<a_p->p[i].n_comp; n++){
      x_f = a_p->p[i].points[n][0];
      y_f = a_p->p[i].points[n][1];
      x_i = (int)x_f;


      if(tpc_hitflag[i][x_i]!=1){
	nhit[i]++;
	tpc_hitflag[i][x_i]=1;
      }

      if(x_f < min_x[i]) min_x[i]=x_f;
      if(y_f < min_y[i]) min_y[i]=y_f;

      if(x_f > max_x[i]) max_x[i]=x_f;
      if(y_f > max_y[i]) max_y[i]=y_f;
    }
  }

  for(i=0; i<N_AC; i++){
    tpc_min[i][0] = min_x[i];
    tpc_min[i][1] = min_y[i];
    tpc_max[i][0] = max_x[i];
    tpc_max[i][1] = max_y[i];
    tpc_wid[i][0] = max_x[i] - min_x[i];
    tpc_wid[i][1] = max_y[i] - min_y[i];
    tpc_nhit[i] = nhit[i];

  }


  if(DBG_LEVEL>=1){
    printf("---------------------------------------------------------\n");
    printf("Anode: min_x= %f, max_x= %f | min_y= %f, max_y= %f \n", 
	 min_x[0], max_x[0], min_y[0], max_y[0]);
    printf("       nhit=%d \n", nhit[0]);
    printf(" \n");
    printf("Cathode: min_x= %f, max_x= %f | min_y= %f, max_y= %f \n", 
	   min_x[1], max_x[1], min_y[1], max_y[1]);
    printf("         nhit=%d \n", nhit[1]);
    printf("---------------------------------------------------------\n");
  }

  if(DBG_LEVEL>=2){
    printf("*********************************************************\n");
    printf("Anode hit flag... \n");
    for(i=0; i<N_STRP; i++){
      printf("strip %d: %d \n", i, tpc_hitflag[0][i]);
    }
    printf(" \n");
    
    printf("Cathode hit flag... \n");
    for(i=0; i<N_STRP; i++){
      printf("strip %d: %d \n", i, tpc_hitflag[1][i]);
    }
    printf("*********************************************************\n");
  }
  return 0;
}


int find_hough_max(unsigned int hough_cnt[N_AC][DIV_HOUGH_X][DIV_HOUGH_Y],
		   float win_x1[N_AC], float win_x2[N_AC],
		   float win_y1[N_AC], float win_y2[N_AC], 
		   int win_typ[N_AC],
		   int max_x[N_AC][N_TRY], int max_y[N_AC][N_TRY], int max_cnt[N_AC][N_TRY],
		   float slope[N_AC][N_TRY], float offset[N_AC][N_TRY],		   
		   int _try){

  int i,j,k;
  int tmp_max_x[N_AC], tmp_max_y[N_AC], tmp_max_cnt[N_AC];
  int wini_x1[N_AC], wini_x2[N_AC], wini_y1[N_AC], wini_y2[N_AC];
  
  for(i=0; i<N_AC; i++){
    tmp_max_x[i]=-10;
    tmp_max_y[i]=-10;
    tmp_max_cnt[i]=0;
    wini_x1[i] = win_x1[i]/3.14*DIV_HOUGH_X;
    wini_x2[i] = win_x2[i]/3.14*DIV_HOUGH_X;
    wini_y1[i] = win_y1[i]/1024.0*DIV_HOUGH_Y;
    wini_y2[i] = win_y2[i]/1024.0*DIV_HOUGH_Y;

    /* Make sure that window is well defined */
    if(win_typ[i] > 8) win_typ[i]=0;
    if(wini_x1[i] > wini_x2[i]) win_typ[i]=0;
    if(wini_y1[i] > wini_y2[i]) win_typ[i]=0;
  }

    for(i=0; i<N_AC; i++){
      if(win_typ[i] == 0){
	for(j=0; j<DIV_HOUGH_X; j++){
	  for(k=0; k<DIV_HOUGH_Y; k++){
	    if(hough_cnt[i][j][k] > tmp_max_cnt[i]){
	      tmp_max_x[i] = j;
	      tmp_max_y[i] = k;
	      tmp_max_cnt[i] = hough_cnt[i][j][k];
	    }
	  }
	}
      } // win_typ == 0

      if(win_typ[i] == 1){
	for(j=wini_x1[i]; j<wini_x2[i]; j++){
	  for(k=0; k<DIV_HOUGH_Y; k++){
	    if(hough_cnt[i][j][k] > tmp_max_cnt[i]){
	      tmp_max_x[i] = j;
	      tmp_max_y[i] = k;
	      tmp_max_cnt[i] = hough_cnt[i][j][k];
	    }
	  }
	}
      } // win_typ == 1

      if(win_typ[i] == 2){
	for(j=0; j<DIV_HOUGH_X; j++){
	  if(j<wini_x1[i] || j>wini_x2[i]){
	    for(k=0; k<DIV_HOUGH_Y; k++){
	      if(hough_cnt[i][j][k] > tmp_max_cnt[i]){
		tmp_max_x[i] = j;
		tmp_max_y[i] = k;
		tmp_max_cnt[i] = hough_cnt[i][j][k];
	      }
	    }
	  }
	}
      } // win_typ == 2
      
      if(win_typ[i] == 3){
	for(j=0; j<DIV_HOUGH_X; j++){
	  for(k=wini_y1[i]; k<wini_y2[i]; k++){
	    if(hough_cnt[i][j][k] > tmp_max_cnt[i]){
	      tmp_max_x[i] = j;
	      tmp_max_y[i] = k;
	      tmp_max_cnt[i] = hough_cnt[i][j][k];
	    }
	  }
	}
      } // win_typ == 3
      
      if(win_typ[i] == 4){
	for(j=0; j<DIV_HOUGH_X; j++){
	  for(k=0; k<DIV_HOUGH_Y; k++){
	    if(k<wini_y1[i] || k>wini_y2[i]){
	      if(hough_cnt[i][j][k] > tmp_max_cnt[i]){
		tmp_max_x[i] = j;
		tmp_max_y[i] = k;
	      tmp_max_cnt[i] = hough_cnt[i][j][k];
	      }
	    }
	  }
	}
      } // win_typ == 4

      if(win_typ[i] == 5){
	for(j=wini_x1[i]; j<wini_x2[i]; j++){
	  for(k=wini_y1[i]; k<wini_y2[i]; k++){
	    if(hough_cnt[i][j][k] > tmp_max_cnt[i]){
	      tmp_max_x[i] = j;
	      tmp_max_y[i] = k;
	      tmp_max_cnt[i] = hough_cnt[i][j][k];
	    }
	  }
	}
      } // win_typ == 5

      if(win_typ[i] == 6){
	for(j=0; j<DIV_HOUGH_X; j++){
	  if(j<wini_x1[i] || j>wini_x2[i]){
	    for(k=wini_y1[i]; k<wini_y2[i]; k++){
	      if(hough_cnt[i][j][k] > tmp_max_cnt[i]){
		tmp_max_x[i] = j;
		tmp_max_y[i] = k;
		tmp_max_cnt[i] = hough_cnt[i][j][k];
	      }
	    }
	  }
	}
      } // win_typ == 6

      if(win_typ[i] == 7){
	for(j=0; j<DIV_HOUGH_X; j++){
	  for(k=wini_y1[i]; k<wini_y2[i]; k++){
	    if(k<wini_y1[i] || k>wini_y2[i]){
	      if(hough_cnt[i][j][k] > tmp_max_cnt[i]){
		tmp_max_x[i] = j;
		tmp_max_y[i] = k;
		tmp_max_cnt[i] = hough_cnt[i][j][k];
	      }
	    }
	  }
	}
      } // win_typ == 7
      
      if(win_typ[i] == 8){
	for(j=0; j<DIV_HOUGH_X; j++){
	  for(k=wini_y1[i]; k<wini_y2[i]; k++){
	    if((j<wini_x1[i] || j>wini_x2[i]) && (k<wini_y1[i] || k>wini_y2[i])){
	      if(hough_cnt[i][j][k] > tmp_max_cnt[i]){
		tmp_max_x[i] = j;
		tmp_max_y[i] = k;
		tmp_max_cnt[i] = hough_cnt[i][j][k];
	      }
	    }
	  }
	}
      } // win_typ == 8


    } //loop for i from 0 to N_AC




  for(i=0; i<N_AC; i++){
    max_x[i][_try] = tmp_max_x[i];
    max_y[i][_try] = tmp_max_y[i];
    max_cnt[i][_try] = tmp_max_cnt[i];

    /* Calcurate parameters of the straight line */
    slope[i][_try]  = -1.0/tan((float)tmp_max_x[i]/DIV_HOUGH_X*2*PI);
    offset[i][_try] = (float)tmp_max_y[i]/DIV_HOUGH_Y*1024.0
      /sin((float)tmp_max_x[i]/DIV_HOUGH_X*2*PI);
  }
  return 0;
}


int find_hough_max_LR(unsigned int hough_cnt[DIV_HOUGH_X][DIV_HOUGH_Y], //INPUT: Hough space
		      float win_x1, float win_x2, //INPUT: window para for theta
		      float win_y1, float win_y2, //INPUT: window para for r
		      int win_typ, //INPUT: window type --> see Log Book
		      int veto_wx,
		      int veto_wy,
		      int veto_flag,
		      int max_x[N_TRY], //OUTPUT: theta of the maximum content
		      int max_y[N_TRY], //OUTPUT: r of the maximum content
		      int max_cnt[N_TRY], //OUTPUT: maximum content
		      float slope[N_TRY], //OUTPUT: slope para of the line (y=a*x+b)
		      float offset[N_TRY], //OUTPUT: offset parameter of the line
		      int _try){ //INPUT: nankaime no Hough search

  int j,k,n;
  int tmp_max_x, tmp_max_y, tmp_max_cnt;
  int wini_x1, wini_x2, wini_y1, wini_y2;
  int in_veto;
  
  tmp_max_x=-10;
  tmp_max_y=-10;
  tmp_max_cnt=0;

  wini_x1 = win_x1/3.14*DIV_HOUGH_X;
  wini_x2 = win_x2/3.14*DIV_HOUGH_X;
  wini_y1 = win_y1/1024.0*DIV_HOUGH_Y;
  wini_y2 = win_y2/1024.0*DIV_HOUGH_Y;
  
  /* Make sure that window is well defined */
  if(win_typ > 8) win_typ=0;
  if(wini_x1 > wini_x2) win_typ=0;
  if(wini_y1 > wini_y2) win_typ=0;
  

  if(win_typ == 0){
    for(j=1; j<DIV_HOUGH_X; j++){
      for(k=1; k<DIV_HOUGH_Y; k++){

	/* Check if the point on Hough space is out of veto regions */
	/* in_veto==1 --> inside the veto regions(rejected) */
	in_veto=0;
	if(veto_flag==1 && _try>0){
	  for(n=1; n<_try+1; n++){
	    if((j>(max_x[n-1]-veto_wx) && j<(max_x[n-1]+veto_wx)) &&
	       (k>(max_y[n-1]-veto_wy) && k<(max_y[n-1]+veto_wy))){
	      in_veto=1;
	    }
	    if(in_veto==1) break;
	  }
	}
	
	if((hough_cnt[j][k] > tmp_max_cnt) && in_veto==0){
	  tmp_max_x = j;
	  tmp_max_y = k;
	  tmp_max_cnt = hough_cnt[j][k];
	}
      }
    }
  } // win_typ == 0
  


  if(win_typ == 1){
    for(j=wini_x1; j<wini_x2; j++){
      for(k=0; k<DIV_HOUGH_Y; k++){

	/* Check if the point on Hough space is out of veto regions */
	/* in_veto==1 --> inside the veto regions(regected) */
	in_veto=0;
	if(veto_flag==1 && _try>0){
	  for(n=1; n<_try+1; n++){
	    if((j>(max_x[n-1]-veto_wx) && j<(max_x[n-1]+veto_wx)) &&
	       (k>(max_y[n-1]-veto_wy) && k<(max_y[n-1]+veto_wy))){
	      in_veto=1;
	    }
	    if(in_veto==1) break;
	  }
	}

	if(hough_cnt[j][k] > tmp_max_cnt && in_veto==0){
	  tmp_max_x = j;
	  tmp_max_y = k;
	  tmp_max_cnt = hough_cnt[j][k];
	}
      }
    }
  } // win_typ == 1
  
  if(win_typ == 2){
    for(j=1; j<DIV_HOUGH_X; j++){
      if(j<wini_x1 || j>wini_x2){
	for(k=0; k<DIV_HOUGH_Y; k++){

	/* Check if the point on Hough space is out of veto regions */
	/* in_veto==1 --> inside the veto regions(regected) */
	in_veto=0;
	if(veto_flag==1 && _try>0){
	  for(n=1; n<_try+1; n++){
	    if((j>(max_x[n-1]-veto_wx) && j<(max_x[n-1]+veto_wx)) &&
	       (k>(max_y[n-1]-veto_wy) && k<(max_y[n-1]+veto_wy))){
	      in_veto=1;
	    }
	    if(in_veto==1) break;
	  }
	}

	  if(hough_cnt[j][k] > tmp_max_cnt && in_veto==0){
	    tmp_max_x = j;
	    tmp_max_y = k;
	    tmp_max_cnt = hough_cnt[j][k];
	  }
	}
      }
    }
  } // win_typ == 2
  
  if(win_typ == 3){
    for(j=1; j<DIV_HOUGH_X; j++){
      for(k=wini_y1; k<wini_y2; k++){

	/* Check if the point on Hough space is out of veto regions */
	/* in_veto==1 --> inside the veto regions(regected) */
	in_veto=0;
	if(veto_flag==1 && _try>0){
	  for(n=1; n<_try+1; n++){
	    if((j>(max_x[n-1]-veto_wx) && j<(max_x[n-1]+veto_wx)) &&
	       (k>(max_y[n-1]-veto_wy) && k<(max_y[n-1]+veto_wy))){
	      in_veto=1;
	    }
	    if(in_veto==1) break;
	  }
	}

	if(hough_cnt[j][k] > tmp_max_cnt && in_veto==0){
	  tmp_max_x = j;
	  tmp_max_y = k;
	  tmp_max_cnt = hough_cnt[j][k];
	}
      }
    }
  } // win_typ == 3
  
  if(win_typ == 4){
    for(j=1; j<DIV_HOUGH_X; j++){
      for(k=0; k<DIV_HOUGH_Y; k++){
	if(k<wini_y1 || k>wini_y2){

	/* Check if the point on Hough space is out of veto regions */
	/* in_veto==1 --> inside the veto regions(regected) */
	in_veto=0;
	if(veto_flag==1 && _try>0){
	  for(n=1; n<_try+1; n++){
	    if((j>(max_x[n-1]-veto_wx) && j<(max_x[n-1]+veto_wx)) &&
	       (k>(max_y[n-1]-veto_wy) && k<(max_y[n-1]+veto_wy))){
	      in_veto=1;
	    }
	    if(in_veto==1) break;
	  }
	}

	  if(hough_cnt[j][k] > tmp_max_cnt && in_veto==0){
	    tmp_max_x = j;
	    tmp_max_y = k;
	    tmp_max_cnt = hough_cnt[j][k];
	  }
	}
      }
    }
  } // win_typ == 4
  
  if(win_typ == 5){
    for(j=wini_x1; j<wini_x2; j++){
      for(k=wini_y1; k<wini_y2; k++){

	/* Check if the point on Hough space is out of veto regions */
	/* in_veto==1 --> inside the veto regions(regected) */
	in_veto=0;
	if(veto_flag==1 && _try>0){
	  for(n=1; n<_try+1; n++){
	    if((j>(max_x[n-1]-veto_wx) && j<(max_x[n-1]+veto_wx)) &&
	       (k>(max_y[n-1]-veto_wy) && k<(max_y[n-1]+veto_wy))){
	      in_veto=1;
	    }
	    if(in_veto==1) break;
	  }
	}

	if(hough_cnt[j][k] > tmp_max_cnt && in_veto==0){
	  tmp_max_x = j;
	  tmp_max_y = k;
	  tmp_max_cnt = hough_cnt[j][k];
	}
      }
    }
  } // win_typ == 5
  
  if(win_typ == 6){
    for(j=1; j<DIV_HOUGH_X; j++){
      if(j<wini_x1 || j>wini_x2){
	for(k=wini_y1; k<wini_y2; k++){

	/* Check if the point on Hough space is out of veto regions */
	/* in_veto==1 --> inside the veto regions(regected) */
	in_veto=0;
	if(veto_flag==1 && _try>0){
	  for(n=1; n<_try+1; n++){
	    if((j>(max_x[n-1]-veto_wx) && j<(max_x[n-1]+veto_wx)) &&
	       (k>(max_y[n-1]-veto_wy) && k<(max_y[n-1]+veto_wy))){
	      in_veto=1;
	    }
	    if(in_veto==1) break;
	  }
	}

	  if(hough_cnt[j][k] > tmp_max_cnt && in_veto==0){
	    tmp_max_x = j;
	    tmp_max_y = k;
	    tmp_max_cnt = hough_cnt[j][k];
	  }
	}
      }
    }
  } // win_typ == 6
  
  if(win_typ == 7){
    for(j=1; j<DIV_HOUGH_X; j++){
      for(k=wini_y1; k<wini_y2; k++){
	if(k<wini_y1 || k>wini_y2){

	/* Check if the point on Hough space is out of veto regions */
	/* in_veto==1 --> inside the veto regions(regected) */
	in_veto=0;
	if(veto_flag==1 && _try>0){
	  for(n=1; n<_try+1; n++){
	    if((j>(max_x[n-1]-veto_wx) && j<(max_x[n-1]+veto_wx)) &&
	       (k>(max_y[n-1]-veto_wy) && k<(max_y[n-1]+veto_wy))){
	      in_veto=1;
	    }
	    if(in_veto==1) break;
	  }
	}

	  if(hough_cnt[j][k] > tmp_max_cnt && in_veto==0){
	    tmp_max_x = j;
	    tmp_max_y = k;
	    tmp_max_cnt = hough_cnt[j][k];
	  }
	}
      }
    }
  } // win_typ == 7
  
  if(win_typ == 8){
    for(j=1; j<DIV_HOUGH_X; j++){
      for(k=wini_y1; k<wini_y2; k++){
	if((j<wini_x1 || j>wini_x2) && (k<wini_y1 || k>wini_y2)){

	/* Check if the point on Hough space is out of veto regions */
	/* in_veto==1 --> inside the veto regions(regected) */
	in_veto=0;
	if(veto_flag==1 && _try>0){
	  for(n=1; n<_try+1; n++){
	    if((j>(max_x[n-1]-veto_wx) && j<(max_x[n-1]+veto_wx)) &&
	       (k>(max_y[n-1]-veto_wy) && k<(max_y[n-1]+veto_wy))){
	      in_veto=1;
	    }
	    if(in_veto==1) break;
	  }
	}

	  if(hough_cnt[j][k] > tmp_max_cnt && in_veto==0){
	    tmp_max_x = j;
	    tmp_max_y = k;
	    tmp_max_cnt = hough_cnt[j][k];
	  }
	}
      }
    }
  } // win_typ == 8
  
  
  max_x[_try] = tmp_max_x;
  max_y[_try] = tmp_max_y;
  max_cnt[_try] = tmp_max_cnt;
  
  /* Calcurate parameters of the straight line */
  slope[_try]  = -1.0/tan((float)tmp_max_x/DIV_HOUGH_X*2*PI);
  offset[_try] = (float)tmp_max_y/DIV_HOUGH_Y*1024.0
    /sin((float)tmp_max_x/DIV_HOUGH_X*2*PI);
  
  
  return 0;
}


int get_hough_CM(unsigned int hough_cnt[DIV_HOUGH_X][DIV_HOUGH_Y], //INPUT: Hough space      
                 int max_x[N_TRY], //OUTPUT: theta of the maximum content                    
                 int max_y[N_TRY], //OUTPUT: r of the maximum content                        
                 float slope[N_TRY], //OUTPUT: slope para of the line (y=a*x+b)              
                 float offset[N_TRY], //OUTPUT: offset parameter of the line
		 int wid_x,
		 int wid_y,
		 int _try){  //INPUT: nankaime no Hough search

  /* Parameters */
  int start_x, start_y;
  unsigned int end_x, end_y;
  unsigned int sum_x, sum_y;
  float mean_x, mean_y;
  int n_points;
  int i,j;

  /* Initialization */
  start_x=max_x[_try]-wid_x;
  start_y=max_y[_try]-wid_y;
  end_x=max_x[_try]+wid_x;
  end_y=max_y[_try]+wid_y;

  if(start_x<0) start_x=0;
  if(start_y<0) start_y=0;
  if(end_x>DIV_HOUGH_X) end_x=DIV_HOUGH_X-1;
  if(end_x>DIV_HOUGH_Y) end_x=DIV_HOUGH_Y-1;

  sum_x=0;
  sum_y=0;
  n_points=0;

  /* Calcurate the mean */
  for(i=start_x; i<=end_x; i++){
    for(j=start_y; j<=end_y; j++){
      sum_x+=i*hough_cnt[i][j];
      sum_y+=j*hough_cnt[i][j];
      n_points+=hough_cnt[i][j];
    }
  }

  mean_x=((float)sum_x)/n_points;
  mean_y=((float)sum_y)/n_points;

  //  printf("ori=%d, mean=%f, n=%d\n", max_x[_try], mean_x, n_points);

  /* Calcurate parameters of the straight line */
  slope[_try]  = -1.0/tan((float)mean_x/DIV_HOUGH_X*2*PI);
  offset[_try] = (float)mean_y/DIV_HOUGH_Y*1024.0
    /sin((float)mean_x/DIV_HOUGH_X*2*PI);

  return 0;
}


int elim2_p(struct p_2dim_dyn *p, //2d. points format
	    float slope, float offset,
	    float lat,
	    int elim_wid,
	    int LR_flag){

  int i,j;
  float diff;
  float mean_diff;
  float sigma;
  int cnt, n_out;
  int varid_region;

  cnt=0;
  n_out=0;
  mean_diff=0;
  sigma=0;

  //  printf("Before: %d \n", p->n_comp);
  

  /* First, calcurate the mean difference from the straight line*/
  for(i=0; i<p->n_comp; i++){
    varid_region=0;

    diff = fabs(p->points[i][1] - slope*p->points[i][0] - offset);

    if(LR_flag==0) varid_region=1;
    if(LR_flag==1 && p->points[i][0]<128)  varid_region=1;
    if(LR_flag==2 && p->points[i][0]>=128) varid_region=1;
	  
    if(diff<lat && varid_region==1){
      cnt++;
      mean_diff += diff;
      sigma += pow((p->points[i][1] - slope*p->points[i][0] - offset), 2.0);
    }
  }

  if(cnt==0) cnt=1;
  
  mean_diff = (float)mean_diff/cnt;
  sigma = (float)sqrt(sigma/cnt);
  //  printf("mean diff= %f\n", mean_diff);
  //  printf("sigma= %f\n", sigma);

  for(i=0; i<p->n_comp; i++){
    varid_region=0;

    diff = fabs(p->points[i][1] - slope*p->points[i][0] - offset);
    if(LR_flag==0) varid_region=1;
    if(LR_flag==1 && p->points[i][0]<128)  varid_region=1;
    if(LR_flag==2 && p->points[i][0]>=128) varid_region=1;
    
    if(diff < elim_wid*mean_diff && varid_region==1){
      p->points[i][1] = -9999; //effective NULL value
      p->points[i][0] = -9999; //effective NULL value
      n_out++;
    }
  }  

  int n_elm; //Number of elimination
  n_elm =0;
  /*** Elimination ***/
  for (i = 0; i < p->n_comp - n_out; i++){
    //If i th components should be eliminated,
    if (p->points[i][0] == -9999 && p->points[i][1] == -9999){
      //search for normal value in latter components
      for(j = i+1; j < p->n_comp; j++){
	//If j th component is normal
	if(p->points[j][0] != -9999 && p->points[j][1] != -9999){
	  //Swap j th < - > i th
	  p->points[i][0] = p->points[j][0];
	  p->points[i][1] = p->points[j][1];
	  
	  p->points[j][0] = -9999;
	  p->points[j][1] = -9999;
	  
	  n_elm++;
	  
	  break; //Break from normal value search loop : j
	}//Swap
      }//Search loop
    }//If should be eliminated value
    
    /*
      if(n_elm == n_out){ //If replacement finished
      //Init. rest NULL values in latter components
      for(j = i+1; j < p->n_comp; j++)
      p->points[j][0] = 0;
      p->points[j][1] = 0
      }
      break; //Break from components loop : i
      }
    */
  }//Loop around all the components of p_2dim
  
  p->n_comp = p->n_comp - n_out;
  //  printf("n_elm = %d\n",n_elm);
  //  printf("p->n_comp = %d\n",p->n_comp);
  
  //  printf("After: %d \n", p->n_comp);
  
  return 0;
}


/* For large TOT track elimination added on 15/06/27 */
int elim3_p(unsigned char tpc_in[N_AC][N_TCLK][N_STRP], //INPUT  TPC data format
	    struct p_2dim_dyn *p, //OUTPUT
	    float slope, float offset, //INPUT: parameters of the straight line
	    int iac, //INPUT: anode(0) or cathode(1)
	    int LR_flag,// LR_flag== 0(no divide), 1(left), 2(right) 
	    int win_L, int win_R
	    ){          

  /* parameters */
  int i,j,k;
  int start_strp, end_strp;
  int cross_0, cross_1024;
  int line_y;
  int x,y;
  int n_elim;

  unsigned char** tpc_msk;
  tpc_msk = (unsigned char**)malloc(sizeof(unsigned char*)*N_TCLK);
  if(tpc_msk==NULL) exit(1);
  for(j=0; j<N_TCLK; j++){
    tpc_msk[j] = (unsigned char*)malloc(sizeof(unsigned char)*N_STRP);
    if(tpc_msk==NULL) exit(1);
  }

  if(LR_flag==0){
    win_L=255;
    win_R=0;
  }

  if(LR_flag==1){
    win_R=255;
  }

  if(LR_flag==2){
    win_L=0;
  }

  start_strp=0;
  end_strp=255;

  cross_0 = (int)(-1.0*offset/slope);
  cross_1024 = (int)((1024.0-offset)/slope);

  /* Initializa the TPC buffer */
  for(j=0; j<N_TCLK; j++){
    for(k=0; k<N_STRP; k++){
      tpc_msk[j][k]=0;
    }
  }

  /* Set the line region */
  if(slope<0){
    if(cross_1024<=0) start_strp=0;
    if(cross_1024> 0) start_strp=cross_1024;

    if(cross_0<=255)  end_strp=cross_0;
    if(cross_0> 255)  end_strp=255;
  }

  if(slope>0){
    if(cross_0<=0) start_strp=0;
    if(cross_0> 0) start_strp=cross_0;

    if(cross_1024<=255)  end_strp=cross_1024;
    if(cross_1024> 255)  end_strp=255;
  }

  if(slope==0){
    start_strp=0;
    end_strp=255;
  }

  n_elim=0;

  printf("start = %d, end=%d \n", start_strp, end_strp);

  /* Main routine */
  for(k=start_strp; k<=end_strp; k++){
    if(k<win_L || k>win_R){
      line_y = slope*k + offset;
      
      /* Go down from the line_y */
      for(j=line_y; j>0; j--){
	if(tpc_in[iac][j][k]==0){
	  break;
	} 
	if(tpc_in[iac][j][k]==1){
	  tpc_msk[j][k]=1;
	}
      }

      
      /* Go up from the line_y */
      for(j=line_y+1; j<1024; j++){
	if(tpc_in[iac][j][k]==0) break;
	if(tpc_in[iac][j][k]==1){
	  tpc_msk[j][k]=1;
	}
      }

    }
  }
  
  printf("point=%d\n", p->n_comp);

  /* delete points */
  for(i=0; i<(p->n_comp); i++){
    x=(p->points[i][0]);
    y=(p->points[i][1]);

    if(x>=0 && x<N_STRP && y>=0 && y<N_TCLK){
      if(tpc_msk[y][x]==1){
	p->points[i][0]=-9999;
	p->points[i][1]=-9999;
	n_elim++;
      }
    }
  }

  printf("eliminated %d points.\n", n_elim);
  
  /* release memory */
  for(j=0; j<N_TCLK; j++){
    free(tpc_msk[j]);
  }
  free(tpc_msk);

  return 0;
}


/* For large TOT track elimination added on 15/06/29 
 go left and right from a point on the input line */
int elim4_p(unsigned char tpc_in[N_AC][N_TCLK][N_STRP], //INPUT  TPC data format
	    struct p_2dim_dyn *p, //OUTPUT
	    float slope, float offset, //INPUT: parameters of the straight line
	    int iac, //INPUT: anode(0) or cathode(1)
	    int LR_flag,// LR_flag== 0(no divide), 1(left), 2(right) 
	    int win_L, int win_R // window for left and right
	    ){

  /* parameters */
  int i,j,k;
  int start_clk, end_clk;
  int y_0, y_255;
  int line_x;
  int hit_cnt, null_cnt;
  int null_thresh, hit_thresh;
  int x,y;
  int n_elim;

  null_thresh=5;
  hit_thresh=25;

  unsigned char** tpc_msk;
  tpc_msk = (unsigned char**)malloc(sizeof(unsigned char*)*N_TCLK);
  if(tpc_msk==NULL) exit(1);
  for(j=0; j<N_TCLK; j++){
    tpc_msk[j] = (unsigned char*)malloc(sizeof(unsigned char)*N_STRP);
    if(tpc_msk==NULL) exit(1);
  }

  if(LR_flag==0){
    win_L=255;
    win_R=0;
  }

  if(LR_flag==1){
    win_R=255;
  }

  if(LR_flag==2){
    win_L=0;
  }

  start_clk=0;
  end_clk=N_TCLK;

  y_0 = (int)offset;
  y_255 = (int)(slope*255+offset);

  /* Initializa the TPC buffer */
  for(j=0; j<N_TCLK; j++){
    for(k=0; k<N_STRP; k++){
      tpc_msk[j][k]=0;
    }
  }

  /* Set the search region */
  if(slope<0){
    if(y_255<=0) start_clk=0;
    if(y_255>0 && y_255<N_TCLK) start_clk=y_255;
    if(y_255>N_TCLK){
      printf("Error: y_255 is too large (%d) for slope<0\n", y_255);
      exit(1);
    }
    
    if(y_0>=N_TCLK) end_clk=N_TCLK;
    if(y_0>=0 && y_0<N_TCLK) end_clk=y_0;
    if(y_0<0){
      printf("Error: y_0 is too small (%d) for slope<0\n", y_0);
      exit(1);
    }
  }

  if(slope>0){
    if(y_0<=0) start_clk=0;
    if(y_0>0 && y_0<N_TCLK) start_clk=y_0;
    if(y_0>N_TCLK){
      printf("Error: y_0 is too large (%d) for slope>0\n", y_0);
      exit(1);
    }

    if(y_255>=N_TCLK) end_clk=N_TCLK;
    if(y_255>=0 && y_255<N_TCLK) end_clk=y_255;
    if(y_255<0){
      printf("Error: y_255 is too large (%d) for slope>0\n", y_255);
      exit(1);
    }
  }

  if(slope==0){
    start_clk=offset;
    end_clk=offset+1;
  }

  n_elim=0;

  /* Main routin */
  for(j=start_clk; j<end_clk; j++){
    line_x = (int)(j-offset)/slope;
    
    /* Go left from the line_x point */
    hit_cnt=0;
    null_cnt=0;
    for(k=line_x; k>=0; k--){
      if(tpc_in[iac][j][k]==0) null_cnt++;
      if(tpc_in[iac][j][k]==1){
	tpc_msk[j][k]=1;
	hit_cnt++;
	null_cnt=0;
      }
      if(null_cnt>null_thresh || hit_cnt>hit_thresh){
	break;
      }
    }

    /* Go right from the line_x point */
    hit_cnt=0;
    null_cnt=0;
    for(k=line_x+1; k<N_STRP; k++){
      if(tpc_in[iac][j][k]==0) null_cnt++;
      if(tpc_in[iac][j][k]==1){
	tpc_msk[j][k]=1;
	hit_cnt++;
	null_cnt=0;
      }
      if(null_cnt>null_thresh || hit_cnt>hit_thresh){
	break;
      }
    }

  }

  /* delete points */
  for(i=0; i<(p->n_comp); i++){
    x=(p->points[i][0]);
    y=(p->points[i][1]);

    if(x>=0 && x<N_STRP && y>=0 && y<N_TCLK &&
       (x<win_L || x>win_R)){
      if(tpc_msk[y][x]==1){
	p->points[i][0]=-9999;
	p->points[i][1]=-9999;
	n_elim++;
      }
    }
  }

  /* release memory */
  for(j=0; j<N_TCLK; j++){
    free(tpc_msk[j]);
  }
  free(tpc_msk);

  return 0;
}




int hough_tra(struct tdc_a_p_2dim *tpc,
	      unsigned int hough_cnt[N_AC][DIV_HOUGH_X][DIV_HOUGH_Y]
	      ){

  float y_ratio;
  int track_x, track_y;
  float hough_x, hough_y;
  int hough_y_i;
  int i,j,n;

  y_ratio = 1024.0/DIV_HOUGH_Y;
  n = tpc->p[0].n_comp;


  for(i=0; i<N_AC; i++){
    for(n=0; n<tpc->p[i].n_comp; n++){
      track_x = tpc->p[i].points[n][0];
      track_y = tpc->p[i].points[n][1];

      for(j=0; j<DIV_HOUGH_X; j++){
	hough_x = j*2*PI/DIV_HOUGH_X;
        hough_y = track_x * cos(hough_x) + track_y * sin(hough_x);
        hough_y_i = hough_y/y_ratio;
	if(hough_y_i >=0){
          hough_cnt[i][j][hough_y_i]++;
        }
      }
    }
  }
  return 0;
}


int hough_tra_LR(struct p_2dim_dyn *track, //INPUT: 2d. points format
	       int win1, //INPUT: lower X window of track
	       int win2, //INPUT: upper X window of track
	       unsigned int cnt_L[DIV_HOUGH_X][DIV_HOUGH_Y], //OUTPUT
	       unsigned int cnt_R[DIV_HOUGH_X][DIV_HOUGH_Y] //OUTPUT
	       ){

  float y_ratio;
  int track_x, track_y;
  float hough_x, hough_y;
  int hough_y_i;
  int j,n;

  y_ratio = 1024.0/DIV_HOUGH_Y;


  /* Transform with NO GATE to cnt_L */
  if(win1==0 && win2==0){

    for(n=0; n<track->n_comp; n++){
      track_x = track->points[n][0];
      track_y = track->points[n][1];
      
      for(j=0; j<DIV_HOUGH_X; j++){
	hough_x = j*2*PI/DIV_HOUGH_X;
	hough_y = track_x * cos(hough_x) + track_y * sin(hough_x);
	hough_y_i = hough_y/y_ratio;
	if(hough_y_i>=0 && 
	   track_x>=0 && track_x<=255 && track_y>=0 && track_y<=1024){
	  cnt_L[j][hough_y_i]++;
	}
      }
    }
    
  }
  

  /* Transform with GATE */
  if(win1>0 && win1<256 && win2>0 && win2<256 && win1<=win2){
    for(n=0; n<track->n_comp; n++){
      track_x = track->points[n][0];
      track_y = track->points[n][1];
      
      /* Transform the left track */
      if(track_x <= win1){
	for(j=0; j<DIV_HOUGH_X; j++){
	  hough_x = j*2*PI/DIV_HOUGH_X;
	  hough_y = track_x * cos(hough_x) + track_y * sin(hough_x);
	  hough_y_i = hough_y/y_ratio;
	  if(hough_y_i >=0 &&
	     track_x>=0 && track_x<=255 && track_y>=0 && track_y<=1024){
	    cnt_L[j][hough_y_i]++;
	  }
	}
      }

      /* Transform the right track */      
      if(track_x > win2){
	for(j=0; j<DIV_HOUGH_X; j++){
	  hough_x = j*2*PI/DIV_HOUGH_X;
	  hough_y = track_x * cos(hough_x) + track_y * sin(hough_x);
	  hough_y_i = hough_y/y_ratio;
	  if(hough_y_i >=0 &&
	     track_x>=0 && track_x<=255 && track_y>=0 && track_y<=1024){
	    cnt_R[j][hough_y_i]++;
	  }
	}
      }
      
    }
  }

  if(win1<0 || win1>255 || win2<0 || win2>255 || win1>win2){
    printf("Bad window values: win1=%d, win2=%d \n", win1, win2);
  }
  return 0;
}

int get_vtx_ud(struct p_2dim *track,
	       float vtx_x,
	       float vtx_y,
	       float slope,
	       float offset,
	       int line_wid,
	       int cL,
	       int cR,
	       int iAC){
  

  /* Parameters */
  int n;
  int n_up;
  int n_down;
  float dist;

  /* Initialization of parameters */
  n_up=0;
  n_down=0;

  /* Anode ana */
  if(iAC==0){
    for(n=0; n<track->n_comp; n++){
      dist = fabs(track->points[n][1] - track->points[n][0]*slope - offset);
      if(dist<line_wid){
	if(track->points[n][1] > vtx_y) n_up++;
	if(track->points[n][1] < vtx_y) n_down++;
      }
    }
    if(n_up <  n_down) return -1;
    if(n_up >  n_down) return  1;
    if(n_up == n_down) return  0;
  }


}

int make_tot(unsigned char tdc[N_AC][N_TCLK][N_STRP], 
	     unsigned int tota[N_TOT_A],
	     unsigned int totc[N_TOT_C],
	     unsigned int max_tot_strp[N_AC]){

  int i,j,k;
  unsigned int tmp_max_tot[N_AC];
  unsigned int tmp_max_strp[N_AC];
  
  for(i=0; i<N_AC; i++){
    tmp_max_tot[i]=0;
    tmp_max_strp[i]=0;
  }

  for(i=0; i<N_AC; i++){
    for(j=0; j<N_TCLK; j++){
      for(k=0; k<N_STRP; k++){
	if(tdc[i][j][k]==1){
	  //	  if(i==0) printf("%d\n", k*N_TOT_A/N_STRP);
	  if(i==0) tota[k*N_TOT_A/N_STRP]++;
	  if(i==1) totc[k*N_TOT_C/N_STRP]++;
	}
      }
    }
  }

  /* Find the maximum TOT strip */
  for(i=0; i<N_TOT_A; i++){ //anode
    if(tota[i]>tmp_max_tot[0]){
      tmp_max_tot[0]=tota[i];
      tmp_max_strp[0]=i;
    }
  }
  for(i=0; i<N_TOT_C; i++){ //cathode
    if(totc[i]>tmp_max_tot[1]){
      tmp_max_tot[1]=totc[i];
      tmp_max_strp[1]=i;
    }
  }

  max_tot_strp[0]=(tmp_max_strp[0]*N_STRP/N_TOT_A);
  max_tot_strp[1]=(tmp_max_strp[1]*N_STRP/N_TOT_C);

    return 0;
}

int get_tpc_hitcnt(unsigned char tdc[N_AC][N_TCLK][N_STRP],
		   unsigned int nhit[N_AC]){
  
  int i,j,k;
  unsigned int hita,hitc;
  hita=0;
  hitc=0;

  for(i=0; i<N_AC; i++){
    for(j=0; j<N_TCLK; j++){
      for(k=0; k<N_STRP; k++){
	if(tdc[i][j][k]==1){
	  if(i==0) hita++;
	  if(i==1) hitc++;
	}
      }
    }    
  }
  
  nhit[0]=hita;
  nhit[1]=hitc;

  return 0;  
}

int judge_penetrate(unsigned char tdc[N_AC][N_TCLK][N_STRP], //INPUT TPC data
		    int iac, // INPUT AC type (0=anode, 1=cathode)
		    float slope, float offset){ //INPUT line parameters

  int wid_x, wid_y;
  int thresh;

  wid_x = 10;
  wid_y = 10;
  thresh=10;
  
  int y_0, y_255;
  int x_0, x_1023;
  int x_line, y_line;
  int penet_flag;
  int j,k;
  int min_x, max_x;
  int min_y, max_y;
  int cnt_L, cnt_R, cnt_U, cnt_D;;

  y_0 = (int)offset;
  y_255 = (int)(slope*255+offset);
  x_0 = (int)(-1.0*offset/slope);
  x_1023 = (int)((1023-offset)/slope);

  cnt_L=0;
  cnt_R=0;
  cnt_U=0;
  cnt_D=0;
  penet_flag=0;

  /* Left penetration */
  if(y_0>=0 && y_0<N_TCLK){
    for(k=0; k<wid_x; k++){
      y_line = (int)(k*slope+offset);

      min_y = y_line-wid_y;
      if(min_y<0) min_y=0;

      max_y = y_line+wid_y;
      if(max_y>N_TCLK) max_y=N_TCLK;

      for(j=min_y; j<max_y; j++){
	if(tdc[iac][j][k]==1){
	  cnt_L++;
	}
      }
    }
    if(cnt_L>=thresh){
      //      printf("penetrate to downstream!!\n");
      penet_flag+=1;
    }
  }

  
  /* Right penetration */
  if(y_255>=0 && y_255<N_TCLK){
    for(k=N_STRP-1; k>(N_STRP-wid_x); k--){
      y_line = (int)(k*slope+offset);
      
      min_y = y_line-wid_y;
      if(min_y<0) min_y=0;
      
      max_y = y_line+wid_y;
      if(max_y>N_TCLK) max_y=N_TCLK;
      
      for(j=min_y; j<max_y; j++){
	if(tdc[iac][j][k]==1){
	  cnt_R++;
	}
      }
    }
    
    if(cnt_R>=thresh){
      //      printf("penetrate to upstream!!\n");
      penet_flag+=2;
    }
  }
  
  /* Down penetration */  
  if(x_0>=0 && x_0<N_STRP){
    for(j=0; j<wid_y; j++){
      x_line = (int)((j-offset)/slope);

      min_x = x_line-wid_x;
      if(min_x<0) min_x=0;

      max_x = x_line+wid_x;
      if(max_x>N_STRP) max_x=N_STRP;

      for(k=min_x; k<max_x; k++){
	if(tdc[iac][j][k]==1){
	  cnt_D++;
	}
      }

    }

    if(cnt_D>=thresh){
      //      printf("penetrate to bottom !!\n");
      penet_flag+=4;
    }
  }

  
  if(x_1023>=0 && x_1023<N_STRP){
    for(j=(N_TCLK-1); j>(N_TCLK-wid_y); j--){
      x_line = (int)((j-offset)/slope);

      min_x = x_line-wid_x;
      if(min_x<0) min_x=0;

      max_x = x_line+wid_x;
      if(max_x>N_STRP) max_x=N_STRP;

      for(k=min_x; k<max_x; k++){
	if(tdc[iac][j][k]==1){
	  cnt_U++;
	}
      }

    }

    if(cnt_U>=thresh){
      //      printf("penetrate to top !!\n");
      penet_flag+=8;
    }

  }
  return penet_flag;
}


int get_related_beam(float slope, float offset, //INPUT line parameters of recoil 
		     int tota_edge, //INPUT TOT edge in anode
		     int n_beam, //INPUT number of beam-like particle
		     float beam_slope[N_TRY], float beam_offset[N_TRY]){ //line parameters of beams

  float recoil_x, recoil_y;
  float beam_y[n_beam];
  float dist_y[n_beam];
  float min_dist;
  int i;
  int true_beam_index;

  /*Initialization of parameters */
  for(i=0; i<n_beam; i++){
    beam_y[i]=10000.0;
    dist_y[i]=10000.0;
  }
  min_dist=10000;
  true_beam_index=1000;

  recoil_x=(float)(tota_edge*N_STRP/N_TOT_A);
  recoil_y=recoil_x*slope+offset;


  /* Calcurate distances between beam and recoil edge */
  for(i=0; i<n_beam; i++){
    beam_y[i] = recoil_x*beam_slope[i]+beam_offset[i];
    dist_y[i] = fabs(recoil_y-beam_y[i]);

    if(dist_y[i]<min_dist){
      min_dist=dist_y[i];
      true_beam_index=i;
    }
  }

  //  printf("true beam index=%d\n", true_beam_index);
  //  printf("true beam offset=%f\n", beam_offset[true_beam_index]);

  return true_beam_index;
}


int get_related_beam2(unsigned char tdc[N_AC][N_TCLK][N_STRP], //INPUT TPC data
		      float slope, float offset, //INPUT line parameters
		      int n_beam, //INPUT number of beam-like particle
		      float beam_slope[N_TRY], float beam_offset[N_TRY]){ //line parameters of beams


  int i,j,k;
  int x_0, x_1023;
  int y_0, y_255;
  int recoil_x;
  int not_hit;
  int hit_clk;
  int min_hit_clk;
  int start_y, end_y;
  int line_x;
  int hit_flag, clk_hit_flag;
  int edge_x, edge_y;
  int break_thresh;
  float beam_y[n_beam];
  float dist_y[n_beam];
  float min_dist;
  int true_beam_index;
  int wid_x;

  break_thresh=5;
  min_hit_clk=70;
  wid_x=5;

  /*Initialization of parameters */
  for(i=0; i<n_beam; i++){
    beam_y[i]=10000.0;
    dist_y[i]=10000.0;
  }
  min_dist=10000;
  true_beam_index=1000;


  x_0=(int)(-1.0*offset/slope);
  x_1023=(int)((1023-offset)/slope);
  y_0=offset;
  y_255=255*slope+offset;
  hit_flag=0;
  not_hit=0;
  hit_clk=0;

  /* define the searcing region */
  if(slope<0){

    if(x_1023>=0 && x_1023<N_STRP){
      start_y=N_TCLK-1;
    }
    else{
      start_y=y_0;
    }

    if(x_0>=0 && x_0<N_STRP){
      end_y=0;
    }
    else{
      end_y=y_255;
    }

  }

  if(slope>0){
    
    if(x_0>=0 && x_0<N_STRP){
      start_y=0;
    }
    else{
      start_y=y_0;
    }

    if(x_1023>=0 && x_1023<N_STRP){
      end_y=N_TCLK-1;
    }
    else{
      end_y=y_255;
    }
  }
  
  printf("start=%d, stop=%d\n", start_y, end_y);
  /* Find the end point */
  if(slope<0){
    edge_x=0;
    edge_y=start_y;
    for(j=start_y; j>end_y; j--){
      line_x=(int)((j-offset)/slope);
      clk_hit_flag=0;
     
      for(k=(line_x-wid_x); k<(line_x+wid_x); k++){

	if(tdc[0][j][k]==1){
	  clk_hit_flag=1;
	}
      }	

      if(clk_hit_flag==1){
	hit_flag=1;
	hit_clk++;
	not_hit=0;
	
	if(hit_clk>min_hit_clk){
	  edge_x=line_x;
	  edge_y=j;	
	}
	
      }
      
      if(hit_flag==1 && tdc[0][j][line_x]==0){
	not_hit++;
	if(not_hit>break_thresh){
	  hit_clk=0;
	}
      }

      if(not_hit>break_thresh && hit_clk>min_hit_clk){ 
	printf("break at y=%d,  hit_clk=%d\n", j, hit_clk);
	printf("edge: (%d, %d)\n", edge_x, edge_y);
	break;
      }
    }

  }

  if(slope>0){
    edge_x=0;
    edge_y=start_y;
    for(j=start_y; j<end_y; j++){
      line_x=(int)((j-offset)/slope);
      clk_hit_flag=0;
     
      for(k=(line_x-wid_x); k<(line_x+wid_x); k++){

	if(tdc[0][j][k]==1){
	  clk_hit_flag=1;
	}
      }	

      if(clk_hit_flag==1){
	hit_flag=1;
	hit_clk++;
	not_hit=0;
	
	if(hit_clk>min_hit_clk){
	  edge_x=line_x;
	  edge_y=j;	
	}
	
      }
      
      if(hit_flag==1 && tdc[0][j][line_x]==0){
	not_hit++;
	if(not_hit>break_thresh){
	  hit_clk=0;
	}
      }

      if(not_hit>break_thresh && hit_clk>min_hit_clk){ 
	break;
      }
    }


  }



  /* Find the true beam */
  for(i=0; i<n_beam; i++){
    beam_y[i]=(edge_x)*beam_slope[i]+beam_offset[i];
    dist_y[i]=fabs(beam_y[i]-edge_y);

    if(dist_y[i]<min_dist){
      min_dist=dist_y[i];
      true_beam_index=i;
    }
  }


  //  printf("true beam index=%d\n", true_beam_index);
  //  printf("true beam offset=%f\n", beam_offset[true_beam_index]);

  return true_beam_index;
}

int calc_cross_point(float slope1, float offset1, //INPUT parameters of line1
		     float slope2, float offset2, //INPUT parameters of line2
		     float cross_point[2]){ //OUTPUT cordinate of the point
  
  cross_point[0] = -1.0*(offset1-offset2)/(slope1-slope2);
  cross_point[1] = (slope1*offset2-slope2*offset1)/(slope1-slope2);
  return 0;
}

int find_track_end_c(unsigned char tdc[N_AC][N_TCLK][N_STRP], //INPUT
		     float slope, float offset, 
		     int L_flag){ //INPUT (1=left, 0=right)

  int j,k;
  int start_x, end_x;
  int start_y, end_y;
  int x_0, x_1023;
  float y_0, y_255;
  int strp_hit_flag;
  int line_y;
  int wid_y;
  int edge_strp;
  int not_hit;
  int break_thresh;

  wid_y=20;
  break_thresh=10;

  x_0=(int)(-1.0*offset/slope);
  x_1023=(int)((1023-offset)/slope);
  y_0=offset;
  y_255=255*slope+offset;

  /* Define searching region */
  if(L_flag==1){
    start_x=C_WIN_L;
    edge_strp=C_WIN_L;
    if(y_0>=0 && y_0<N_TCLK){
      end_x=0;
    }
    else{
      if(slope<0) end_x=x_1023;
      if(slope>0) end_x=x_0;
    }
  }

  if(L_flag==0){
    start_x=C_WIN_R;
    edge_strp=C_WIN_R;
    if(y_255>=0 && y_255<N_TCLK){
      end_x=N_STRP;
    }
    else{
      if(slope<0) end_x=x_0;
      if(slope>0) end_x=x_1023;
    }
  }

  /* Serch the edge strip */
  not_hit=0;
  if(L_flag==1){
    for(k=start_x; k>end_x; k--){
      strp_hit_flag=0;
      line_y=(int)(k*slope+offset);
      
      start_y=line_y-wid_y;
      if(start_y<0) start_y=0;
      end_y=line_y+wid_y;
      if(end_y>N_TCLK) end_y=N_TCLK;

      for(j=start_y; j<end_y; j++){
	if(tdc[1][j][k]==1){ 
	  strp_hit_flag=1;
	  edge_strp=k;
	}
      }
      
      if(strp_hit_flag==0) not_hit++;
      if(not_hit>break_thresh) break;
    }
  }

  if(L_flag==0){
    for(k=start_x; k<end_x; k++){
      strp_hit_flag=0;
      line_y=(int)(k*slope+offset);

      start_y=line_y-wid_y;
      if(start_y<0) start_y=0;
      end_y=line_y+wid_y;
      if(end_y>N_TCLK) end_y=N_TCLK;

      for(j=start_y; j<end_y; j++){
	if(tdc[1][j][k]==1){ 
	  strp_hit_flag=1;
	  edge_strp=k;
	}
      }
      
      if(strp_hit_flag==0) not_hit++;
      if(not_hit>break_thresh) break;
    }
  }

  return edge_strp;
}


float calc_Ex(float m1, float m2, float m3, float m4,
	      float K1, float K3, float theta3){

  float E1, E3; //total energy
  float p1, p3; //momentum
  float s, t, u;  //Mandelstam parameters
  float total_m4;
  float ex4;

  E1=m1+K1;
  E3=m3+K3;

  p1=sqrt(E1*E1-m1*m1);
  p3=sqrt(E3*E3-m3*m3);

  s=m1*m1+m2*m2+2*m2*E1;
  t=m1*m1+m3*m3+2*(p1*p3*cos(theta3/180.0*PI)-E1*E3);
  u=m2*m2+m3*m3-2*m2*E3;

  total_m4=sqrt(s+t+u-m1*m1-m2*m2-m3*m3);
  ex4=total_m4-m4;

  return ex4;
}

int fit_2nd_a(unsigned char tdc[N_AC][N_TCLK][N_STRP],
	      float slope_in, float offset_in,
	      float vtx_clk, float stop_clk,
	      float fit_para[2]){
  
  /* parameters */
  int i,j,k;
  int start_clk, end_clk;
  int start_strp, end_strp;
  int y_0, y_255;
  int line_x;
  int n_point;
  
  int window;

  window=20;

  int center_clk;
  int nhit_mid[3];
  int mean_hit;

  int tmp_lead;
  int lead_strp, trad_strp;
  float mid_strp;
  int hit_flag;
  int nhit_strp;
  int not_hit;

  float sum_x, sum_y, sum_xx, sum_xy, sum_yy;
  float offset_out, slope_out;

  y_0 = (int)offset_in;
  y_255 = (int)(slope_in*255+offset_in);

  n_point=0;
  sum_x=0;
  sum_y=0;
  sum_xx=0;
  sum_xy=0;
  sum_yy=0;

  if(slope_in>0){
    start_clk=(int)(stop_clk+10);
    //    end_clk=(int)(vtx_clk-30);
    //    start_clk=(int)(stop_clk);
    end_clk=(int)(vtx_clk);
  }

  if(slope_in<=0){
    //    start_clk=(int)(vtx_clk+30);
    end_clk=(int)(stop_clk-10);
    start_clk=(int)(vtx_clk);
    //    end_clk=(int)(stop_clk);
  }


  printf("start=%d, stop=%d \n", start_clk, end_clk);

  /* calculate the mean hit strip of the recoil track */
  mean_hit=0;
  center_clk=(int)((vtx_clk+stop_clk)/2.0);
  for(j=center_clk-1; j<=center_clk+1; j++){
    line_x = (int)((j-offset_in)/slope_in);

    start_strp=line_x-window;
    end_strp=line_x+window;
    if(start_strp<0) start_strp=0;
    if(end_strp>N_STRP) end_strp=N_STRP;

    for(k=start_strp; k<end_strp; k++){
      if(tdc[0][j][k]==1) mean_hit++;
    }
  }
  mean_hit=(int)(mean_hit/3.0);
  printf("mean_hit=%d \n", mean_hit);

  /* Main routin */
  for(j=start_clk; j<end_clk; j++){
    line_x = (int)((j-offset_in)/slope_in);

    start_strp=line_x-window;
    end_strp=line_x+window;
    if(start_strp<0) start_strp=0;
    if(end_strp>N_STRP) end_strp=N_STRP;

    hit_flag=0;
    nhit_strp=0;

    lead_strp=-1000;
    trad_strp=-1000;

//    /* loop for strips */
//    for(k=start_strp; k<end_strp; k++){
//      if(tdc[0][j][k]==1){
//	nhit_strp++;
//	if(hit_flag==0){
//	  lead_strp=k;
//	}
//	trad_strp=k;
//	hit_flag=1;
//      }
//    }

    not_hit=0;
    nhit_strp=0;
    hit_flag=0;
    for(k=end_strp; k>start_strp; k--){
      if(tdc[0][j][k]==1){
	hit_flag=1;
	tmp_lead=k;
	nhit_strp++;
	not_hit=0;
      }
      
      if(tdc[0][j][k]==0 && hit_flag==1){
	not_hit++;
	if(not_hit>2){
	  lead_strp=tmp_lead;
	  break;
	}
      }
    }



    //    mid_strp=(lead_strp+trad_strp)/2.0;
    mid_strp=lead_strp;

    if(mid_strp>0 && mid_strp<N_STRP && nhit_strp>3 && nhit_strp<mean_hit+5
       && lead_strp<=line_x){
      //    printf("clock=%d, st=%d, end=%d, nhit=%d, lead=%d\n", 
      //	   j, start_strp, end_strp, nhit_strp, lead_strp);

      n_point++;
      sum_x+=mid_strp;
      sum_y+=j;
      sum_xx+=mid_strp*mid_strp;
      sum_xy+=j*mid_strp;
      sum_yy+=j*j;
    }

  }

  /* Calcurate the fitted slope and offset */
  offset_out=(float)((sum_xx*sum_y-sum_x*sum_xy)/
		     (n_point*sum_xx-sum_x*sum_x))*1.0;

  slope_out=(float)((n_point*sum_xy-sum_x*sum_y)/
		    (n_point*sum_xx-sum_x*sum_x))*1.0;
  
  fit_para[0]=offset_out;
  fit_para[1]=slope_out;
  
  printf("1st: slope=%f, offset=%f\n", slope_in, offset_in);
  printf("2nd: slope=%f, offset=%f\n", slope_out, offset_out);
  
  return 0;
}


int fit_2nd_c(unsigned char tdc[N_AC][N_TCLK][N_STRP],
	      float slope_in, float offset_in,
	      float vtx_clk, float stop_clk,
	      float fit_para[2]){

  int j,k;
  int vtx_strp, stop_strp;
  int start_strp, end_strp;
  int start_clk, end_clk;
  int window;
  int line_y;
  int hit_flag;
  int lead_clk, trad_clk;
  float mid_clk;
  int n_point;
  float sum_x, sum_y, sum_xx, sum_xy, sum_yy;
  float offset_out, slope_out;


  window=75;

  n_point=0;
  sum_x=0;
  sum_y=0;
  sum_xx=0;
  sum_xy=0;
  sum_yy=0;


  vtx_strp =(int)((vtx_clk-offset_in)/slope_in);
  stop_strp=(int)((stop_clk-offset_in)/slope_in);

  /* Left case */
  if(vtx_strp>stop_strp){
    start_strp=stop_strp;
    end_strp=vtx_strp-20;
  }

  /* Right case */
  if(vtx_strp<stop_strp){
    start_strp=vtx_strp+20;
    end_strp=stop_strp;
  }

  for(k=start_strp; k<=end_strp; k++){
    line_y=(int)(k*slope_in+offset_in);

    start_clk=line_y-window;
    end_clk=line_y+window;
    if(start_clk<0){
      start_clk=0;
    }
    if(end_clk>N_TCLK){
      end_clk=N_TCLK;
    }

    hit_flag=0;
    lead_clk=-1000;
    trad_clk=-1000;
    for(j=start_clk; j<end_clk; j++){
      if(tdc[1][j][k]==1){
	if(hit_flag==0){
	  lead_clk=j;
	}
	trad_clk=j;
	hit_flag=1;
      }
    }

    mid_clk=(lead_clk+trad_clk)/2.0;

    if(mid_clk>0 && mid_clk<N_TCLK){
      n_point++;
      sum_x+=k;
      sum_y+=mid_clk;
      sum_xx+=k*k;
      sum_xy+=k*mid_clk;
      sum_yy+=mid_clk*mid_clk;
    }
      
  }

  /* Calcurate the fitted slope and offset */
  offset_out=(float)((sum_xx*sum_y-sum_x*sum_xy)/
		     (n_point*sum_xx-sum_x*sum_x))*1.0;

  slope_out=(float)((n_point*sum_xy-sum_x*sum_y)/
		    (n_point*sum_xx-sum_x*sum_x))*1.0;
  
  fit_para[0]=offset_out;
  fit_para[1]=slope_out;

  return 0;
}


float calc_rela_ang(float vec1[3], float vec2[3]){
  float dot;
  float l1, l2;
  float ang;

  dot = vec1[0]*vec2[0]+vec1[1]*vec2[1]+vec1[2]*vec2[2];
  l1  = sqrt(vec1[0]*vec1[0]+vec1[1]*vec1[1]+vec1[2]*vec1[2]);
  l2  = sqrt(vec2[0]*vec2[0]+vec2[1]*vec2[1]+vec2[2]*vec2[2]);

  ang = acos(dot/(l1*l2))*180.0/PI;
  if(ang>90){
    ang = 180.0-ang;
  }

  return ang;
}


float get_fadc_max(float fadc[N_AC][N_FADC][N_ACLK], 
		   int iac, int ich, int win_min, int win_max){

  int i;
  float tmp_max;
  int max_clk;

  tmp_max=-10;
  max_clk=-10;

  if(win_min<0) win_min=0;
  if(win_max>255) win_max=255;
  if(win_min>win_max) win_min=win_max;

  for(i=win_min; i<win_max; i++){
    if(fadc[iac][ich][i] > tmp_max){
      tmp_max=fadc[iac][ich][i];
      max_clk=i;
    }
  }

  return tmp_max;
}
