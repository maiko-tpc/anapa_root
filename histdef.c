#include <stdio.h>
#include <math.h>
#include <string.h>

/* babirl */
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

//#define DEBUG
#define BDC_TDC_OFFSET 7700
#define BDC_TRACK_BIN 200
#define TPC_CLK_FREQ 100000000.0
#define MADC_CLK_FREQ 16000000.0
//#define VDRIFT 0.87 //for PID (H2(1500))
#define VDRIFT 1.91
#define VDRIFT_TH 1.
void histdef(){
  int i,j;
  char hname[128];

  HBOOK1(10, "TMB2 bad header", 4,0.,4., 0);
  HBOOK1(11, "TMB2 bad FADC header", 4,0.,4., 0);
  HBOOK1(12, "TMB2 bad TPC  header", 4,0.,4., 0);
  HBOOK1(13, "TMB2 bad count inconsistent", 4,0.,4., 0);
  HBOOK1(14, "TMB2 bad footer", 4,0.,4., 0);

  /* Histgrams for TPC */
  HBOOK2(10000, "Anode track",     256,0.,256., 1024,0.,1024., 0);
  HBOOK2(10001, "Anode sum",       256,0.,256., 1024,0.,1024., 0);
  HBOOK2(10010, "Cathode track",   256,0.,256., 1024,0.,1024., 0);
  HBOOK2(10011, "Cathode sum",     256,0.,256., 1024,0.,1024., 0);

  HBOOK2(10020, "Anode track(over)",       256,0.,256., 1024,0.,1024., 0);
  HBOOK2(10021, "Anode sum(over)",       256,0.,256., 1024,0.,1024., 0);
  HBOOK2(10022, "Cathode track(over)",       256,0.,256., 1024,0.,1024., 0);
  HBOOK2(10023, "Cathode sum(over)",       256,0.,256., 1024,0.,1024., 0);

  HBOOK2(10030, "Anode track(under)",       256,0.,256., 1024,0.,1024., 0);
  HBOOK2(10031, "Anode sum(under)",       256,0.,256., 1024,0.,1024., 0);
  HBOOK2(10032, "Cathode track(under)",       256,0.,256., 1024,0.,1024., 0);
  HBOOK2(10033, "Cathode sum(under)",       256,0.,256., 1024,0.,1024., 0);

  HBOOK2(10050, "Anode track(fiducial)",   256,0.,256., 1024,0.,1024., 0);
  HBOOK2(10060, "Cathode sum(fiducial)",     256,0.,256., 1024,0.,1024., 0);

  HBOOK2(10070, "Anode sum(test)",     256,0.,256., 1024,0.,1024., 0);
  HBOOK2(10080, "Cathode sum(test)",     256,0.,256., 1024,0.,1024., 0);

  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC anode ch%d in mV", i);
    HBOOK1(11000+i, hname, 256,0.,256., 0);
  }
  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC cathode ch%d in mV", i);
    HBOOK1(11010+i, hname, 256,0.,256., 0);
  }
  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC anode ch%d baseline correction", i);
    HBOOK1(11020+i, hname, 256,0.,256., 0);
  }
  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC cathode ch%d baseline correction", i);
    HBOOK1(11030+i, hname, 256,0.,256., 0);
  }
  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC anode   ch%d max", i);
    HBOOK1(11040+i, hname, 256,0., 256., 0);
  }
  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC cathode ch%d max", i);
    HBOOK1(11050+i, hname, 256,0., 256., 0);
  }
  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC anode ch%d baseline corr. sum ", i);
    HBOOK1(11060+i, hname, 200,0.,2000., 0);
  }
  HBOOK1(11068, "FADC anode ch0 baseline corr. sum test", 200,0.,2000., 0);
  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC cathode ch%d baseline corr. sum", i);
    HBOOK1(11070+i, hname, 200,0.,2000., 0);
  }
  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC anode ch%d baseline corr. sum (over)", i);
    HBOOK1(11080+i, hname, 200,0.,2000., 0);
  }
  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC cathode ch%d baseline corr. sum (over)", i);
    HBOOK1(11090+i, hname, 200,0.,2000., 0);
  }

  HBOOK2(14000, "Anode track sum 0degree",     256,0.,256., 1024,0.,1024., 0);
  HBOOK2(14010, "Cathode track sum 0degree",   256,0.,256., 1024,0.,1024., 0);
  HBOOK2(15000, "Anode track sum 30degree",  256,0.,256., 1024,0.,1024., 0);
  HBOOK2(15010, "Cathode track sum 30degree",  256,0.,256., 1024,0.,1024., 0);
  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC anode   ch%d max 0deg gated", i);
    HBOOK1(14040+i, hname, 256,0., 256., 0);
  }
  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC cathode ch%d max 0deg gated", i);
    HBOOK1(14050+i, hname, 256,0., 256., 0);
  }
  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC anode   ch%d max 30deg gated", i);
    HBOOK1(15040+i, hname, 256,0., 256., 0);
  }
  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC cathode ch%d max 30deg gated", i);
    HBOOK1(15050+i, hname, 256,0., 256., 0);
  }
  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC anode ch%d baseline corr. sum 0deg gated", i);
    HBOOK1(14060+i, hname, 500,0.,2000., 0);
  }
  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC cathode ch%d baseline corr. integ 0deg gated", i);
    HBOOK1(14070+i, hname, 500,0.,2000., 0);
  }
  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC anode ch%d baseline corr. sum 30deg gated", i);
    HBOOK1(15060+i, hname, 1000,0.,2000., 0);
  }
  for(i=0; i<N_FADC; i++){
    sprintf(hname, "FADC cathode ch%d baseline corr. sum 30deg gated", i);
    HBOOK1(15070+i, hname, 1000,0.,2000., 0);
  }

  HBOOK2(12000, "Anode track in mm",  256,0.,102.4, 1024,0.,102.4, 0);
  HBOOK2(12010, "Cathode track in mm",  256,0.,102.4, 1024,0.,102.4, 0);
  HBOOK2(12001, "Anode track sum in mm",  256,0.,102.4, 1024,0.,102.4, 0);
  HBOOK2(12011, "Cathode track sum in mm",  256,0.,102.4, 1024,0.,102.4, 0);
  HBOOK1(12002, "Anode track angle",180, -90., 90., 0);
  HBOOK1(12052, "Anode track angle(fiducial)",180, -90., 90., 0);
  HBOOK1(12053, "Anode track angle(over)",180, -90., 90., 0);
  HBOOK1(12054, "Anode track angle(under)",180, -90., 90., 0);
  HBOOK1(12012, "Cathode track angle",180, -90., 90., 0);
  HBOOK1(12062, "Cathode track angle(fiducial)",180, -90., 90., 0);
  HBOOK1(12063, "Cathode track angle(over)",180, -90., 90., 0); 
  HBOOK1(12064, "Cathode track angle(under)",180, -90., 90., 0);
  HBOOK2(12072, "Anode track angle(fiducial) vs RangeA",250., 0., 180., 180, -90., 90, 0.);
  HBOOK1(12003, "Anode track fit para",500, -20., 20., 0);
  HBOOK1(12004, "Anode track fit para long track",500, -10., 10., 0);
  HBOOK1(12013, "Cathode track fit para",500, -20., 20., 0);
  HBOOK1(12500, "track angle theta",90, 0., 90., 0);
  HBOOK1(12501, "track angle phi",180, -90., 90., 0);
  HBOOK2(12502,"track angle theta vs phi",90, -90., 90.,45, 0., 90., 0);
  HBOOK1(12600,"0deg track fit residual",100, 0., 1000., 0);
