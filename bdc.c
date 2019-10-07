#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <memory.h>

#include "bdc.h"

float tdc2len(int tdcch, int bdcid, int planeid){

  unsigned int size = 10000;
  float tdc[size], len[size];

  float length;

  char fname[256];
  sprintf(fname, "./calib/bdccalib-%d.dat", 10*bdcid+planeid);

  FILE *fp;
  fp=fopen(fname,"r");
  if(fp==NULL){
    printf("calibration file does not exist!\n");
    return -1;
  }

  int i;
  int cnt=0;

  for(i=0; i<size; i++){
    fscanf(fp, "%f %f", &tdc[i], &len[i]);
    if(feof(fp)==1) break;
    cnt++;
  }
  fclose(fp);

  if(tdcch <= tdc[0]) length=0;
  if(tdcch >= tdc[cnt-1]) length=2.5;

  if(tdcch>tdc[0] && tdcch<tdc[cnt-1]){
    for(i=0; i<cnt-1; i++){
      if(tdcch<tdc[i]){
	length=len[i]+(tdcch-tdc[i])/(tdc[i+1]-tdc[i])*(len[i+1]-len[i]);
	if(length>2.5) length=2.5; 
	break;
      }
    }
  }
  
  //  printf("input=%d, output=%f\n", tdcch, length);

  return length;
}


float tdc2len2(int tdcch, int bdcid, int planeid,
	       float parm[N_BDC][BDC_PLANE][2][N_CALI_PARM]){

  float length;
  int i;
  length = -1;

  if(tdcch <= parm[bdcid][planeid][0][0]) length=0;
  if(tdcch >= parm[bdcid][planeid][0][N_CALI_PARM-1]) length=2.5;
  
  if(tdcch>parm[bdcid][planeid][0][0] && 
     tdcch<parm[bdcid][planeid][0][N_CALI_PARM-1]){
    for(i=0; i<N_CALI_PARM-1; i++){
      if(tdcch < parm[bdcid][planeid][0][i+1]){
	length=parm[bdcid][planeid][1][i] +
	  (tdcch - parm[bdcid][planeid][0][i])/
	  (parm[bdcid][planeid][0][i+1] - parm[bdcid][planeid][0][i])*
	  (parm[bdcid][planeid][1][i+1] - parm[bdcid][planeid][1][i]);
	if(length>2.5) length=100; 
	break;
      }
    }
  }

  //  printf("bdcid=%d, planeid=%d, input=%d, output=%f\n", bdcid, planeid, tdcch, length);
  return length;
}


int bdc_fit(float bdc_pos[N_BDC][BDC_PLANE],
	    float para[N_BDC][2][2], float bdc_fit_resi[N_BDC][BDC_PLANE],
	    int ibdc, int ixy){

  if(ibdc!=0 && ibdc!=1){
    printf("ERROR: ibdc=%d\n", ibdc);
    return 1;
  }
  
  if(ixy!=0 && ixy!=1){
    printf("ERROR: ixy=%d\n", ixy);
    return 1;
  }
  
  float z[4], pos[4];

  if(ixy==0){ // X planes
    z[0]=-17.5;
    z[1]=-12.5;
    z[2]=  2.5;
    z[3]=  7.5;
  }
  
  else{ // Y planes
    z[0]= -7.5;
    z[1]= -2.5;
    z[2]= 12.5;
    z[3]= 17.5;
  }

  if(ixy==0){
    pos[0]=bdc_pos[ibdc][0];
    pos[1]=bdc_pos[ibdc][1];
    pos[2]=bdc_pos[ibdc][4];
    pos[3]=bdc_pos[ibdc][5];
  }

  else{
    pos[0]=bdc_pos[ibdc][2];
    pos[1]=bdc_pos[ibdc][3];
    pos[2]=bdc_pos[ibdc][6];
    pos[3]=bdc_pos[ibdc][7];
  }


  float s_x, s_y, s_xx, s_xy;
  int i;
  float fit_para[2];

  s_x= 0;
  s_y= 0;
  s_xx=0;
  s_xy=0;

  for(i=0; i<4; i++){
    s_x +=z[i];
    s_y +=pos[i];
    s_xx+=z[i]*z[i];
    s_xy+=z[i]*pos[i];
  }

  fit_para[0] = (s_xx*s_y - s_x*s_xy)/(4*s_xx - s_x*s_x);
  fit_para[1] = (4*s_xy - s_x*s_y)/(4*s_xx - s_x*s_x);

  para[ibdc][ixy][0] = fit_para[0];
  para[ibdc][ixy][1] = fit_para[1];

//  printf("BDC:%d, ixy:%d\n", ibdc, ixy);
//  printf("input: %f, %f, %f, %f\n", pos[0], pos[1], pos[2], pos[3]);
//  printf("ang=%f, offset=%f\n", fit_para[1], fit_para[0]);


  /* calculate the residuals */
  if(ixy==0){
    bdc_fit_resi[ibdc][0] = pos[0] - (fit_para[0] + fit_para[1]*z[0]);
    bdc_fit_resi[ibdc][1] = pos[1] - (fit_para[0] + fit_para[1]*z[1]);
    bdc_fit_resi[ibdc][4] = pos[2] - (fit_para[0] + fit_para[1]*z[2]);
    bdc_fit_resi[ibdc][5] = pos[3] - (fit_para[0] + fit_para[1]*z[3]);
  }

  if(ixy==1){
    bdc_fit_resi[ibdc][2] = pos[0] - (fit_para[0] + fit_para[1]*z[0]);
    bdc_fit_resi[ibdc][3] = pos[1] - (fit_para[0] + fit_para[1]*z[1]);
    bdc_fit_resi[ibdc][6] = pos[2] - (fit_para[0] + fit_para[1]*z[2]);
    bdc_fit_resi[ibdc][7] = pos[3] - (fit_para[0] + fit_para[1]*z[3]);
  }

  return 0;
}

