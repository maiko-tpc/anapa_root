#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>


/* babirl */
#include <bi-config.h>
#include <bi-common.h>
#include <ridf.h>

/* anapa */
#include "anapa.h"

void initevt(struct evtdata *evt){
  int i,j,k;
  
  for(i=0; i<(int)128*N_V1190; i++){
    for(j=0; j<MAX_MULTI_V1190; j++){
      evt->v1190_lead[i][j]=100;
      evt->v1190_trad[i][j]=100;
      evt->v1190_tot[i][j]=0;
      evt->v1190_compen[i][j]=0;
    }
    evt->multi_v1190[i]=0;
    evt->multi_v1190_lead[i]=0;
    evt->multi_v1190_trad[i]=0;
    evt->hit_v1190[i]=0;
  }

  for(i=0; i<N_QTC; i++){
    for(j=0; j<MAX_MULTI_V1190; j++){
      evt->qtc_lead[i][j]=100000;
      evt->qtc_trad[i][j]=100000;
      evt->qtc_tot[i][j]=0;
    }
    evt->qtc_hit[i]=0;
    evt->qtc_multi[i]=0;
  }

  for(i=0; i<N_PPAC; i++){
    for(j=0; j<N_PPAC_CH; j++){
      evt->ppac_hit[i][j]=0;
      for(k=0; k<MAX_MULTI_V1190; k++){
	evt->ppac_lead[i][j][k]=100;
	evt->ppac_trad[i][j][k]=100;
	evt->ppac_tot[i][j][k]=0;
      }
    }
  }

  for(i=0; i<MAX_MULTI_V1190; i++){
    evt->rf_lead[i]=0;
    evt->rf_trad[i]=0;
    evt->rf_tot[i]=0;
  }
  evt->rf_hit=0;

  /* for time stamp */
  evt->madcclk[0]=evt->madcclk[1];
  for(i=0; i<4; i++){
    evt->tpcclk[i][0]=evt->tpcclk[i][1];
  }

#ifdef ANA_TPC
  for(i=0; i<N_AC; i++){
    for(j=0; j<N_TCLK; j++){
      for(k=0; k<N_STRP; k++){
	evt->imtpc[i][j][k]=0;
      }
    }
  }

  for(i=0; i<N_AC; i++){
    for(j=0; j<N_FADC; j++){
      for(k=0; k<N_ACLK; k++){
	evt->icadc[i][j][k]=evt->baseline_fadc[i][j];
      }
    }
  }
#endif

  for(i=0; i<N_AC; i++){
    for(j=0; j<2; j++){
      evt->tmb2_bad_header[i][j]=0;
      evt->tmb2_bad_fadc_header[i][j]=0;
      evt->tmb2_bad_tpc_header[i][j]=0;
      evt->tmb2_bad_cnt_inconsistent[i][j]=0;
      evt->tmb2_bad_footer[i][j]=0;
    }
  }

  evt->si_trg_flag=0;
  for(i=0; i<N_SI; i++){
    evt->si_hit[i]=0;
  }

} // initevt() to here

/** buf should be pointor to the data **/
unsigned int decodeseg(unsigned short *buf,
		       RIDFHDSEG segh,struct evtdata *evt){

  //  int ienc;  /* Id for TPC encoder */
  int isz;   /* Segment size - 6 */
  unsigned int rp=0;
  RIDFRHDSEG rhdseg;

  //  unsigned int itmphd,itmpevn,itmpclk;
  
  rhdseg.rhd=ridf_dechd(segh.chd);
  memcpy((char *)&(rhdseg.bsid), (char *)&segh.segid, sizeof(segh.segid));
  isz=rhdseg.rhd.blksize-6;
  
  switch(rhdseg.bsid.module){
  case 32: /******  MADC32  ******************/
    rp=decmadc32(rhdseg,buf,evt);
    break;
  case 21:
    /******  V7xx  ******************/
    rp=decv7xx(rhdseg,buf,evt);
    //    else rp=isz;
    break;
  case 20: /******  V830  ******************/
    rp=decv830(rhdseg,buf,evt);
    break;
  case 24: /******  V1190  ******************/
    rp=decv1190(rhdseg,buf,evt);
    break;
  case 25: /******  V1290  ******************/
    rp=decv1290(rhdseg,buf,evt);
    break;
  case 50: /******  TPC  ******************/
    rp=dectpc(rhdseg,buf,evt);
    break;

  default:
    rp=isz;
    break;
  }

  if(rp!=isz){
    fprintf(stderr,"Wrong data format.\n");
    fprintf(stderr,"mod id= %d\n",rhdseg.bsid.module);
  }
  return(rp);
}


