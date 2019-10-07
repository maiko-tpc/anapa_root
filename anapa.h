/* Header file for anapa */
/* Ver 0.0 Nov. 19, 2013  T. Kawabata */

#ifndef _ANAPA_H

#include "para.h"
#include <ridf.h>
#include <libbabirl.h>

#define ANA_TPC 
//#define ANA_PPAC 1
#define ANA_SI 1

//#define _DEBUG 1
#define _ANAPA_H 1
//#define N_SCA 32
//#define N_SCA560 32
#define N_SCA 16
#define N_SCA560 16
#define L_LINE 400

#define RINT(a)  (*(int *)&(a))

#define N_SSD 32
#define TRG_CH 64
#define N_V1190 3
#define N_V1290 1
#define MAX_MULTI_V1190 64
#define MAX_MULTI_V1290 64
#define PLA_CH 0
//#define N_QTC  16
#define N_QTC  5
#define N_PPAC 2
#define N_PPAC_CH 5
#define N_SI 16
#define RF_CH 382

/*** for TPC ***********/
#define N_STRP 256   /* Number of strips */
#define N_FADC 8     /* Number of FADC */
#define N_ACLK 256
#define N_TCLK 1024
#define N_AC 2       /* Anode or Cathode */

#define FADC_DATA 0x2
#define TPC_DATA 0x3
#define FOOTER_DATA 0xffffffff

/*** for TPC ***********/
#define N_RF 100

/*** for Ray Trace ****/
#define N_PLANE 9

struct ridf_rhdst_seg{
  struct ridf_rhdst rhd;
  struct stbsegid bsid;
};

typedef struct ridf_rhdst_seg RIDFRHDSEG;

struct enparm {
  /* PPAC parameteors Channel to Time (ns) */
  double ppac_ch_t[3][5];
  /* [0][] ... F2U, [1][] ... F2D, [2][]...F3  */
  /* [0...4]  LDRUA */

  /* PPAC parameters Time (ns) to Distance (mm) */
  double ppac_t_mm[3][4];
  /* [0][] ... F2U, [1][] ... F2D, [2][]...F3  */
  /* [0...3] X slope, Y slope, X offset, Y offset LDRUAb */

  /* PPAC geometrical offset */
  double ppac_geo[3][2];
  /* [0][] ... F2U, [1][] ... F2D, [2][]...F3  */
  /* [][0] ... X, [][1] ... Y  */

  /* Z position */
  double zpos[N_PLANE];
  /* [0] .. F2PPACU */
  /* [1] .. F2PPACD */
  /* [2] .. F2PL    */
  /* [3] .. F2GV    */
  /* [4] .. F2Viewer */
  /* [5] .. TPC */
  /* [6] .. Si */
  /* [7] .. F3PPAC */
};


struct evtdata{
  int nev;     /* Number of event */
  int nevoffset; /* EVT number of the first event */
  int iev;      /* EVT number of the current event */
  int tgtnev;    /* Target event number for the analysis */
  int tgtiev;    /* Target event number for the analysis */
  int iflana;

  int nblk;    /* Number of block */
  int nblkoffset; /* BLK number of the first block number */
  int iblk;    /* Block number of the curret block */

  unsigned int isca[N_SCA];    /* No clear Scaler data */
  unsigned int scv830[N_SCA];  /* V830 Scaler data */
  
  /* raw data for Si + CsI */
  int assd[N_SSD];
  int tssd[N_SSD];

  /* data for v1190 on 16/06/19 for EN BDC test */
  int   v1190_trad[128*N_V1190][MAX_MULTI_V1190];
  int   v1190_lead[128*N_V1190][MAX_MULTI_V1190];
  int v1190_compen[128*N_V1190][MAX_MULTI_V1190];
  unsigned int    v1190_tot[128*N_V1190][MAX_MULTI_V1190];
  unsigned int  multi_v1190[128*N_V1190];
  unsigned int  multi_v1190_lead[128*N_V1190];
  unsigned int  multi_v1190_trad[128*N_V1190];
  unsigned int    hit_v1190[128*N_V1190];
  
  /* data for v1290 on 16/06/19 for EN BDC test */
  int   v1290_trad[32*N_V1290][MAX_MULTI_V1290];
  int   v1290_lead[32*N_V1290][MAX_MULTI_V1290];
  int v1290_compen[32*N_V1290][MAX_MULTI_V1290];
  unsigned int    v1290_tot[32*N_V1290][MAX_MULTI_V1290];
  unsigned int  multi_v1290[32*N_V1290];
  unsigned int  multi_v1290_lead[32*N_V1290];
  unsigned int  multi_v1290_trad[32*N_V1290];
  unsigned int    hit_v1290[32*N_V1290];
  
