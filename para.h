/* Analysis parameters of anapa */
/* 2015.02.13 */

#define driftv 0.15 //for He(98%)+CF4(2%), 1000hPa, plate=-5.0kV, grid=-800V
//#define driftv 0.18 //for He(98%)+CF4(2%), 1000hPa, plate=-5.0kV, grid=-800V

#define strpwid 0.40
#define PI 3.1415926535

#define FADC_TH 3
#define FADC_INT_TIME 25 // in 40ns unit


/* Parameters for Hough ana */
#define DIV_HOUGH_X 180
#define DIV_HOUGH_Y 256
#define N_TRY 10
#define HOUGH_TH 150 //for scattering events
//#define HOUGH_TH 100 //for beam search

#define N_TOT_A 64
#define N_TOT_C 256

#define C_WIN_L 110
#define C_WIN_R 140

#define HOUGH_BEAM_LOW  80.0
#define HOUGH_BEAM_HIGH 100.0

/* Information of the scattering */
#define E_BEAM 766
#define M1 12112.547
#define M2 3728.4
#define M3 3728.4
#define M4 12112.547

#define VTX_FADC_WIN 25
#define FADC_VTX_TH 30

/*** for BDC ***********/
#define N_BDC   2
#define N_WIRE 256
#define BDC_PLANE 8
#define N_CALI_PARM 10000
#define WIRE_PITCH 5.00
#define MAIKO_Z 1390.0
//#define MAIKO_Z 850.0
//#define BDC_TDC_L 1400
//#define BDC_TDC_H 3300
#define BDC_TDC_L -14000000
#define BDC_TDC_H  33000000

/* for slit simulation */
#define SLIT_L -1
#define SLIT_R  1
#define SLIT_Z  900
#define SLIT_T  20

/* for plastic */
//#define PLA_T_L -60000
//#define PLA_T_H -42000
//#define PLA_T_L -60000000
//#define PLA_T_H 420000000
#define PLA_T_L -5000
#define PLA_T_H  5000
//#define PLA_Q_L 2000
//#define PLA_Q_H 3000
#define PLA_Q_L -20000000
#define PLA_Q_H  30000000



