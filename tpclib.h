/************** tpclib.h **************/

#ifndef _TPCLIB
#define _TPCLIB 1
#include "anapa.h"
#include "para.h"

//#define MAX_POINT_COMP 12000 //Specify sufficient number
#define MAX_POINT_COMP 12000 //Specify sufficient number
#define MAX_LINE_COMP 10 //Specify sufficient number

#define N_MULT 5 // Acceptable number of multi-hit in mhit_x(y)

/******************** definition of STRUCT. ******************/

/*
Type "p_2dim" is general purpose PSEUDO-variable-length 2d. points format.
This consists of at most "MAX_POINT_COMP" 2d. cordinates point data.
If MAX_POINT_COMP is chosen "adequently" (sufficiently large),
p_2dim can be  regarded as variable length 2d. points format.
Length of series of points is represented by "n_comp." 
*/
struct p_2dim{ //Array of 2 dimensional points
  float points[MAX_POINT_COMP][2]; //Main part of this struct.
  int n_comp; //Number of points
};

/* for malloc version */
struct p_2dim_dyn{ //Array of 2 dimensional points
  float** points;
  int n_comp; //Number of points
};



/*
Type "fadc_a_p_2dim" consists of data of FADC as 2 d. points format.
8 strips for Anode and Cathode ( 2 x 8 ARRAY ) and
each FADC strip has a series of 256 points (analogue value) as a p_2dim.
*/
struct fadc_a_p_2dim{ //Array of array of 2 d. points for FADC
  struct p_2dim p[N_AC][N_FADC]; //p_2dim x N_AC (2) x N_FADC (8)
};

/*
Type "tdc_a_p_2dim" consists of data of TDC as 2 d. points format.
Anode and Cathode has a series of points as a p_2dim.
*/
struct tdc_a_p_2dim{ //Array of array of 2 d. points for TDC
  struct p_2dim_dyn p[N_AC]; //p_2dim x N_AC (2)
};

struct tdc_a_p_2dim_dyn{ //Array of array of 2 d. points for TDC
  struct p_2dim_dyn p[N_AC]; //p_2dim x N_AC (2)
};

/*
Type "area_2dim" is PSEUDO-variable-length 2d. area format.
This consists of at most "MAX_LINE_COMP" 2d. cordinates line data.
Lines described by linear function
 lines[][0] * x + lines[][1] * y + lines[][2] = 0
define the border of the area.
LHS of formular above is
  positive (if lines[][3] > 0) or
  negative (if lines[][3] < 0)
in the area.
If MAX_LINE_COMP is chosen "adequently" (sufficiently large),
area_2dim can be  regarded as variable length 2d. area format.
Length of series of lines is represented by "n_comp." 
*/
struct area_2dim{ //Area of 2 dim. designated with line
  double lines[MAX_LINE_COMP][4]; //Main part of this struct.
  int n_comp; //Number of lines
};

/******************** definition of FUNC. ******************/

int init_p_2dim(struct p_2dim *p); //Intiallize p_2dim

/* Intiallize p_2dim for malloc version */
int init_p_2dim_dyn(struct p_2dim_dyn *p, unsigned int hitcnt); 

int init_fadc_a_p_2dim(struct fadc_a_p_2dim *a_p); //Intiallize fadc_a_p_2dim

int init_tdc_a_p_2dim(struct tdc_a_p_2dim *a_p); //Intiallize tdc_a_p_2dim

/* Intiallize tdc_a_p_2dim for malloc version */
int init_tdc_a_p_2dim_dyn(struct tdc_a_p_2dim *a_p, unsigned int hitcnt); 

int init_area_2dim(struct area_2dim *a_p); //Intiallize area_2dim

int cp_p_2dim(struct p_2dim *p,
              struct p_2dim *p_cp); //COPY of p_2dim "p"

int cp_p_2dim_dyn(struct p_2dim_dyn *p,
              struct p_2dim_dyn *p_cp); //COPY of p_2dim "p"

int cp_area_2dim(struct area_2dim *a,
                 struct area_2dim *a_cp); //COPY of area_2dim "a"