//  HBOOK2(12503,"track angle theta vs phi range gated",90, -90., 90.,45, 0., 90., 0);
//  HBOOK2(12504,"theta_a vs theta_c over",45, -90., 90.,45, -90., 90., 0);
//  HBOOK2(12505,"theta_a vs theta_c under",45, -90., 90.,45, -90., 90., 0);
//  HBOOK2(12506,"theta_a vs theta_c",45, -90., 90.,45, -90., 90., 0);
  HBOOK1(13000, "drift velocity",100, 0., 3., 0);
  HBOOK2(13001, "drift velocity anode track", 256,0.,256., 1024,0.,1024., 0);
  HBOOK2(13002, "drift velocity cathode track", 256,0.,256., 1024,0.,1024., 0);
  HBOOK1(13050, "drift velocity by anode long track",100, 0., 3., 0);
  HBOOK2(13090, "drift velocity vs madc time stamp",100, 0., 3., 100, 0., 4320000, 0.);
  HBOOK2(13091, "drift velocity vs tpc time stamp", 100, 0., 3., 100, 0., 4320000, 0.);
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_FADC; j++){
      if(i == 0){
	sprintf(hname,"Anode ch%d baseline",j);
	HBOOK1(20000 + 10*i + j, hname, 200., -100., 100., 0);
      }
      if(i == 1){
	sprintf(hname,"Cathode ch%d baseline",j);
	HBOOK1(20000 + 10*i + j, hname, 156., 100., 256., 0);
      }
    }
  }
  HBOOK1(30000, "FADC anode all ch. sum", 300., 0., 3000., 0);
  HBOOK1(30005, "FADC anode all ch. sum(over)", 300., 0., 3000., 0);
  HBOOK1(30010, "FADC anode all ch. sum 0deg gated", 3000., 0., 3000., 0);
  HBOOK1(30020, "FADC anode all ch. sum 30deg gated", 3000., 0., 3000., 0);
  HBOOK1(30100, "FADC cathode all ch. sum", 3000., 0., 3000., 0);
  HBOOK1(30105, "FADC cathode all ch. sum(over)", 300., 0., 3000., 0);
  HBOOK1(30110, "FADC cathode all ch. sum 0deg gated", 500., 0., 3000., 0);
  HBOOK1(30120, "FADC cathode all ch. sum 30deg gated", 3000., 0., 3000., 0);
  HBOOK1(31000, "track length by anode", 150., 0., 150., 0);
  HBOOK1(31010, "track length by cathode", 150., 0., 150., 0);
  HBOOK2(32000, "FADC(Anode) vs Range(Anode)", 250., 0., 180.,  250., 0., 2500., 0);
  HBOOK2(32001, "FADC(Anode) vs Range(Cathode)", 250., 0., 180.,  250., 0., 2500., 0);

  HBOOK2(32002, "FADC(Cathode) vs Range(Anode)", 200., 0., 150.,  250., 0., 2500., 0);
  HBOOK2(32003, "FADC(Cathode) vs Range(Cathode)", 200., 0., 150.,  250., 0., 2500., 0);

  HBOOK2(32004, "FADC(Anode) vs Range(Anode)", 250., 0., 180.,  250., 0., 2500., 0);
  HBOOK2(32005, "FADC(Anode) vs Range(Cathode)", 250., 0., 180.,  250., 0., 2500., 0);

  HBOOK2(32006, "FADC(Cathode) vs Range(Anode)", 200., 0., 150.,  250., 0., 2500., 0);
  HBOOK2(32007, "FADC(Cathode) vs Range(Cathode)", 200., 0., 150.,  250., 0., 2500., 0);
  HBOOK2(32008, "FADC vs Range(test)", 200., 0., 150.,  250., 0., 2500., 0);

  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_FADC; j++){
      if(i == 0) sprintf(hname,"%s%d%s","Anode FADC",j,"ch vs Range");
      else sprintf(hname,"%s%d%s","Cathode FADC",j,"ch vs Range");
      HBOOK2(32100 + 10*i + j,hname, 200., 0., 150.,  100., 0., 1000., 0);
    }
  }
