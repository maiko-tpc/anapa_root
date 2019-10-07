#include <stdio.h>
#include <math.h>
#include <string.h>

// babirl //
#include <bi-config.h>
#include <bi-common.h>
#include <ridf.h>

#include "nrutil.h"
#include "myfunc.h"
#include "cfortran.h"
#include "hbook.h"
#include "anapa.h"
#include "bdc.h"
#include "tpclib.h"

void analysis(struct evtdata *evt, struct enparm *parm){
  int i,j,k;

  /* initialization */
  for(i=0; i<N_AC; i++){
    for(j=0; j<N_FADC; j++){
      evt->max_fadc[i][j]=0;
      evt->min_fadc[i][j]=10000;
      evt->fadc_integ[i][j]=0;
      //      evt->fadc_gain[i][j] = 0.;
    }
  }

  for(i = 0; i < N_AC; i++){
    evt->fadc_integ_sum[i] = 0;
  }

  evt->anode_trg_flag=0;
  evt->anode_trg_cnt=0;
  evt->mini_trg_flag=0;

  evt->strp_cnt[0]=0;
  evt->strp_cnt[1]=0;
  evt->strp_cnt_L=0;
  evt->strp_cnt_R=0;
  evt->si_multi=0;
 
  for(i=0; i<N_AC; i++){
    for(j=0; j<N_STRP; j++){
      evt->cnt_lead[i][j]=0;
      evt->tpc_tot[i][j] = 0;
    }
    evt->cnt_multi_lead[i]=0;
  }

  evt->anode_trg_cnt=0;
  for(i=0; i<N_ACLK; i++){
    evt->fadc_hit_multi[i]=0;
  }
  evt->fadc_multi_trig=0;
  evt->fadc_hit_trig=0;
  /* initialization to here */  

  /////////////////////////////////////////////
  /* Analysis of Si */
  /////////////////////////////////////////////
//  evt->ene_ssd[5]=0.04+0.00823*evt->assd[5];
//  evt->ene_ssd[6]=0.05+0.00819*evt->assd[6];
//  evt->ene_ssd[7]=0.05+0.00821*evt->assd[7];
//  evt->ene_ssd[8]=0.04+0.00817*evt->assd[8];
//
//  for(i=0; i<N_SI; i++){
//    if(evt->si_hit[i]==1) evt->si_multi++;
//  }
//
//  if(evt->ene_ssd[5]>0.6 || evt->ene_ssd[6]>0.6 ||
//     evt->ene_ssd[7]>0.6 || evt->ene_ssd[8]>0.6){
//    //    evt->iflana|=0x1;
//  }

  /////////////////////////////////////////////
  /* Analysis of MAIKo */
  /////////////////////////////////////////////

  //FUNDAMENTAL PHYSICAL QUANTITY ANALYSIS

  /* TOT */
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_STRP; j++){
      int totcnt = 0;
      for(k = 0; k < N_TCLK; k++){
	if(evt->imtpc[i][k][j] == 1) totcnt++;
      }
      evt->tpc_tot[i][j] = totcnt;
    }
  } 


  ///////////////////////////////////////////////////////////////////
  // MAIKo fitting routine 
  // * getting the track region by FADC tot 
  // * getting the track region by TDC tot in above track region
  // * fitting in track region decided by avobe routine
  // * some routine to make fitting accuracy better might be executed
  ////////////////////////////////////////////////////////////////////


  // * getting the track region by FADC tot 

  /* FADC */