int refill_fadc(float fadc[N_AC][N_FADC][N_ACLK], //INPUT : FADC DATA
                struct fadc_a_p_2dim *a_p); //OUTPUT : Array of points array

int refill_tdc(unsigned char tdc[N_AC][N_TCLK][N_STRP], //INPUT : TDC DATA
	       struct tdc_a_p_2dim *a_p); //OUTPUT : Array of points array

/* for malloc version */
int refill_tdc_dyn(unsigned char tdc[N_AC][N_TCLK][N_STRP], //INPUT : TDC DATA
		   struct tdc_a_p_2dim *a_p, //OUTPUT : Array of points array
		   unsigned int hitcnt); //INPUT: hit count

int add_p(struct p_2dim *p, //2 d. points format
	  float *p_in); //INPUT value : point p_in[2]

int add_brd(struct area_2dim *a, //2 d. Area format
	    double *l_para, //INPUT value : Parameter of border line
	    int p_n); //INPUT value : Parameter to define "INSIDE"

/*
int refill_fadc(float ***fadc, //INPUT : FADC DATA[N_AC][N_FADC][N_ACLK]
                struct fadc_a_p_2dim *a_p); //OUTPUT : Array of points array
*/
double sumy_p(struct p_2dim *p);//2d. point format
double peak_p(struct p_2dim *p, //2d. point format
	      int pol); //Greatest (pol > 0) or Smallest (pol < 0) value returned

int mm_p(struct p_2dim p, //2 d. points format
	     float *min, //RETURN value : 2 para. min. of point min[0] <= x, min[1] <= y
	     float *max);//RETURN value : 2 para. max. of point max[0] >= x, max[1] >= y

int stat_p(struct p_2dim p, //2 d. points format
	   float *min, //RETURN value : 2 para. min. of point min[0] <= x, min[1] <= y
	   float *max, //RETURN value : 2 para. max. of point max[0] >= x, max[1] >= y
	   double *cm, //RETURN value : 2 para. Center of Mass (x,y)=(cm[0],cm[1])
      	   double *st_dv, //RETURN value : 2 para. Standard deviation for x & y
	   double *crl_cef); //RETURN value : correlation coefficient between x-y

int fit_p_lin(int tdc_hit[N_AC][N_STRP][N_TCLK], //TDC hit pattern
              int ac, //Anode(0) or Cathode(1)
	      double *fit_para, //RETURN value : 3 parameter of linear function
	      double *res); //RETURN value : average residual deviation

int fit_lin(struct p_2dim p, //2 d. points format
	    double *fit_para, //RETURN value : 3 parameters of linear function
	    double *res); //RETURN value : average residual deviation

int fit_con(struct p_2dim p, //2d. points format (DEFINED in tpclib.h)
              double *fit_para, //RETURN value : 3 parameter of constant value (fit_lin compatible)
	    double *res); //RETURN value : residual of fitting
int elim_p(struct p_2dim *p, //2d. points format
	   double *l_para, //INPUT : standard linear function for eliminating
	   double lat);    //INPUT : lattitude of preservation

int pick_p_o(struct p_2dim *p, //2d. points format
	   struct p_2dim *p_sub, //RETURN : subset of p
	   int cp_ll,     //INPUT : lower limit of subset
           int cp_ul);    //INPUT : upper limit of subset

int pick_p_a(struct p_2dim *p, //2d. points format
             struct area_2dim *a, //2d. area format
	     struct p_2dim *p_sub, //RETURN : subset of p
	     int i_o);    //INPUT : Inside or Outside of the area


/* added 2015/01/04 by cotemba 
      calculate the baseline for FADC */
float get_fadc_bl(struct p_2dim p, //2d points format
		  int iac,  //INPUT: 0-->anode, 1-->cathode, for debug
		  int ifadc, //INPUT: 0--7, for debug
		  int DBG_LEVEL);//INPUT: 0--3, for debug