//  for(i = 0; i < N_FADC; i++){
//    HBOOK2(40000 + i,"anode fadc persistent(fiducial)",256,0.,256., 256., 0., 256., 0);
//    HBOOK2(40020 + i,"anode fadc persistent(over)",256,0.,256., 256., 0., 256., 0);
//    HBOOK2(40040 + i,"anode fadc persistent(under)",256,0.,256., 256., 0., 256., 0);
//  }
//  for(i = 0; i < N_FADC; i++){
//    HBOOK2(40010 + i,"cathode fadc persistent(fiducial)",256,0.,256., 256., 0., 256., 0);
//    HBOOK2(40030 + i,"cathode fadc persistent(over)",256,0.,256., 256., 0., 256., 0);
//    HBOOK2(40050 + i,"cathode fadc persistent(under)",256,0.,256., 256., 0., 256., 0);
//  }
  
  HBOOK1(70000,"0degree track range",100,0.,100.,0);
//  HBOOK2(70001,"0deg track range vs fadc mux pulse",100,0.,100.,128.,0.,256.,0);
//  HBOOK2(70002,"0deg track range vs fadc integ",100,0.,100.,500.,0.,1000.,0);
//  HBOOK2(70003,"0deg track", 256,0.,256., 1024,0.,1024., 0);

  HBOOK1(80000,"total range", 180., 0., 180., 0.);
  HBOOK1(80001,"total range(over)", 180., 0., 180., 0.);
  HBOOK1(80002,"total range(under)", 180., 0., 180., 0.);
  HBOOK1(80010,"total range(all_events)", 180., 0., 180., 0.);
} // end of histdef