/* base line subtraction (event by event)*/
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_FADC; j++){
      evt->baseline_fadc[i][j] = 0;
    }
  }
  /* fit by p0 */
  int N_fit = 15;
  double base_Sy[N_AC][N_FADC];
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_FADC; j++){
      base_Sy[i][j] = 0;
    }
  }
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_FADC; j++){
      for(k = 0; k < N_fit; k++){
	base_Sy[i][j] += evt->icadc[i][j][k];
      }
      evt->baseline_fadc[i][j] = (int)(base_Sy[i][j] / (double)N_fit + 0.5);
    }
  }  /* baseline calc to here */

  for(i=0; i<N_AC; i++){
    for(j=0; j<N_FADC; j++){
      for(k=0; k<N_ACLK; k++){
        if(i==0){
          evt->rev_fadc[i][j][k]=evt->icadc[i][j][k]-evt->baseline_fadc[i][j];
        }
        else{
          evt->rev_fadc[i][j][k]=-(evt->icadc[i][j][k]-evt->baseline_fadc[i][j]);
        }
      }
    }
  }

  int l;
  int fadc_cnt;
 
  /* maximum pusle height */
  for(i=0; i<N_AC; i++){
    for(j=0; j<N_FADC; j++){
      for(k=0; k<N_ACLK; k++){
        if((evt->rev_fadc[i][j][k]) > (evt->max_fadc[i][j])){
          evt->max_fadc[i][j] = evt->rev_fadc[i][j][k];
        }
        if((evt->rev_fadc[i][j][k]) < (evt->min_fadc[i][j])){
          evt->min_fadc[i][j] = evt->rev_fadc[i][j][k];
        }
	
      }
    }
  }  /* maximum pusle height to here*/

  /* integral */
  int th = 3;
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_FADC; j++){
      for(k = 0; k < N_ACLK; k++){
	if(evt->rev_fadc[i][j][k] > th) evt->fadc_integ[i][j] += evt->rev_fadc[i][j][k];
      }
      evt->fadc_integ_sum[i] += evt->fadc_integ[i][j];
    }
  }
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_FADC; j++){
      if(evt->fadc_integ[i][j] == 0) evt->fadc_integ[i][j] = -1;
    }
    if(evt->fadc_integ_sum[i] == 0) evt->fadc_integ_sum[i] = -1;
  }
#ifdef DEBUG
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_FADC; j++){
      printf("fadc_integ[%d][%d] : %d\n",i,j,evt->fadc_integ[i][j]);
    }
    printf("fadc_integ_sum[%d] : %d\n",i,evt->fadc_integ_sum[i]);
  }
