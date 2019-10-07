#include "para.h"

float tdc2len(int tdcch, int bdcid, int planeid);

float tdc2len2(int tdcch, int bdcid, int planeid,
	       float parm[N_BDC][BDC_PLANE][2][N_CALI_PARM]);

int bdc_fit(float bdc_pos[N_BDC][BDC_PLANE],
	    float para[N_BDC][2][2], float bdc_fit_resi[N_BDC][BDC_PLANE], 
	    int ibdc, int ixy);

int bdc_fit2(float bdc_pos[N_BDC][BDC_PLANE],
	     float para[2][2], float bdc_fit_resi[N_BDC][BDC_PLANE],
	     int ixy, float bdc_dist);