/****************************************************************/
/*  TPC decode routine ******************************************/
/****************************************************************/
unsigned int dectpc(RIDFRHDSEG rhdseg,
		    unsigned short *buf,struct evtdata *evt){
  unsigned int rp=0,isz;
  unsigned int itmphd,itmpevn,itmpclk;
  int ienc;  /* Id for TPC encoder */
  int j;

  ienc=rhdseg.bsid.detector-44;
  isz=rhdseg.rhd.blksize-6;
  while(rp<isz){

    /***** Header Data ******/
    itmphd=ri(buf,&rp);
    itmpevn=ri(buf,&rp);
    itmpclk=ri(buf,&rp);
    evt->tpcclk[ienc][1]=itmpclk&0xfffffff;
    if((itmphd!=1)||((itmpevn>>28)!=4)||((itmpclk>>28)!=05)) {
      fprintf(stderr,"Header (%08x %08x %08x) for imem=%d and icn=%d "
	      "in evt=%d is unreliable.\n",
	      itmphd,itmpevn,itmpclk,
	      ienc/2,ienc%2,
	       itmpevn&0x0fffffff);
      evt->tmb2_bad_header[ienc/2][ienc%2]=1;
    }

    /***** FADC Data ******/
    itmphd=ri(buf,&rp);
    if((itmphd&0xfffff)==FADC_DATA) {
      int iaclk;
      unsigned int itdc;
      if(itmphd!=FADC_DATA){
	fprintf(stderr,"FADC Header  (%08x) for imem=%d and icn=%d "
		"in evt=%d is unreliable, but accept ...\n",
		itmphd,ienc/2,ienc%2,itmpevn&0x0fffffff);
	evt->tmb2_bad_fadc_header[ienc/2][ienc%2]=1;	
      }
      for(iaclk=0;iaclk<N_ACLK;iaclk++){
	itdc=ri(buf,&rp);
	for(j=0;j<4;j++){
	  evt->icadc[ienc/2][(1-ienc%2)*4+j][iaclk]=((itdc>>(24-j*8))&0xff);
	}
      }
      itmphd=ri(buf,&rp); 
    }
    
    /***** TPC Data ******/
    
    if((itmphd&0xfffff)==TPC_DATA){
      int itpcdep,istrp,ichip,iclk,ihitp,isize;
      if(itmphd!=TPC_DATA){
	fprintf(stderr,
		"TPC Header   (%08x) for imem=%d and icn=%d "
		"in evt=%d is unreliable, but accept ...\n",
		itmphd,ienc/2,ienc%2,itmpevn&0x0fffffff);
	evt->tmb2_bad_tpc_header[ienc/2][ienc%2]=1;
      }
      isize=(isz-rp)/2;
      for(itpcdep=0;itpcdep<isize;itpcdep++){
	itmphd=ri(buf,&rp); 
	if((itmphd&0xc0000000)!=0xc0000000){
	  iclk=(itmphd&0x3ff);
	  ihitp=((itmphd>>10)&0xffff);
	  ichip=((itmphd>>26)&0x7);
	  for(j=0;j<16;j++){
	    if(((ihitp>>(15-j))&1)==1){
	      istrp=((1-ienc%2)<<7)+(ichip<<4)+j;
	      evt->imtpc[ienc/2][iclk][istrp]=1;
	      evt->nctpc[ienc/2][istrp]++;
	      if(evt->nctpc[ienc/2][istrp]==1) evt->nstpc[ienc/2]++;
	    }
	  }
	}
	else{
	  if((itmphd&0x1fff)!=itpcdep){
	    fprintf(stderr,
		    "Inconsistent TPC Depth "
		    "(Count:%4x Read:%8x) for imem=%d and icn=%d "
		    "in evt=%d is unreliable.\n",
		    itpcdep,itmphd,
		    ienc/2,ienc%2,
		    itmpevn&0x0fffffff);
	    evt->tmb2_bad_cnt_inconsistent[ienc/2][ienc%2]=1;
	  }
	  break;
	}
      }
      itmphd=ri(buf,&rp); 
    }
    /***** FOOTER Data ******/
    if(itmphd!=FOOTER_DATA){
      fprintf(stderr,"Wrong footer "
	      "(%08x) for imem=%d and icn=%d "
	      "in evt=%d is unreliable.\n",
	      itmphd, ienc/2,ienc%2, itmpevn&0x0fffffff);
      evt->tmb2_bad_footer[ienc/2][ienc%2]=1;
      if((itmphd&0xfffff)!=0xfffff){
	while(rp<isz){
	  if((ri(buf,&rp)&0xfffff)==0xfffff) break;
	}
      }
    }
  }
  return(rp);
}