  unsigned int hit_plane[N_BDC][BDC_PLANE];
  unsigned int hit_wire_cnt[N_BDC][BDC_PLANE];
  unsigned int hit_wire_cnt_gated[N_BDC][BDC_PLANE];
  unsigned int hit_wire_plane[N_BDC][BDC_PLANE];
  unsigned int hit_plane_cnt[N_BDC][2]; //[][0]:X, [][1]:Y
  unsigned int hit_plane_cnt_gated[N_BDC][2]; //[][0]:X, [][1]:Y
  
  float wire_pos[2][16];
  float bdc_pos[N_BDC][BDC_PLANE];

  float drift_time[N_WIRE];
  float drift_time2[N_WIRE][MAX_MULTI_V1190];
  float drift_length[N_WIRE];
  float drift_length_plane[N_BDC][BDC_PLANE];
  unsigned int true_hit_multi_num[N_WIRE];
  unsigned int bdc_good_wire[N_WIRE];
  unsigned int bdc_good_hit;
  
  int pla_q;
  int pla_36Ar;

  float bdc_fit_para[N_BDC][2][2];
  float bdc_fit_resi[N_BDC][BDC_PLANE];

  float bdc_fit_para2[2][2];
  float bdc_fit_resi2[N_BDC][BDC_PLANE];

  /* added on 16/11/14 for QTC */
  unsigned int   qtc_lead[N_QTC][MAX_MULTI_V1190];
  unsigned int   qtc_trad[N_QTC][MAX_MULTI_V1190];
  unsigned int    qtc_tot[N_QTC][MAX_MULTI_V1190];
  unsigned int    qtc_hit[N_QTC];
  unsigned int qtc_multi[N_QTC];

  float f2_LR_multi;
  float f2_LR_sum;
  float f3_LR_multi;
  float f3_LR_sum;
  float f2_mean_time;
  float f3_mean_time;

  unsigned int f2_pla_hit;
  unsigned int f3_pla_hit;
  
  //  float f2_tof[MAX_MULTI_V1190];
  //  float f3_tof[MAX_MULTI_V1190];
  int anode_trg_flag;
  unsigned int anode_trg_cnt;
  int si_trg_flag;
  int mini_trg_flag;

  /* parameters for F2 U+D PPACs */
  unsigned int ppac_lead[N_PPAC][N_PPAC_CH][MAX_MULTI_V1190];
  unsigned int ppac_trad[N_PPAC][N_PPAC_CH][MAX_MULTI_V1190];
  unsigned int ppac_tot[N_PPAC][N_PPAC_CH][MAX_MULTI_V1190];
  unsigned int ppac_hit[N_PPAC][N_PPAC_CH];
  float ppac_pos[N_PPAC][2]; // [][0]:X(mm), [][1]:Y(mm)
  float ppac_ang[2];

  /* parameters for Si */
  unsigned int si_lead[N_SI][MAX_MULTI_V1190];
  unsigned int si_hit[N_SI];
  unsigned int si_trad[N_SI][MAX_MULTI_V1190];
  unsigned int si_tot[N_SI][MAX_MULTI_V1190];
  unsigned int si_multi;


  /* parameters for RF */
  unsigned int rf_lead[MAX_MULTI_V1190];
  unsigned int rf_trad[MAX_MULTI_V1190];
  unsigned int rf_tot[MAX_MULTI_V1190];
  unsigned int rf_hit;

  /* calibration for TPC */
  float max_fadc[2][8];
  float min_fadc[2][8];
  int baseline_fadc[N_AC][N_FADC];
  int rev_fadc[N_AC][N_FADC][N_ACLK];

  float fadc_th[8];
  int fadc_hit_multi[N_ACLK];
  int fadc_multi_trig;
  int fadc_hit_trig;
  int fadc_integ[N_AC][N_FADC];
  int fadc_integ_sum[N_AC];

  
  /* calibration parameters for EN-BDC */
  float bdc_parm[N_BDC][BDC_PLANE][2][N_CALI_PARM];

  /* Calibrated data for Si + CsI */
  int nssd;
  int ncsi;
  int ssdch[N_SSD];
  int csich[N_SSD];
  unsigned int madcclk[2]; /* Time stamp [0]..previous evt, [1]..current evt */
  double ene_ssd[N_SSD];

  int multi_ssd;
  int multi_si, multi_cry;
  int multi_si_adc, multi_cry_adc;

  double time_stamp;
  
  /* PPAC data */
  int tppac[3][5]; 
  double ctppac[3][5]; 
  /* [0][] ... F2U, [1][] ... F2D, [2][]...F3  */
  /* [0...4]  LDRUAb */
  int zv1190;
  double ppacpos[3][2]; /* Position decided by PPAC */

  /*** for Ray Trace ****/
  double trpos[N_PLANE][2]; /* Position at each plane */
  double trang[2][2]; /* Angle [0][]..F2U-D, [1][]..F2D-F3 */