#endif
 /* integral to here*/

  /* selection FADC for track start and end */
  int trackflag[N_AC][N_FADC];
  int fadc_track_num[N_AC][2];

  int fadc_tot_th = 2;
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_FADC; j++){
      trackflag[i][j] = 0;
    }
    for(j = 0; j < 2; j++){
      fadc_track_num[i][j] = 0;
    }
  }
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_FADC; j++){
      int temp = 0,temp1 = 0;
      for(k = 0; k < N_ACLK; k++){
	if(evt->rev_fadc[i][j][k] > th){
	  temp++;
	  if(temp1 < temp) temp1 = temp;
	}else temp = 0;
	if(temp1 > fadc_tot_th){
	  trackflag[i][j] = 1;
	  break;
	}
      }
    }
  }

  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_FADC; j++){
      if(trackflag[i][j] == 1){
	fadc_track_num[i][0] = j;
	break;
      }
    }
    for(j = 0; j < N_FADC; j++){
      if(trackflag[i][N_FADC-j-1] == 1){
	fadc_track_num[i][1] = N_FADC - j - 1;
	break;
      }
    }
  }/* select FADC for track start and end to here */

  /* get start & end point of track in above FADC region */
  int tot_th = 10; 
  int tpc_track_num[N_AC][2];
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < 2; j++){
      tpc_track_num[i][j] = -1;
    }
  }
  for(i = 0; i < N_AC; i++){
    for(j = 32 * fadc_track_num[i][0]; j < 32 * (fadc_track_num[i][0] + 1); j++){
      if(evt->tpc_tot[i][j] > tot_th){
	tpc_track_num[i][0] = j;
	break;
      }
      if(tpc_track_num[i][0] == -1) tpc_track_num[i][0] = 32 * (fadc_track_num[i][0] + 1);
    }
    for(j = 32 * fadc_track_num[i][1]; j < 32 * (fadc_track_num[i][1] + 1); j++){
      if(evt->tpc_tot[i][64 * fadc_track_num[i][1] + 31 - j] > tot_th){
	tpc_track_num[i][1] = 64 * fadc_track_num[i][1] + 31 - j;
	break;
      }
      if(tpc_track_num[i][1] == -1) tpc_track_num[i][1] =  32 * fadc_track_num[i][1];
    }
  }  /* get start & end point of track in above FADC region to here*/

  /* get start & end point of track by tpc tdc */
  int track_clock_num[N_AC][N_STRP][2];
  int track_edge_clock[N_AC][2];
  for(i = 0; i < N_AC; i++){
      for(k = 0; k < 2; k++){
	for(j = 0; j < N_STRP; j++){
	  track_clock_num[i][j][k] = -1;
	}
      }
      track_edge_clock[i][0] = 10000;
      track_edge_clock[i][1] = -1;
  }
  for(i = 0; i < N_AC; i++){
    for(j = tpc_track_num[i][0]; j < tpc_track_num[i][1] + 1; j++){
      int temp = 0,temp1 = 0;
      for(k = 0; k < N_TCLK; k++){
	if(evt->imtpc[i][k][j] == 1){
	  temp++;
	  if(temp1 < temp) temp1 = temp;
	}else temp = 0;
	if(temp1 > tot_th){
	  track_clock_num[i][j][0] = k - tot_th;
	  break;
	}
      }
    }
    for(j = tpc_track_num[i][0]; j < tpc_track_num[i][1] + 1; j++){
      int temp = 0,temp1 = 0;
      for(k = 0; k < N_TCLK; k++){
	if(evt->imtpc[i][N_TCLK - k - 1][j] == 1){
	  temp++;
	  if(temp1 < temp) temp1 = temp;
	}else temp = 0;
	if(temp1 > tot_th){
	  track_clock_num[i][j][1] = N_TCLK - k - 1 + tot_th;
	  break;
	}
      }
    }
  }

  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_STRP; j++){
      if((track_edge_clock[i][0] > track_clock_num[i][j][0]) && track_clock_num[i][j][0] != -1) track_edge_clock[i][0] = track_clock_num[i][j][0];
      if((track_edge_clock[i][1] < track_clock_num[i][j][1]) && track_clock_num[i][j][1] != -1) track_edge_clock[i][1] = track_clock_num[i][j][1];
    }
  }  /* get start & end point of track by tpc tdc to here*/

#ifdef DEBUG
  for(i = 0; i < N_AC; i++){
    printf("fadc_track[%d][0] : %d\n",i,tpc_track_num[i][0]);
    printf("fadc_track[%d][1] : %d\n",i,tpc_track_num[i][1]);
    printf("tdc_track[%d][1] : %d\n",i,track_edge_clock[i][0]);
    printf("tdc_track[%d][1] : %d\n",i,track_edge_clock[i][1]);
  }
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < 2; j++){
      printf("fadc_track_num[%d][%d] : %d\n",i,j,fadc_track_num[i][j]);
    }
  }
#endif

  //////////////////
  // TRACK FITTING
  ///////////////////

  /* Initialization of fitting parameters */
  for(i=0; i<N_AC; i++){
//    s_x[i]=0;
//    s_y[i]=0;
//    s_xx[i]=0;
//    s_xy[i]=0;
//    cnt[i]=0;
    evt->tpc_fit_para[i][0]=10000;
    evt->tpc_fit_para[i][1]=10000;
    //    evt->tpc_fit_angle[i] = 0;
    evt->drift_v = -1;
  }

//  /* Simple fitting routine */

//  float s_x[N_AC];
//  float s_y[N_AC];
//  float s_xx[N_AC];
//  float s_xy[N_AC];
//  unsigned int cnt[N_AC];