/****************************************************************/
/*  MADC32 decode routine ***************************************/
/****************************************************************/
unsigned int decmadc32(RIDFRHDSEG rhdseg,
		       unsigned short *buf,struct evtdata *evt){
  int i;
  unsigned int tmpdat;
  unsigned int rp=0,isz;
  int ichan;

  struct bhdmadc32 {
    unsigned ndat  : 12;
    unsigned res   : 3;
    unsigned fmt   : 1;
    unsigned mid   : 8;
    unsigned subhd : 6;
    unsigned hd    : 2;
  } bhd;



  isz=rhdseg.rhd.blksize-6;
  tmpdat=ri(buf,&rp);
  memcpy((char *)&bhd,(char *)&tmpdat,sizeof(bhd));

#ifdef _DEBUG
  printf("  %08x\n",tmpdat);
  printf("  HD %d / SUBHD %d / MODID %d / FMT %d / RES %d / NDAT %d\n",
	 bhd.hd, bhd.subhd, bhd.mid, bhd.fmt, bhd.res, bhd.ndat);
#endif

  for(i=0;i<bhd.ndat-1;i++){
    tmpdat=ri(buf,&rp);
    ichan=(tmpdat>>16)&0x1f;
    evt->assd[ichan]=(tmpdat&0xffff);
    if(((tmpdat>>14)&0x1)==0x1){
      //      printf("MADC32 overflow: ch=%d, val=%d \n", ichan, evt->assd[ichan]);
      evt->assd[ichan]=3840.0;
    }
  }
  // check overflow of ADC (added by cotemba)
  

  tmpdat=ri(buf,&rp);
  if((tmpdat>>30)==0x3){
    evt->madcclk[1]=tmpdat&0x3fffffff;
    //    printf("MADC time=%d\n", evt->madcclk[1]);
  }

  return(rp);
}