int msk_tpc(unsigned char tmp_tpc[N_AC][N_TCLK][N_STRP], //INPUT: tpc raw track 
	    float re_fadc[N_AC][N_FADC][N_ACLK], //INPUT: revised FADC
	    int fadc_th, //INPUT: FADC threshold (should be defined in histdef.c)
	    int msk[N_AC][N_FADC][N_ACLK], // RETURN: mask of TPC
	    unsigned char tpc2[N_AC][N_TCLK][N_STRP]); // RETURN: revised track of TPC

int get_tpcinfo(struct tdc_a_p_2dim *a_p, //INPUT: 2d. data for TPC
		int tpc_nhit[N_AC], //OUTPUT: number of hit strips
		float tpc_min[N_AC][2], //OUTPUT: minimum strip number
		float tpc_max[N_AC][2], //OUTPUT: maximum strip number
		float tpc_wid[N_AC][2], //OUTPUT: (max)-(min)
		int DBG_LEVEL);

int find_hough_max(unsigned int hough_cnt[N_AC][DIV_HOUGH_X][DIV_HOUGH_Y], //INPUT: Hough space
		   float win_x1[N_AC], float win_x2[N_AC], //INPUT: window para for theta
		   float win_y1[N_AC], float win_y2[N_AC], //INPUT: window para for r
		   int win_typ[N_AC], //INPUT: window type --> see Log Book
		   int max_x[N_AC][N_TRY], //OUTPUT: theta of the maximum content
		   int max_y[N_AC][N_TRY], //OUTPUT: r of the maximum content
		   int max_cnt[N_AC][N_TRY], //OUTPUT: maximum content
		   float slope[N_AC][N_TRY], //OUTPUT: slope para of the line (y=a*x+b)
		   float offset[N_AC][N_TRY], //OUTPUT: offset parameter of the line
		   int _try); //INPUT: nankaime no Hough search

int find_hough_max_LR(unsigned int hough_cnt[DIV_HOUGH_X][DIV_HOUGH_Y], //INPUT: Hough space
		      float win_x1, float win_x2, //INPUT: window para for theta
		      float win_y1, float win_y2, //INPUT: window para for r
		      int win_typ, //INPUT: window type --> see Log Book
		      int veto_wx, //INPUT: VETO width for the previous max points  
		      int veto_wy, //INPUT: VETO width for the previous max points  
		      int veto_flag, //INPUT: VETO flags
		      int max_x[N_TRY], //OUTPUT: theta of the maximum content
		      int max_y[N_TRY], //OUTPUT: r of the maximum content
		      int max_cnt[N_TRY], //OUTPUT: maximum content
		      float slope[N_TRY], //OUTPUT: slope para of the line (y=a*x+b)
		      float offset[N_TRY], //OUTPUT: offset parameter of the line
		      int _try); //INPUT: nankaime no Hough search


int get_hough_CM(unsigned int hough_cnt[DIV_HOUGH_X][DIV_HOUGH_Y], //INPUT: Hough space
		 int max_x[N_TRY], //OUTPUT: theta of the maximum content
		 int max_y[N_TRY], //OUTPUT: r of the maximum content
		 float slope[N_TRY], //OUTPUT: slope para of the line (y=a*x+b)
		 float offset[N_TRY], //OUTPUT: offset parameter of the line
		 int wid_x,
		 int wid_y,
		 int _try); //INPUT: nankaime no Hough search

int elim2_p(struct p_2dim_dyn *p, //2d. points format
	    float slope, float offset, //INPUT: parameters of the straight line
	    float lat, //INPUT: width to calcurate mean TOT
	    int elim_wid, //INPUT: width of the VETO line
	    int LR_flag //INPUT: whether divide the track into left and right 
	    );          // LR_flag== 0(no divide), 1(left), 2(right) 

/* For large TOT track elimination added on 15/06/27 
 go up and down from a point on the input line */
int elim3_p(unsigned char tpc_in[N_AC][N_TCLK][N_STRP], //INPUT  TPC data format
	    struct p_2dim_dyn *p, //OUTPUT
	    float slope, float offset, //INPUT: parameters of the straight line
	    int iac, //INPUT: anode(0) or cathode(1)
	    int LR_flag,// LR_flag== 0(no divide), 1(left), 2(right) 
	    int win_L, int win_R
	    );          