//  for(i=0; i<N_AC; i++){
//    for(j=0; j<N_TCLK; j++){
//      for(k=0; k<N_STRP; k++){
//	if(evt->imtpc[i][j][k]==1){
//	  s_x[i]+=k;
//          s_y[i]+=j;
//          s_xx[i]+=k*k;
//          s_xy[i]+=k*j;
//          cnt[i]++;
//	}
//      }
//    }
//
//    evt->tpc_fit_para[i][0] = (s_xx[i]*s_y[i] - s_x[i]*s_xy[i])
//      /(cnt[i]*s_xx[i] - s_x[i]*s_x[i]);
//    evt->tpc_fit_para[i][1] = (cnt[i]*s_xy[i] - s_x[i]*s_y[i])
//      /(cnt[i]*s_xx[i] - s_x[i]*s_x[i]);
//

  /* MORIMOTO fitting routine */
  /* calc mean value of x, y */
  float strp_mean[N_AC]={0,0};
  float tclk_mean[N_AC]={0,0};
  int cnt[N_AC] = {0,0};
  for(i = 0; i < N_AC; i++){
    for(j = tpc_track_num[i][0]; j < tpc_track_num[i][1] + 1; j++){
      for(k = track_edge_clock[i][0]; k < track_edge_clock[i][1] + 1; k++){
	if(evt->imtpc[i][k][j] == 1){
	  strp_mean[i] += j;
	  tclk_mean[i] += k;
	  cnt[i]++;
	}
      }
    }
    strp_mean[i] = strp_mean[i]/cnt[i];
    tclk_mean[i] = tclk_mean[i]/cnt[i];
  }  /* calc mean value of x, y to here*/
#ifdef DEBUG
  for(i = 0; i < N_AC; i++){
    printf("strp_mean[%d] : %f\n",i,strp_mean[i]);
    printf("tclk_mean[%d] : %f\n",i,tclk_mean[i]);
  }
#endif

  /* calc coefficient for evaluate function */
  float a[N_AC][3] = {{0, 0, 0},{0, 0, 0}};
  for(i = 0; i < N_AC; i++){
    for(j = tpc_track_num[i][0]; j < tpc_track_num[i][1] + 1; j++){
      for(k = track_edge_clock[i][0]; k < track_edge_clock[i][1] + 1; k++){
	if(evt->imtpc[i][k][j] == 1){
	  a[i][0] += j*j - strp_mean[i]*j;
	  a[i][1] += k*k - tclk_mean[i]*k;
	  a[i][2] += j*k - tclk_mean[i]*strp_mean[i];
	}
      }
    }
  }  /* calc coefficient for evaluate function to here*/
#ifdef DEBUG
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < 3; j++){
      printf("a[%d][%d] : %f\n",i,j,a[i][j]);
    }
  }
#endif

  /* calc minimun of evaluate func and get fitting parameter */
  for(i = 0; i < N_AC; i++){
    evt->tpc_fit_residual[i] = 0;
    float theta[N_AC];
    float minx,maxx;
    minx = 0.;
    maxx = M_PI;
    evt->tpc_fit_residual[i] = mesh_min(minx, maxx, (int)((maxx-minx)/0.01), evalfunc_tpc_fitting2, &theta[i], a[i]);
#ifdef DEBUG
    printf("result[%d] : %f, theta[%d] : %f\n",i,evt->tpc_fit_residual[i],i,theta[i]);
#endif
    evt->tpc_fit_para[i][0] = -(tan(theta[i])*strp_mean[i] - tclk_mean[i]);
    evt->tpc_fit_para[i][1] = tan(theta[i]);
#ifdef DEBUG
    printf("tpc_fit_para[%d][0] : %f\n",i,evt->tpc_fit_para[i][0]);
    printf("tpc_fit_para[%d][1] : %f\n",i,evt->tpc_fit_para[i][1]);
#endif
  }

  ////////////////////////////////////////////////
  // a routine below is accurate but using time..
  ////////////////////////////////////////////////