void booking(struct evtdata *evt){
  int i,j,k,l;
  for(i=0; i<N_AC; i++){
    for(j=0; j<2; j++){
      if(evt->tmb2_bad_header[i][j]==1)           HF1(10, 2*i+j, 1.0);
      if(evt->tmb2_bad_fadc_header[i][j]==1)      HF1(11, 2*i+j, 1.0);
      if(evt->tmb2_bad_tpc_header[i][j]==1)       HF1(12, 2*i+j, 1.0);
      if(evt->tmb2_bad_cnt_inconsistent[i][j]==1) HF1(13, 2*i+j, 1.0);
      if(evt->tmb2_bad_footer[i][j]==1)           HF1(14, 2*i+j, 1.0);
    }
  }


  ///////////////////////////////////////////////
  /* Booking of MAIKo */
  ///////////////////////////////////////////////

  /* Booking for Si */
//#ifdef ANA_SI
//  for(i=5; i<9; i++){
//    HF1(6000+(i-5), evt->assd[i], 1.0);
//    if(evt->hit_v1190[i+256]==1){
//      HF1(6010+(i-5), evt->assd[i], 1.0);
//      HF1(6020+(i-5), evt->ene_ssd[i], 1.0);
//
//      if(evt->strp_cnt_L>60 || evt->strp_cnt_R>60){
//	HF1(6030+(i-5), evt->ene_ssd[i], 1.0);
//      }
//    }
//  }
//  HF1(6040, evt->si_multi, 1.0);
//#endif

#ifdef ANA_TPC
  /* test */
  /* count nstrp for track */
  int strpth = 30;
  for(i = 0; i < N_AC; i++){
      evt->strpcnt[i] = 0;
  }
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_STRP; j++){
      if(evt->tpc_tot[i][j] > 10) evt->strpcnt[i]++;
    }
  }
#ifdef DEBUG
  for(i = 0; i < N_AC; i++){
    printf("strpcnt[%d] : %d\n",i,evt->strpcnt[i]);
  }