/****************************************************************/
/*  V1190A decode routine ***************************************/
/****************************************************************/
unsigned int decv1190(RIDFRHDSEG rhdseg,
		      unsigned short *buf,struct evtdata *evt){
  int i,j;
  unsigned int tmpdat;
  unsigned int rp=0,isz;

  int ichan,idat, lead, trad;
  int ievtcnt,igeo;
  int iwdcnt,fgeo;
  int qtc_ch;
  int ippac,ppac_ch;
  int isi;

  igeo=0;

  isz=rhdseg.rhd.blksize-6;

  for(j=0; j<N_V1190; j++){
    
    while(rp<isz){
      /**** Global Header ***********/
      tmpdat=ri(buf,&rp);
      if((tmpdat&0xf8000000)==0x40000000){ /* G. Header */
	ievtcnt=(tmpdat>>5)&0x3fffff;
	//	printf("V1190 event cnt=%d\n", ievtcnt);
	igeo=tmpdat&0x1f;
      }
      else{
	fprintf(stderr,"V1190:Wrong global header.\n");
      }
      for(i=0;i<4;i++){  /* TDC loop */
	tmpdat=ri(buf,&rp);
	if((tmpdat&0xf8000000)==0x08000000){ /* TDC. Header */
	  int wcnt=1;
	  while(rp<isz){
	    tmpdat=ri(buf,&rp);
	    wcnt++;
	    if((tmpdat&0xf8000000)==0x18000000){ /* TDC Trailer */
	      if(wcnt!=(tmpdat&0xfff)){
		fprintf(stderr,
			"V1190:TDC %d:Inconsistent word count "
			"(count:%d read:%d).\n",i,
			wcnt,(tmpdat&0xfff));
	      }
	      break;
	    }
	    /* TDC data */
	    ichan=(tmpdat>>19)&0x7f;
	    if(igeo==7) ichan+=128;
	    if(igeo==5) ichan+=256;
	    idat=tmpdat&0x3ffff;
	    //	    printf("ichan=%d\n", ichan);
	    if(igeo!=5 && igeo!=6 && igeo!=7) printf("V1190 geo err, geo=%d\n", igeo);
	    
	    /* leading edge */
	    if(((tmpdat>>26) & 0xf3) ==0){
	      lead=idat;
	      evt->v1190_lead[ichan][evt->multi_v1190_lead[ichan]]=idat;
	      evt->hit_v1190[ichan]=1;
	      //	      if(ichan==383) printf(" REF_LE: :%d\n",idat);

	      /* for QTC */
	      //nazo	      if(igeo==5 && ichan>271 && ichan<287){
	      if(igeo==5 && ichan>=320 && ichan<=324){
		qtc_ch=ichan-320;
		evt->qtc_lead[qtc_ch][evt->multi_v1190_lead[ichan]]=idat;
		evt->qtc_hit[qtc_ch]=1;
		//		if(ichan==273) printf(" LE: %d:%d\n",evt->multi_v1190_trad[ichan],idat);
	      }

	      /* for PPAC */
	      if(igeo==5 && ichan>351 && ichan<362){
		ippac=(ichan-352)/5;
		ppac_ch=(ichan-352)%5;
		evt->ppac_lead[ippac][ppac_ch][evt->multi_v1190_lead[ichan]]=idat;
		evt->ppac_hit[ippac][ppac_ch]=1;
	      }

	      /* for Si */
	      if(igeo==5 && ichan-255>5 && ichan-255<10){
		isi=ichan-261;
		evt->si_lead[isi][evt->multi_v1190_lead[ichan]]=idat;
		evt->si_trg_flag=1;
		evt->si_hit[isi]=1;
	      }

	      /* for RF */
	      if(igeo==5 && ichan==RF_CH){
		evt->rf_hit=1;
		evt->rf_lead[evt->multi_v1190_lead[ichan]]=idat;
	      }

	      evt->multi_v1190_lead[ichan]++;

	      if(igeo==5 && ichan>=320 && ichan<=324){
		qtc_ch=ichan-320;
		evt->qtc_multi[qtc_ch]++;
	      }
	    }
	    
	    /* trailing edge */
	    if(((tmpdat>>26) & 0xf3)==1){
	      trad=idat;
	      evt->v1190_trad[ichan][evt->multi_v1190_trad[ichan]]=idat;

	      /* in case of missing leading edge of reference triggers */
	      if(ichan==127 || ichan==255 || ichan== 383){
		evt->hit_v1190[ichan]=1;
		evt->v1190_lead[ichan][evt->multi_v1190_trad[ichan]]=idat;
	      }

	      /* for QTC */
	      if(igeo==5 && ichan>=320 && ichan<=324){
		qtc_ch=ichan-320;
		evt->qtc_trad[qtc_ch][evt->multi_v1190_trad[ichan]]=idat;
		//		if(ichan==273) printf(" TR: %d:%d\n",evt->multi_v1190_trad[ichan],idat);
	      }

	      /* for PPAC */
	      if(igeo==5 && ichan>351 && ichan<362){
		ippac=(ichan-352)/5;
		ppac_ch=(ichan-352)%5;
		evt->ppac_trad[ippac][ppac_ch][evt->multi_v1190_trad[ichan]]=idat;
	      }

	      /* for Si */
	      if(igeo==5 && ichan >=0 && ichan<16){
		isi=ichan;
		evt->si_trad[isi][evt->multi_v1190_trad[ichan]]=idat;
	      }

	      /* for RF */
	      if(igeo==5 && ichan==RF_CH){
		evt->rf_trad[evt->multi_v1190_trad[ichan]]=idat;
	      }

	      evt->multi_v1190[ichan]++;
	      evt->multi_v1190_trad[ichan]++;
	    }

	    //	    if(evt->hit_v1190[ichan]=1) evt->multi_v1190[ichan]++;

	  }
	}
	else{
	  fprintf(stderr,
		  "V1190:TDC %d:Wrong TDC Trailer (%08x).\n",i,tmpdat);
	}
      }
      tmpdat=ri(buf,&rp);
      if((tmpdat&0xf8000000)==0x80000000){ /* Gl Trailer */
	fgeo=tmpdat&0x1f;
	if(fgeo!=igeo)
	  fprintf(stderr,
		"V1190:Wrong GEO in Global trailer "
		  "(H:%d T:%d %08x).\n",igeo,fgeo,tmpdat);
	iwdcnt=(tmpdat>>5)&0xffff;
	//	if(iwdcnt!=isz/2)
	//	  fprintf(stderr,
	//		  "V1190:Wrong WORDCOUNT in Global trailer "
	//		  "(H:%d T:%d %08x).\n",isz/2,iwdcnt,tmpdat);

	break;
      }
      else{
	fprintf(stderr,"V1190:Wrong Global trailer (%08x).\n",tmpdat);
	break;
      }
    } // loop of while(rp<isz) to here
    
  } // N_V1190 loop to here

  for(i=0; i<(int)128*N_V1190; i++){
    if(evt->multi_v1190_lead[i] != evt->multi_v1190_trad[i]){
      //      printf("V1190 # of leading & trailing diff. igeo=%d, ch: %d\n", igeo, i);
      //      printf("lead:%d, trail:%d\n", evt->multi_v1190_lead[i], evt->multi_v1190_trad[i]);
    }
    if(evt->multi_v1190_lead[i] == evt->multi_v1190_trad[i]){

      for(j=0; j<evt->multi_v1190_lead[i]; j++){
	evt->v1190_tot[i][j]=
	  evt->v1190_trad[i][j]-evt->v1190_lead[i][j];

	/* for QTC */
	if(i>=320 && i<=324){
	  qtc_ch=i-320;
	  evt->qtc_tot[qtc_ch][j]=evt->v1190_tot[i][j];
	}

	/* for PPAC */
	if(i>351 && i<362){
	  ippac=(i-352)/5;
	  ppac_ch=(i-352)%5;	  
	  evt->ppac_tot[ippac][ppac_ch][j]=evt->v1190_tot[i][j];
	}

	if(i>=0 && i<16){
	  evt->si_tot[i][j]=evt->v1190_tot[i][j];
	}

	if(i==RF_CH){
	  evt->rf_tot[j]=evt->v1190_tot[i][j];
	}
      }
    }
    
  }

  if(evt->hit_v1190[127]==0) printf("V1190 BDC1  no reference!!\n");
  if(evt->hit_v1190[255]==0) printf("V1190 BDC2  no reference!!\n");
  if(evt->hit_v1190[383]==0) printf("V1190 MAIKo no reference!!\n");


//  /* random generator */
//  for(i=10; i<256; i+=16){
//    evt->hit_v1190[i]=1;
//    evt->v1190_lead[i][0]=(int)(rand()%2500)+2000;
//    //    evt->v1190_lead[i][0]=3000;
//    evt->multi_v1190[i]=1;
//  }

  return(rp);
}