/* For large TOT track elimination added on 15/06/29 
 go left and right from a point on the input line */
int elim4_p(unsigned char tpc_in[N_AC][N_TCLK][N_STRP], //INPUT  TPC data format
	    struct p_2dim_dyn *p, //OUTPUT
	    float slope, float offset, //INPUT: parameters of the straight line
	    int iac, //INPUT: anode(0) or cathode(1)
	    int LR_flag,// LR_flag== 0(no divide), 1(left), 2(right) 
	    int win_L, int win_R // window for left and right
	    );          


int hough_tra(struct tdc_a_p_2dim *tpc, //INPUT: 2d. data for TPC
	      unsigned int hough_cnt[N_AC][DIV_HOUGH_X][DIV_HOUGH_Y] //OUTPUT: contents of each bin in hough space
	      );

int hough_tra_LR(struct p_2dim_dyn *track, //INPUT: 2d. points format
	       int win1, //INPUT: lower X window of track
	       int win2, //INPUT: upper X window of track
	       unsigned int cnt_L[DIV_HOUGH_X][DIV_HOUGH_Y], //OUTPUT
	       unsigned int cnt_R[DIV_HOUGH_X][DIV_HOUGH_Y] //OUTPUT
	       );


int get_vtx_ud(struct p_2dim *track,
	       float vtx_x,
	       float vtx_y,
	       float slope,
	       float offset,
	       int line_wid,
	       int cL,
	       int cR,
	       int iAC);

int make_tot(unsigned char tdc[N_AC][N_TCLK][N_STRP], // INPUT TPC data
	     unsigned int tota[N_TOT_A], //OUTPUT TOT data in anode
	     unsigned int totc[N_TOT_C], //OUTPUT TOT data in cathode
	     unsigned int max_tot_strp[N_AC]); //OUTPUT max tot strip numbers

int get_tpc_hitcnt(unsigned char tdc[N_AC][N_TCLK][N_STRP],
		   unsigned int nhit[N_AC]);


int judge_penetrate(unsigned char tdc[N_AC][N_TCLK][N_STRP], //INPUT TPC data
		    int iac, // INPUT AC type (0=anode, 1=cathode)
		    float slope, float offset); //INPUT line parameters

int get_related_beam(float slope, float offset, //INPUT line parameters
		     int tot_edge, //INPUT TOT edge in anode
		     int n_beam, //INPUT number of beam-like particle
		     float beam_slope[N_TRY], float beam_offset[N_TRY]); //line parameters of beams

int get_related_beam2(unsigned char tdc[N_AC][N_TCLK][N_STRP], //INPUT TPC data
		      float slope, float offset, //INPUT line parameters
		      int n_beam, //INPUT number of beam-like particle
		      float beam_slope[N_TRY], float beam_offset[N_TRY]); //line parameters of beams

int calc_cross_point(float slope1, float offset1, //INPUT parameters of line1
		     float slope2, float offset2, //INPUT parameters of line2
		     float cross_point[2]); //OUTPUT cordinate of the point

int find_track_end_c(unsigned char tdc[N_AC][N_TCLK][N_STRP], //INPUT
		     float slope, float offset,
		     int L_flag); //INPUT (1=left, 0=right)

float calc_Ex(float m1, float m2, float m3, float m4, // INPUT masses of the particles in MeV
	      float K1, float K3, // INPUT kinetic energy of the insident and recoil particles
	      float theta3); //INPUT recoil angle in degree

int fit_2nd_a(unsigned char tdc[N_AC][N_TCLK][N_STRP],
	      float slope_in, float offset_in,
	      float vtx_clk, float stop_clk,
	      float fit_para[2]);

int fit_2nd_c(unsigned char tdc[N_AC][N_TCLK][N_STRP],
	      float slope_in, float offset_in,
	      float vtx_clk, float stop_clk,
	      float fit_para[2]);

float calc_rela_ang(float vec1[3], float vec2[3]);

float get_fadc_max(float fadc[N_AC][N_FADC][N_ACLK], 
		   int iac, int ich, int win_min, int win_max);

#endif