#endif


  /* TRACK */
  for(i=0; i<N_AC; i++){
    HRESET(10000+i*10, "");
    HRESET(10020+i*10, "");
    HRESET(10022+i*10, "");
    HRESET(12000+i*10,"");
    //    HRESET(45000+i*10,"");
  }
  for(i=0; i<N_AC; i++){
    for(j=0; j<N_TCLK; j++){
      for(k=0; k<N_STRP; k++){
	if(evt->imtpc[i][j][k]==1){
	  HF2(10000+i*10, k, j, 1.0);
	  HF2(10001+i*10, k, j, 1.0);
	  if(evt->in_fiducial == 1){
	    HF2(10050+i*10, k, j, 1.0);
	  }
	  if(evt->fadc_flag == 1 && evt->in_fiducial == 1){
	    if(i == 0){
	      HF2(10030, k, j, 1.0);
	      HF2(10031, k, j, 1.0);
	    }else if(i == 1){
	      HF2(10032, k, j, 1.0);
	      HF2(10033, k, j, 1.0);
	    }
	  }
	  if(evt->fadc_flag == -1 && evt->in_fiducial == 1){
	    if(i == 0){
	      HF2(10020, k, j, 1.0);
	      HF2(10021, k, j, 1.0);
	    }else if(i == 1){
	      HF2(10022, k, j, 1.0);
	      HF2(10023, k, j, 1.0);
	    }
	  }
	  if(evt->tpc_3d_angle[0] > 40 && evt->tpc_3d_angle[0] < 50){
	    HF2(10070 + 10*i, k, j, 1.);
	      }
	  HF2(12000+i*10, 0.4*k, 0.1*evt->drift_v*j, 1.0);
	  HF2(12001+i*10, 0.4*k, 0.1*evt->drift_v*j, 1.0);
	  if(evt->tpc_fit_para[0][1] > -0.5 && evt->tpc_fit_para[0][1] < 0.5 && i == 0){
	    HF2(14000, k, j, 1.0);
	  }else if(evt->tpc_fit_para[0][1] > 2 && evt->tpc_fit_para[0][1] < 4&& i == 0){
	    HF2(15000, k, j, 1.0);
	  }
	  if(evt->tpc_fit_para[1][1] > -0.5 && evt->tpc_fit_para[1][1] < 0.5 && i == 1){
	    HF2(14010, k, j, 1.0);
	  }else if(evt->tpc_fit_para[0][1] > 0.5 && evt->tpc_fit_para[0][1] < 2.5 && i == 1){
	    HF2(15010, k, j, 1.0);
	  }
	}
      }
    }
  }
  for(i = 0; i < N_AC; i++){
    HF1(12002 + 10*i,evt->tpc_fit_angle[i], 1.);
    if(evt->in_fiducial == 1){
      HF1(12052 + 10*i,evt->tpc_fit_angle[i], 1.);
      if(evt->fadc_flag == 1) HF1(12054 + 10*i,evt->tpc_fit_angle[i], 1.);
      else if(evt->fadc_flag == -1) HF1(12053 + 10*i,evt->tpc_fit_angle[i], 1.);
    }
    HF1(12003 + 10*i,evt->tpc_fit_para[i][1],1.);
  }
  if(evt->in_fiducial == 1) HF2(12072,evt->anode_drift_length, evt->tpc_fit_angle[0],1.);
  if(evt->strpcnt[0] > strpth) HF1(12004,evt->tpc_fit_para[0][1],1.);
  for(i = 0; i < 2; i++){
    HF1(12500 + i,evt->tpc_3d_angle[i],1.);
  }
  HF2(12502,evt->tpc_3d_angle[1],evt->tpc_3d_angle[0],1.);
  if(evt->tpc_range[1] > 10){
    HF2(12503,evt->tpc_3d_angle[1],evt->tpc_3d_angle[0],1.);
  }
  if(evt->tpc_fit_angle[1] > -0.5 && evt->tpc_fit_angle[1] < 0.5){
    HF1(12600, sqrt(evt->tpc_fit_residual[1]), 1.);
  }
  /* FADC */
  for(i=0; i<N_AC; i++){
    for(j=0; j<N_FADC; j++){
      HRESET(11000+i*10+j, "");
      HRESET(11020+i*10+j, "");
      //      HRESET(45100+10*i+j,"");
    }
  }

  for(i=0; i<N_AC; i++){
    for(j=0; j<N_FADC; j++){
      for(k=0; k<N_ACLK; k++){
	//        HF1(11000+i*10+j, k, evt->icadc[i][j][k] * 8); //(if mV)
        HF1(11000+i*10+j, k, evt->icadc[i][j][k]);
	HF1(11020+i*10+j, k, evt->rev_fadc[i][j][k] * 8); //(if mV)
	//	HF1(11020+i*10+j, k, evt->rev_fadc[i][j][k]);
      }
    }
  }


  for(i=0; i<N_AC; i++){
    for(j=0; j<N_FADC; j++){
      HF1(11040+i*10+j, evt->max_fadc[i][j],1.0);
      if(evt->tpc_fit_para[0][1] > -0.5 && evt->tpc_fit_para[0][1] < 0.5 && 
	 evt->strpcnt[0] > strpth){
	HF1(14040+i*10+j, evt->max_fadc[i][j],1.0);
      }else if(evt->tpc_fit_para[0][1] < -1.5 && evt->strpcnt[0] > strpth){
	HF1(15040+i*10+j, evt->max_fadc[i][j],1.0);
      }
    }
  }
  
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_FADC; j++){
      HF1(11060+i*10 + j, evt->fadc_integ[i][j], 1.);
      if(evt->in_fiducial == 1 && evt->total_range > 135 && 
	 evt->fadc_satu[0] == 0 && evt->fadc_satu[1] == 0){
	HF1(11080+i*10 + j, evt->fadc_integ[i][j], 1.);
      }
      if(evt->tpc_fit_para[0][1] > -0.5 && evt->tpc_fit_para[0][1] < 0.5 ){
	HF1(14060+i*10+j, evt->fadc_integ[i][j],1.0);
      }else if(evt->tpc_fit_para[0][1] > 2 && evt->tpc_fit_para[0][1] < 4){
	HF1(15060+i*10+j, evt->fadc_integ[i][j],1.0);
      }
    }
  }
  if(evt->tpc_fit_angle[0] > 20 && evt->tpc_fit_angle[0] < 40){
    HF1(11068, evt->fadc_integ[0][0]/cos(30./180.*M_PI), 1.);
  }else if(evt->tpc_fit_angle[0] > -10 && evt->tpc_fit_angle[0] < 10){
    //    HF1(11068, evt->fadc_integ[0][0], 1.);
  }
  if(evt->tpc_fit_para[0][1] > -2 && evt->tpc_fit_para[0][1] < -0.5){
    HF1(13000, evt->drift_v, 1.);
    if(evt->strpcnt[0] > strpth) HF1(13050, evt->drift_v, 1.);
    for(i = 0; i < N_AC; i++){
      for(j = 0; j < N_FADC; j++){
	for(k = 0; k < N_TCLK; k++){
	  if(evt->imtpc[i][k][j] == 1){
	    HF2(13001 + i, j,k, 1.);
	  }
	}
      }
    }
    HF2(13090, evt->drift_v, evt->madcclk[0], 1.);
    HF2(13091, evt->drift_v, evt->tpcclk[0][1], 1.);
  }
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_FADC; j++){
      HF1(20000 + 10*i + j, evt->baseline_fadc[i][j], 1.0);
    }
  }
  for(i = 0; i < N_AC; i++){
    HF1(30000 + 100 * i, evt->fadc_integ_sum[i], 1.);
    if(evt->fadc_flag == -1 && evt->in_fiducial == 1 && 
       evt->fadc_satu[0] == 0 && evt->fadc_satu[1] == 0){
      HF1(30005 + 100 * i,evt->fadc_integ_sum[i], 1.);
    }
    if(evt->tpc_fit_para[0][1] > -0.5 && evt->tpc_fit_para[0][1] < 0.5
       && evt->strpcnt[0] > strpth){
      HF1(30010 + 100 * i,evt->fadc_integ_sum[i], 1.);
    }else if(evt->tpc_fit_para[0][1] > 1.5 && evt->strpcnt[0] > strpth){
      HF1(30020 + 100 * i,evt->fadc_integ_sum[i], 1.);
    }
  }

  /* track length, PID etc... */
  int i_ev = evt->iev;
  for(i = 0; i < N_AC; i++){
    HF1(31000 + 10*i, evt->tpc_range[i], 1.);
  }
  for(i = 0; i < 4; i++){
    //    HF2(32010 + i,evt->tpc_range[i%2],evt->fadc_integ_sum[(int)i/2],1.);
    if(evt->in_fiducial == 1 && evt->fadc_satu[0] == 0 && evt->fadc_satu[1] == 0)
      HF2(32000 + i,evt->tpc_range[i%2],evt->fadc_integ_sum[(int)i/2],1.);
    if(evt->fadc_flag == -1)HF2(32004 + i,evt->tpc_range[i%2],evt->fadc_integ_sum[(int)i/2],1.);
  }

  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_FADC; j++){
      if(evt->in_fiducial == 1 && evt->fadc_satu[0] == 0 && evt->fadc_satu[1] == 0){
	HF2(32100 + 10*i + j,evt->tpc_range[i],evt->fadc_integ[i][j],1.);
      }
    }
  }
  int over,under;
  if((evt->fadc_integ_sum[1] < (evt->tpc_range[1] * 14.15 + 70.2) + 64.5) && (evt->fadc_integ_sum[1] > (evt->tpc_range[1] * 14.15 + 70.2) - 64.5) && evt->tpc_range[1] > 30 && evt->tpc_range[1] < 40) over = 1;
  else over = 0;
  if((evt->fadc_integ_sum[1] < (evt->tpc_range[1] * 9.2 + 15.1) + 64.5) && (evt->fadc_integ_sum[1] > (evt->tpc_range[1] * 9.2 + 15.1) - 64.5) && evt->tpc_range[1] > 30 && evt->tpc_range[1] < 40) under = 1;
  else under = 0;
  if(over){
    for(i = 0; i < N_AC; i++){
      for(j = 0; j < N_FADC; j++){
	for(k = 0; k < N_ACLK; k++){
	  HF2(51000+100*i+10*j,k,evt->rev_fadc[i][j][k],1);
	}
	HF2(50000,evt->tpc_range[1],evt->fadc_integ_sum[1],1.);
      }
    }
  }
  if(under){
    for(i = 0; i < N_AC; i++){
      for(j = 0; j < N_FADC; j++){
	for(k = 0; k < N_ACLK; k++){
	  HF2(52000+100*i+10*j,k,evt->rev_fadc[i][j][k],1);
	}
	HF2(50001,evt->tpc_range[1],evt->fadc_integ_sum[1],1.);
      }
    }
  }
  if(evt->in_fiducial && evt->tpc_range[0] < 80 && evt->tpc_range[0] > 60 && evt->fadc_integ_sum[0] > 2000 && evt->fadc_integ_sum[0] < 2250) HF2(12504,evt->tpc_fit_angle[0],evt->tpc_fit_angle[1],1.);
  else if(evt->in_fiducial && evt->tpc_range[0] < 80 && evt->tpc_range[0] > 60 && evt->fadc_integ_sum[0] > 1500 && evt->fadc_integ_sum[0] < 1750) HF2(12505,evt->tpc_fit_angle[0],evt->tpc_fit_angle[1],1.);
  HF2(12506,evt->tpc_fit_angle[0],evt->tpc_fit_angle[1],1.);  
  for(i = 0; i < N_AC; i++){
    if(over) HF1(53000+10*i,evt->fadc_integ_sum[i],1.);
    else if(under) HF1(53100+10*i,evt->fadc_integ_sum[i],1.);
  }
  int irange;