/****************************************************************/
/*  V1290A decode routine ***************************************/
/****************************************************************/
unsigned int decv1290(RIDFRHDSEG rhdseg,
		      unsigned short *buf,struct evtdata *evt){

  unsigned int tmpdat;
  unsigned int rp=0,isz;

  int i, j;
  int ichan,idat, lead, trad;
  int ievtcnt;
  int igeo;
  int fgeo;
  int iwdcnt;

  igeo=0;
  isz=rhdseg.rhd.blksize-6;

    while(rp<isz){
      /**** Global Header ***********/
      tmpdat=ri(buf,&rp);
      if((tmpdat&0xf8000000)==0x40000000){ /* G. Header */
	ievtcnt=(tmpdat>>5)&0x300000;
	igeo=tmpdat&0x1f;
      }
      else{
	fprintf(stderr,"V1290:Wrong global header.\n");
      }
      for(i=0;i<4;i++){  /* TDC loop */
	tmpdat=ri(buf,&rp);
	if((tmpdat&0xf8000000)==0x08000000){ /* TDC. Header */
	  int wcnt=1;
	  while(rp<isz){
	    tmpdat=ri(buf,&rp);
	    wcnt++;
	    if((tmpdat&0xf8000000)==0x18000000){ /* TDC Trailer */
	      if(wcnt!=(tmpdat&0xfff)){
		fprintf(stderr,
			"V1290:TDC %d:Inconsistent word count "
			"(count:%d read:%d).\n",i,
			wcnt,(tmpdat&0xfff));
	      }
	      break;
	    }

	    /* TDC data */
	    ichan=(tmpdat>>21)&0x1f;
	    idat=tmpdat&0x1fffff;

	    //	    printf("v1290: ichan=%d, idat=%d\n", ichan, idat);
	    
	    /* leading edge */
	    if(((tmpdat>>26) & 0xf3) ==0){
	      lead=idat;
	      evt->v1290_lead[ichan][evt->multi_v1290_lead[ichan]]=idat;
	      evt->hit_v1290[ichan]=1;
	      evt->multi_v1290_lead[ichan]++;
	    }
	    
	    /* trading edge */
	    if(((tmpdat>>26) & 0xf3)==1){
	      trad=idat;
	      evt->v1290_trad[ichan][evt->multi_v1290[ichan]]=idat;
	      //	      evt->v1290_tot[ichan][evt->multi_v1290[ichan]]=
	      //		evt->v1290_trad[ichan][evt->multi_v1290[ichan]] -
	      //		evt->v1290_lead[ichan][evt->multi_v1290[ichan]];	      
	      evt->multi_v1290[ichan]++;
	      evt->multi_v1290_trad[ichan]++;
	    }

	  }
	}
	else{
	  fprintf(stderr,
		  "V1290:TDC %d:Wrong TDC Trailer (%08x).\n",i,tmpdat);
	}
      }
      tmpdat=ri(buf,&rp);
      if((tmpdat&0xf8000000)==0x80000000){ /* Gl Trailer */
	fgeo=tmpdat&0x1f;
	if(fgeo!=igeo)
	  fprintf(stderr,
		"V1290:Wrong GEO in Global trailer "
		  "(H:%d T:%d %08x).\n",igeo,fgeo,tmpdat);
	iwdcnt=(tmpdat>>5)&0xffff;
	//	if(iwdcnt!=isz/2)
	//	  fprintf(stderr,
	//		  "V1290:Wrong WORDCOUNT in Global trailer "
	//		  "(H:%d T:%d %08x).\n",isz/2,iwdcnt,tmpdat);

	break;
      }
      else{
	fprintf(stderr,"V1290:Wrong Global trailer (%08x).\n",tmpdat);
	break;
      }
    } // loop of while(rp<isz) to here


    for(i=0; i<(int)32*N_V1290; i++){
      if(evt->multi_v1290_lead[i] != evt->multi_v1290_trad[i]){
	//	printf("V1290 # of leading & trailing is different. ch=%d\n", i);
	//	printf("lead:%d, trail:%d\n", evt->multi_v1290_lead[i], evt->multi_v1290_trad[i]);
      }
      if(evt->multi_v1290_lead[i] == evt->multi_v1290_trad[i]){
	for(j=0; j<(evt->multi_v1290_lead[i]); j++){
	  evt->v1290_tot[i][j] = evt->v1290_trad[i][j] - evt->v1290_lead[i][j];
	}
      }
    }
    
  return(rp);
}