  /* F2 PL */
  //  int f2pla;
  //  int f2plt;

  /* RF */
  //  int nrf;
  //  int rf[N_RF];
  
  /* TPC */
  unsigned int tmb2_bad_header[N_AC][2];
  unsigned int tmb2_bad_fadc_header[N_AC][2];
  unsigned int tmb2_bad_tpc_header[N_AC][2];
  unsigned int tmb2_bad_cnt_inconsistent[N_AC][2];
  unsigned int tmb2_bad_footer[N_AC][2];
  
  unsigned int icadc[N_AC][N_FADC][N_ACLK]; 
  unsigned int tpcclk[4][2]; 
  /* Time stamp [0]..previous evt, [1]..current evt */
  /* ADC data for the current event */
  unsigned int imtpc[N_AC][N_TCLK][N_STRP]; /* TPC data map */
  unsigned int nctpc[N_AC][N_STRP]; /* Number of hit clk in each strip */
  unsigned int nstpc[N_AC];         /* Number of hit strips */
  
  float tr_tpc_madc[4]; /* Time stamp ratio TPC/MADC/10.0 */

  float tpc_fit_para[N_AC][2];
  float tpc_fit_para_mm[N_AC][2];
  float tpc_fit_angle[N_AC];
  float tpc_zx_angle;
  float tpc_3d_angle[2];
  float drift_v;
  int strpcnt[N_AC];
  int tpc_tot[N_AC][N_STRP];
  float tpc_range[N_AC];
  int in_fiducial;
  int fadc_flag;
  int out_fiducial;
  int fadc_satu[N_AC];
  float tpc_fit_residual[N_AC];
  float total_range;
  float anode_drift_length;  
  int spark;

  unsigned int strp_cnt[N_AC];
  unsigned int strp_cnt_L;
  unsigned int strp_cnt_R;

  unsigned int cnt_lead[N_AC][N_STRP];
  unsigned int cnt_multi_lead[N_AC];
};


void file_end(int );
int startup(int argc,char *argv[],int *fd,char *hbfile, int *fdst);
void fopenerror(char *s);
void flushdst(int wpdst);

void initevt(struct evtdata *evt);
unsigned int ri(unsigned short *,unsigned int *);
void booking(struct evtdata *evt);
void analysis(struct evtdata *evt, struct enparm *parm);
void histdef();

int setnextev(struct evtdata *evt);

/* routine used in decode.c ************************/
unsigned int dectpc(RIDFRHDSEG rhdseg,
		    unsigned short *buf,struct evtdata *evt);

unsigned int decmadc32(RIDFRHDSEG rhdseg,
			unsigned short *buf,struct evtdata *evt);
unsigned int decv1190(RIDFRHDSEG rhdseg,
		      unsigned short *buf,struct evtdata *evt);
unsigned int decv1290(RIDFRHDSEG rhdseg,
		      unsigned short *buf,struct evtdata *evt);
unsigned int decv7xx(RIDFRHDSEG rhdseg,
		     unsigned short *buf,struct evtdata *evt);
unsigned int decv830(RIDFRHDSEG rhdseg,
		     unsigned short *buf,struct evtdata *evt);


/************* just for compatibility **************/

struct runparm {
  double m1;                /* Projectile Mass in MeV */
  double m2;                /* Target Mass in MeV */
  double m3;                /* Ejectile Mass in MeV */
  double m4;                /* Residual Target Mass in MeV */
  double K1;                /* Beam Energy */
  double grang;             /* GR Angle */
  double bgr;               /* GR magnetic field in mT */
  double brhogr;            /* GR Brho Tm/c */
  double dpdx;              /* Dispersion of GR */
  double ytgt;              /* Vertical Positon at the Target */
  double dex[2];            /* Fine tuning parameter for Ex */
                            /* dex[0] .. offset, 
                               dex[1] .. first order in Ex */
  double z3;                /* Charge state of the particle 3 */

  double rho;               /* Orbit radius for paritlces with delta=0 */
  char grmode[10];          /* GR MODE NORM or UNF */
  int  igrmode;             /* 0..NORM, 1..UNF */
  double rfc[2];            /* GRRF gate center */
  double rfw;               /* GRRF gate width */
  double yp[2];             /* Y gate peak */
  double yb[4];             /* Y gate back [0]:1L [1]:1H [2]:2L [3]:2H */
  double grpid1[2];         /* PID gate by GR SC1 */

  double lasang;             /* LAS Angle */
  double blas;               /* LAS magnetic field in mT */
  double rholas;            /* Orbit radius for paritlces with delta=0 */
  double brholas;            /* LAS Brho Tm/c */

  double z5;                /* Charge state of the particle 5 (recoiled) */
  double m5;                /* Mass of the particle 5 (recoiled) */
};


#endif