//  for(i = 0; i < N_AC; i++){
//    int arrayno = 0;
//    float x[10000],y[10000];
//    for(j = tpc_track_num[i][0]; j < tpc_track_num[i][1] + 1; j++){
//      for(k = track_edge_clock[i][0]; k < track_edge_clock[i][1] + 1; k++){
//	if(evt->imtpc[i][k][j] == 1){
//	  arrayno++;
//	  x[arrayno] = j;
//	  y[arrayno] = k;
//	}
//      }
//    }
//    float theta = 0,result = 0;
//    float farrayno = (float)arrayno;
//    float *a[5];
//    a[0] = x;
//    a[1] = y;
//    a[2] = &strp_mean[i];
//    a[3] = &tclk_mean[i];
//    a[4] = &farrayno;
//    result = mesh_min(0, M_PI, (int)(M_PI/0.01), evalfunc_tpc_fitting, &theta, a);
//    printf("result[%d] : %f, theta[%d] : %f\n",i,result,i,theta);
//  }
///////////////////////////////////////////////////////

  /* calc minimun of evaluate func and get fitting parameter to here*/

  /* drift velocity by tracking*/
  if(evt->tpc_fit_para[0][1] < -0.5 && evt->tpc_fit_para[0][1] > -4.){
    evt->drift_v = 4 * tan(30./180.*M_PI)/fabs(evt->tpc_fit_para[0][1]);
  } 
#ifdef VDRIFT
  evt->drift_v = VDRIFT;
#endif
#ifdef DEBUG
  printf("drift speed : %f\n",evt->drift_v);
#endif
  /* drift velocity by anode tracking to here */

  /* translate tclk, strp to mm (only after)*/
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < 2; j++){
      evt->tpc_fit_para_mm[i][j] = 0;
    }
    evt->tpc_fit_angle[i] = 1000;
  }
  for(i = 0; i < N_AC; i++){
    evt->tpc_fit_para_mm[i][0] = evt->drift_v * evt->tpc_fit_para[i][0]/10;
    evt->tpc_fit_para_mm[i][1] = evt->drift_v * evt->tpc_fit_para[i][1]/4;
    evt->tpc_fit_angle[i] = atan(evt->tpc_fit_para_mm[i][1])/M_PI*180;
#ifdef DEBUG
    printf("tpc_fit_para_mm[%d][0] : %f\n",i,evt->tpc_fit_para_mm[i][0]);
    printf("tpc_fit_para_mm[%d][1] : %f\n",i,evt->tpc_fit_para_mm[i][1]);
    printf("tpc_fit_angle[%d]: %f\n",i,evt->tpc_fit_angle[i]);
#endif
  }/* translate tclk, strp to mm to here*/

  /* calc 3-dimensional angle theta, phi */
  for(i = 0; i < 2; i++){
    evt->tpc_3d_angle[i] = 200;
  }
  evt->tpc_3d_angle[0] = atan(fabs(tan(evt->tpc_fit_angle[0]/180.*M_PI)/sin(evt->tpc_fit_angle[1]/180.*M_PI)))/M_PI*180.;
  if(evt->tpc_3d_angle[0] > 1){
    evt->tpc_3d_angle[1] = evt->tpc_fit_angle[1];
  }
#ifdef DEBUG
  printf("track theta : %f\n",evt->tpc_3d_angle[0]);
  printf("track phi : %f\n",evt->tpc_3d_angle[1]);
#endif
  /* calc 3-dimensional angle theta, phi to here*/

  /* calc drift length from above fitting results */
  float track_x[N_AC][2],track_y[N_AC][2];
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < 2; j++){
      track_x[i][j] = -1000;
      track_y[i][j] = -1000;
    }
  }
  //get edge point of the track
  for(i = 0; i < N_AC; i++){
    if(fabs(evt->tpc_fit_angle[i]) > 45){
      for(j = 0; j < 2; j++){
	track_y[i][j] = track_edge_clock[i][j];
	track_x[i][j] = 1/evt->tpc_fit_para[i][1]*track_y[i][j]+strp_mean[i]-1/evt->tpc_fit_para[i][1]*tclk_mean[i];
      }
    }else{
      for(j = 0; j < 2; j++){
	track_x[i][j] = tpc_track_num[i][j];
	track_y[i][j] = evt->tpc_fit_para[i][1]*track_x[i][j]-(evt->tpc_fit_para[i][1]*strp_mean[i]-tclk_mean[i]);
      }
    }
  }//get edge point of the track to here

  //calc range
  for(i = 0; i < N_AC; i++){
    evt->tpc_range[i] = -1;
  }
  float temp = 0;
  for(i = 0; i < N_AC; i++){
    temp += 0.4*(track_x[i][0] - track_x[i][1])*0.4*(track_x[i][0] - track_x[i][1]);
  }
  for(i = 0; i < N_AC; i++){
    evt->tpc_range[i] = sqrt(temp + 0.01*evt->drift_v*evt->drift_v*(track_y[i][0] - track_y[i][1])*(track_y[i][0] - track_y[i][1]));
  }
  evt->anode_drift_length = sqrt(temp);