/****************************************************************/
/*  V7xx  decode routine ***************************************/
/****************************************************************/
unsigned int decv7xx(RIDFRHDSEG rhdseg,
		     unsigned short *buf,struct evtdata *evt){
  unsigned int tmpdat;
  unsigned int rp=0,isz;

  int ichan,idat;
  int ievtcnt,icrate,igeo;

  //  printf("V775 decode start\n");

  isz=rhdseg.rhd.blksize-6;
  while(rp<isz){
    /**** Header ***********/
    tmpdat=ri(buf,&rp);
    if((tmpdat&0x07000000)==0x02000000){ /* Header */
      ievtcnt=(tmpdat>>8)&0x3f;
      icrate=(tmpdat>>16)&0xff;
      igeo=(tmpdat>>27)&0x1f;
#ifdef _DEBUG
      printf("   V7xx:HD:%08x GEO %d / CRATE %d / CNT %d\n",
	     tmpdat,igeo,icrate,ievtcnt);
#endif
    }
    else{
      fprintf(stderr,"V7xx:Wrong global header (%08x).\n",tmpdat);
    }
    /**** DATA ***********/
    tmpdat=ri(buf,&rp);
    while((tmpdat&0x07000000)==0) { /* if word format */
      ichan=(tmpdat>>16)&0x1f;
      idat=(tmpdat&0xfff);
      //      evt->tssd[ichan]=idat; // for V775
      evt->assd[ichan]=idat; // for V792

#ifdef _DEBUG
      printf("   V7xx:DT:%08x GEO %d / CH %d / UN %d / OV %d / ADC %d\n",
	     tmpdat,(tmpdat>>27)&0x1f,ichan,(idat>>13)&0x1, (idat>>12)&0x1, 
	     idat&0xfff);
#endif
      tmpdat=ri(buf,&rp);
    }
    /**** Trailer ***********/
    if((tmpdat&0x07000000)==0x04000000){ /* Trailer */
      ievtcnt=(tmpdat>>8)&0x3f;
      icrate=(tmpdat>>16)&0xff;
      igeo=(tmpdat>>27)&0x1f;
#ifdef _DEBUG
      printf("   V7xx:TR:%08x GEO %d / EVENTNO %d\n",
	     tmpdat,(tmpdat>>27)&0x1f,tmpdat&0xffffff);
#endif
    }
    else{
      fprintf(stderr,"V7xx:Wrong End of event (%08x).\n",tmpdat);
      rp=isz;
      break;
    }
  }
  return(rp);
}