//  for(i = 0; i < N_AC; i++){
//    HRESET(60100 + 10*i,"");
//    HRESET(60000 + i,"");
//  }
  if(evt->tpc_3d_angle[0] > 70 && evt->tpc_3d_angle[0] < 80 && evt->tpc_3d_angle[1] > -30 && evt->tpc_3d_angle[1] < -20 && evt->tpc_range[1] > 30 && evt->tpc_range[1] < 40){
    for(i = 0; i < N_AC; i++){
      for(j = 0; j < N_STRP; j++){
	for(k = 0; k < N_TCLK; k++){
	  if(evt->imtpc[i][k][j] == 1) HF2(60100+10*i,j,k,1.);
	}
      }     
    }
    if(over){
      for(i = 0; i < N_ACLK; i++){
	HF1(60000,i,evt->rev_fadc[1][6][i]);
	HF2(60002,i,evt->rev_fadc[1][6][i],1.);
      }
    }else if(under){
      for(i = 0; i < N_ACLK; i++){
	HF1(60001,i,evt->rev_fadc[1][6][i]);
	HF2(60003,i,evt->rev_fadc[1][6][i],1.);
      }
    }
  }

  if(evt->tpc_fit_para[1][1] > -0.5 && evt->tpc_fit_para[1][1] < 0.5){
    HF1(70000,evt->tpc_range[1],1.);
    HF2(70001,evt->tpc_range[1],evt->max_fadc[1][7],1.);
    HF2(70002,evt->tpc_range[1],evt->fadc_integ[1][7],1.);
    for(i = 0; i < N_STRP; i++){
      for(j = 0; j < N_TCLK; j++){
	if(evt->imtpc[1][j][i] == 1){
	  HF2(70003,i,j,1.);
	}
      }
    }
  }
  for(i = 0; i < N_AC; i++){
    for(j = 0; j < N_FADC; j++){
      for(k = 0; k < N_ACLK; k++){
	if(evt->in_fiducial == 1){
	  HF2(40000 + 10*i + j,k,evt->rev_fadc[i][j][k],1);
	  if(evt->fadc_flag == -1) HF2(40020 + 10*i + j,k,evt->rev_fadc[i][j][k],1);
	  else if(evt->fadc_flag == 1) HF2(40040 + 10*i + j,k,evt->rev_fadc[i][j][k],1);
	}
      }
    }
  }
  HF1(80010, evt->total_range, 1.);
  if(evt->in_fiducial == 1 && evt->fadc_satu[0] == 0 && evt->fadc_satu[1] == 0){
    HF1(80000, evt->total_range, 1.);
    if(evt->fadc_flag == 1) HF1(80001, evt->total_range, 1.);
    else if(evt->fadc_flag == -1) HF1(80002, evt->total_range, 1.);
  }
#endif
} // end of booking