#ifdef DEBUG
  for(i = 0; i < N_AC; i++){
    printf("tpc_range[%d] : %f\n",i,evt->tpc_range[i]);
  }
#endif
  /* calc drift length from above fitting results to here*/

  /* fiducial cut flag */
  evt->in_fiducial = 0;
  if(fadc_track_num[0][1] < 7 && fadc_track_num[1][0] > 0 && fadc_track_num[1][1] < 7){
    if(evt->tpc_fit_angle[0] > 0){
      if(sqrt(temp) + 50./cos(evt->tpc_fit_angle[0]/180.*M_PI) < 71./sin(evt->tpc_fit_angle[0]/180.*M_PI)){ 
	evt->in_fiducial = 1;
      }
    }
    else if(evt->tpc_fit_angle[0] < 0){
      if(sqrt(temp) + 50./cos(evt->tpc_fit_angle[0]/180.*M_PI) < -58./sin(evt->tpc_fit_angle[0]/180.*M_PI)) 
	{
	  evt->in_fiducial = 1;
      }
    }
  }
#ifdef DEBUG
  printf("anode drift length : %f\n",sqrt(temp));
  printf("in_fiducial : %d\n",evt->in_fiducial);
#endif
  /* fiducial cut to here */

  /* fadc saturation flag */
  int fadc_satu_th = 180;
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_FADC; j++){
      evt->fadc_satu[i] = 0;
    }
  }
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_FADC; j++){
      if(evt->max_fadc[i][j] > fadc_satu_th) evt->fadc_satu[i] = 1;
    }
  }

#ifdef DEBUG
  for(i = 0; i < N_AC; i++){
    printf("fadc_satu[%d] : %d\n",i,evt->fadc_satu[i]);
  }
#endif

  /*********** fadc saturation flag to here ***********/

  /**** tpc zx angle with various cuts ****/
  evt->tpc_zx_angle = 1000;
  evt->total_range = -1;
  float z0 = 50;
  evt->total_range = (z0 + 0.4*fabs((track_x[0][0] - track_x[0][1])))/0.4/(fabs(track_x[0][0] - track_x[0][1])) * evt->tpc_range[0];
#ifdef DEBUG
  printf("total_range : %f\n",evt->total_range);
#endif

  /***** FADC CUT ****/
  evt->fadc_flag = 0;
  if(evt->tpc_range[0] > 60 && evt->tpc_range[0] < 80){
    if((evt->fadc_integ_sum[0] > 18.*evt->tpc_range[0] -36) && (evt->fadc_integ_sum[0] < 25.*evt->tpc_range[0] - 102)) evt->fadc_flag = 1;
    else evt->fadc_flag = -1;
  }
#ifdef DEBUG
  printf("fadc_flag : %d\n",evt->fadc_flag);
#endif
  /***** FADC CUT to here ****/
#ifdef DEBUG
//  printf("iev : %d\n",evt->iev);
//  printf("tgtiev : %d\n",evt->tgtiev);
#endif

  /****** spark event flag *****/
  evt->spark = 0;
  int spark_count[N_AC] = {0 ,0};
  int spark_th = 6000;
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_TCLK; j++){
      for(k = 0; k < N_STRP; k++){
	if(evt->imtpc[i][j][k] == 1) spark_count[i]++;
      }
    }
  }
  for(i = 0; i < N_AC; i++){
    if(spark_count[i] > spark_th) evt->spark = 1;
  }
  /****** spark event flag to here *****/

  /******************* iflana flag ****************************/
  //  if(evt->tpc_range[0] > 30 || evt->tpc_range[1] > 30) evt->iflana = 0x1;
  //  if(evt->iev == 827) 
  //  else evt->iflana = 0x0;
  /*************************** iflana flag to here ************************/

} // analysis function to here