int bdc_fit2(float bdc_pos[N_BDC][BDC_PLANE],
	     float para[2][2], float bdc_fit_resi[N_BDC][BDC_PLANE],
	     int ixy, float bdc_dist){

  float z[8], pos[8];

  if(ixy==0){ // X planes
//    z[0]=-17.5-bdc_dist/2.0;
//    z[1]=-12.5-bdc_dist/2.0;
//    z[2]=  2.5-bdc_dist/2.0;
//    z[3]=  7.5-bdc_dist/2.0;
//
//    z[4]=z[0]+bdc_dist/2.0;
//    z[5]=z[1]+bdc_dist/2.0;
//    z[6]=z[2]+bdc_dist/2.0;
//    z[7]=z[3]+bdc_dist/2.0;

    z[0]=-17.5;
    z[1]=-12.5;
    z[2]=  2.5;
    z[3]=  7.5;

    z[4]=z[0]+bdc_dist;
    z[5]=z[1]+bdc_dist;
    z[6]=z[2]+bdc_dist;
    z[7]=z[3]+bdc_dist;
  }
  
  else{ // Y planes
//    z[0]= -7.5-bdc_dist/2.0;
//    z[1]= -2.5-bdc_dist/2.0;
//    z[2]= 12.5-bdc_dist/2.0;
//    z[3]= 17.5-bdc_dist/2.0;
//
//    z[4]=z[0]+bdc_dist/2.0;
//    z[5]=z[1]+bdc_dist/2.0;
//    z[6]=z[2]+bdc_dist/2.0;
//    z[7]=z[3]+bdc_dist/2.0;

    z[0]= -7.5;
    z[1]= -2.5;
    z[2]= 12.5;
    z[3]= 17.5;

    z[4]=z[0]+bdc_dist;
    z[5]=z[1]+bdc_dist;
    z[6]=z[2]+bdc_dist;
    z[7]=z[3]+bdc_dist;
  }

  if(ixy==0){
    pos[0]=bdc_pos[0][0];
    pos[1]=bdc_pos[0][1];
    pos[2]=bdc_pos[0][4];
    pos[3]=bdc_pos[0][5];
    pos[4]=bdc_pos[1][0];
    pos[5]=bdc_pos[1][1];
    pos[6]=bdc_pos[1][4];
    pos[7]=bdc_pos[1][5];
  }

  else{
    pos[0]=bdc_pos[0][2];
    pos[1]=bdc_pos[0][3];
    pos[2]=bdc_pos[0][6];
    pos[3]=bdc_pos[0][7];
    pos[4]=bdc_pos[1][2];
    pos[5]=bdc_pos[1][3];
    pos[6]=bdc_pos[1][6];
    pos[7]=bdc_pos[1][7];
  }

  float s_x, s_y, s_xx, s_xy;
  int i;
  float fit_para[2];

  s_x= 0;
  s_y= 0;
  s_xx=0;
  s_xy=0;

  for(i=0; i<8; i++){
    s_x +=z[i];
    s_y +=pos[i];
    s_xx+=z[i]*z[i];
    s_xy+=z[i]*pos[i];
  }

  fit_para[0] = (s_xx*s_y - s_x*s_xy)/(8.0*s_xx - s_x*s_x);
  fit_para[1] = (8.0*s_xy - s_x*s_y)/(8.0*s_xx - s_x*s_x);

  para[ixy][0] = fit_para[0];
  para[ixy][1] = fit_para[1];


  /* calculate the residuals */
  if(ixy==0){
    bdc_fit_resi[0][0] = pos[0] - (fit_para[0] + fit_para[1]*z[0]);
    bdc_fit_resi[0][1] = pos[1] - (fit_para[0] + fit_para[1]*z[1]);
    bdc_fit_resi[0][4] = pos[2] - (fit_para[0] + fit_para[1]*z[2]);
    bdc_fit_resi[0][5] = pos[3] - (fit_para[0] + fit_para[1]*z[3]);
    bdc_fit_resi[1][0] = pos[4] - (fit_para[0] + fit_para[1]*z[4]);
    bdc_fit_resi[1][1] = pos[5] - (fit_para[0] + fit_para[1]*z[5]);
    bdc_fit_resi[1][4] = pos[6] - (fit_para[0] + fit_para[1]*z[6]);
    bdc_fit_resi[1][5] = pos[7] - (fit_para[0] + fit_para[1]*z[7]);
  }

  if(ixy==1){
    bdc_fit_resi[0][2] = pos[0] - (fit_para[0] + fit_para[1]*z[0]);
    bdc_fit_resi[0][3] = pos[1] - (fit_para[0] + fit_para[1]*z[1]);
    bdc_fit_resi[0][6] = pos[2] - (fit_para[0] + fit_para[1]*z[2]);
    bdc_fit_resi[0][7] = pos[3] - (fit_para[0] + fit_para[1]*z[3]);
    bdc_fit_resi[1][2] = pos[4] - (fit_para[0] + fit_para[1]*z[4]);
    bdc_fit_resi[1][3] = pos[5] - (fit_para[0] + fit_para[1]*z[5]);
    bdc_fit_resi[1][6] = pos[6] - (fit_para[0] + fit_para[1]*z[6]);
    bdc_fit_resi[1][7] = pos[7] - (fit_para[0] + fit_para[1]*z[7]);
  }

  return 0;
}