/****************************************************************/
/*  V830 decode routine ***************************************/
/****************************************************************/
unsigned int decv830(RIDFRHDSEG rhdseg,
		     unsigned short *buf,struct evtdata *evt){
  int i;unsigned int tmpdat;
  unsigned int rp=0,isz;

  isz=rhdseg.rhd.blksize-6;

  if(isz!=64) {
    fprintf(stderr,"V830: Wrong data length (size=%d).\n",rhdseg.rhd.blksize);
    rp=isz;
  }
  else{
    for(i=0;i<32;i++){
      tmpdat=ri(buf,&rp);
      evt->scv830[i]=tmpdat;
#ifdef _DEBUG
      printf("   V830:CH %02d   %8d\n",i,tmpdat);
#endif
    }
  }
  return(rp);
}


/************** junk ***************************************************/
#if 0  
    ienc=bsid.detector-44;
    while(rp<isz){

      /***** Header Data ******/
      itmphd=ri(buf,&rp);
      itmpevn=ri(buf,&rp);
      itmpclk=ri(buf,&rp);
      if((itmphd!=1)||((itmpevn>>28)!=4)||((itmpclk>>28)!=05)) {
	fprintf(stderr,"TPC Header (%08x %08x %08x) for imem=%d and icn=%d "
	       "in evt=%d is unreliable.\n",
	       itmphd,itmpevn,itmpclk,
	       ienc/2,ienc%2,
	       itmpevn&0x0fffffff);
      }

      /***** FADC Data ******/
      itmphd=ri(buf,&rp);
      if((itmphd&0xfffff)==FADC_DATA) {
	int iaclk,j;
	unsigned int itdc;
	if(itmphd!=FADC_DATA){
	  fprintf(stderr,"FADC Header  (%08x) for imem=%d and icn=%d "
		 "in evt=%d is unreliable, but accept ...\n",
		 itmphd,ienc/2,ienc%2,itmpevn&0x0fffffff);
	}
	for(iaclk=0;iaclk<N_ACLK;iaclk++){
	  itdc=ri(buf,&rp);
	  for(j=0;j<4;j++){
	    evt->icadc[ienc/2][(1-ienc%2)*4+j][iaclk]=((itdc>>(24-j*8))&0xff);
	  }
	}
	itmphd=ri(buf,&rp); 
      }

      /***** TPC Data ******/

      if((itmphd&0xfffff)==TPC_DATA){
	int itpcdep,istrp,ichip,iclk,ihitp,isize;
	if(itmphd!=TPC_DATA){
	  fprintf(stderr,
		  "TPC Header   (%08x) for imem=%d and icn=%d "
		  "in evt=%d is unreliable, but accept ...\n",
		  itmphd,ienc/2,ienc%2,itmpevn&0x0fffffff);
	}
	isize=(isz-rp)/2;
	for(itpcdep=0;itpcdep<isize;itpcdep++){
	  itmphd=ri(buf,&rp); 
	  if((itmphd&0xc0000000)!=0xc0000000){
	    iclk=(itmphd&0x3ff);
	    ihitp=((itmphd>>10)&0xffff);
	    ichip=((itmphd>>26)&0x7);
	    for(j=0;j<16;j++){
	      if(((ihitp>>(15-j))&1)==1){
		istrp=((1-ienc%2)<<7)+(ichip<<4)+j;
		evt->imtpc[ienc/2][iclk][istrp]=1;
		evt->nctpc[ienc/2][istrp]++;
		if(evt->nctpc[ienc/2][istrp]==1) evt->nstpc[ienc/2]++;
	      }
	    }
	  }
	  else{
	    if((itmphd&0x1fff)!=itpcdep){
	      fprintf(stderr,
		      "Inconsistent TPC Depth "
		      "(Count:%4x Read:%8x) for imem=%d and icn=%d "
		      "in evt=%d is unreliable.\n",
		      itpcdep,itmphd,
		      ienc/2,ienc%2,
		      itmpevn&0x0fffffff);
	    }
	    break;
	  }
	}
	itmphd=ri(buf,&rp); 
      }
      /***** FOOTER Data ******/
      if(itmphd!=FOOTER_DATA){
	fprintf(stderr,"Wrong footer "
	       "(%08x) for imem=%d and icn=%d "
	       "in evt=%d is unreliable.\n",
	       itmphd, ienc/2,ienc%2, itmpevn&0x0fffffff);
	if((itmphd&0xfffff)!=0xfffff){
	  while(rp<isz){
	    if((ri(buf,&rp)&0xfffff)==0xfffff) break;
	  }
	}
      }
    }
#endif
/************** junk ***************************************************/

