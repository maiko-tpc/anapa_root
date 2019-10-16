/* babirl : devtool/monan.c
 * last modified : 07/01/18 14:37:31 
 * 
 * Hidetada Baba (RIKEN)
 * baba@ribf.riken.jp
 *
 * Monitor babian shared memory
 *
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ioctl.h>
#include <fcntl.h>

/* babirl */
#include <libbabirl.h>
#include <bi-config.h>
#include <bi-common.h>
#include <ridf.h>

/* anapa */
#include "anapa.h"
#include "myana.hpp"

/* cfortran.h */
//** #include "cfortran.h"
//** #include "hbook.h"

//** #define PAWC COMMON_BLOCK(PAWC,pawc)
//#define PAWC_SIZE 3355443
//** #define PAWC_SIZE 10000000
//** typedef struct { float PAW[PAWC_SIZE]; } PAWC_DEF;

//** COMMON_BLOCK_DEF(PAWC_DEF,PAWC);
//** PAWC_DEF PAWC;

//#define _DEBUG 1

char comname[L_LINE];
char gname[L_LINE];
char cidname[32][100]={
  "RIDF_EF_BLOCK",  "RIDF_EA_BLOCK",  "RIDF_EAF_BLOCK",  "RIDF_EVENT",
  "RIDF_SEGMENT",  "RIDF_COMMENT",  "RIDF_EVENT_TS",  "",
  "RIDF_BLOCK_NUMBER",  "RIDF_END_BLOCK",  "",  "RIDF_NCSCALER",
  "RIDF_CSCALER24",  "RIDF_NCSCALER32",  "",  "", 
  "RIDF_TIMESTAMP","","","",
  "","IDF_STATUS","","",
  "","","","",
  "","","",""};


unsigned int decodeseg(unsigned short *tbuf,
		       struct ridf_hdst_seg idseg, struct evtdata *evt);

unsigned short *buf; /* Buffer for block data */
int flag; /* option flag */
int fdin; /* File pointer to input data file */
struct evtdata *evtg;  /* Pointer to Event data (evt)*/

unsigned short *bufdst; /* Buffer for DST data */
int fdst; /* File pointer to the DST file */
unsigned int rpdst;  /* Read pointer for the DST file */
unsigned int wpdst;  /* Write pointer for the DST file */

unsigned int ri(unsigned short *lbuf, unsigned int *lrp){
  unsigned int idat;
  idat=*(int *)(lbuf+*lrp);
  *lrp+=2;
  return(idat);
}


int iitmp=0;

int main(int argc, char *argv[]){
  int i,j,k;
  struct sigaction act;
  char hbfile[100];
  //  char null[L_LINE];

  RIDFHD hd;
  RIDFRHD rhdl[3]; /* RIDFRHD for Layer 0, 1, 2 */
  //  struct stbsegid bsid;

  struct sembuf semb;
  union semun semunion;
  char *shmp, *fshmp;
  unsigned int blocknum, tblocknum;
  int shmid, semid;
  //  unsigned short sval;

  /* Variables for PAPA */
  unsigned int rp;     /* Read pointer */
  unsigned int sz[3];  /* Size for Layer 0, 1, 2 */
  struct evtdata evt;  /* Event data */
  struct enparm parm={
    /* ppac_ch_t[3][5] */
    /* [0][] ... F2U, [1][] ... F2D, [2][]...F3  */
    /* [0...4]  LRUDA */
    {{0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1}},

    /* ppac_t_mm[3][5] */
    /* [0][] ... F2U, [1][] ... F2D, [2][]...F3  */
    /* [0...3] X slope, Y slope, X offset, Y offset LDRUAb */
    {{0.63419, 0.62479, -7.262, -4.923},
     {0.62691,0.62147, -2.8068, 0.63758},
     {0.62691,0.62147, -2.8068, 0.63758}},

    /* ppac_geo[3][2] */
    /* [0][] ... F2U, [1][] ... F2D, [2][]...F3  */
    /* [][0] ... X, [][1] ... Y  */
    {{0., 0}, {0., 0}, {0., 0}},

    /* Z position */
    {0, 482, 88, 630, 755, 1360, 1536.6, 1639, 2261.45}

    /* [0] .. F2PPACU */
    /* [1] .. F2PPACD */
    /* [2] .. F2PL    */
    /* [3] .. F2GV    */
    /* [4] .. F2Viewer */
    /* [5] .. TPC Chamber */
    /* [6] .. TPC Center */
    /* [7] .. Si */
    /* [8] .. F3PPAC */
    /* EN parameters */
  };

  time_t *time;
  int first_verbose = 1;

  int lreq=1024,istat;

  evt.nev=0;
  evt.tgtnev=1;
  evt.madcclk[0]=0;
  evt.madcclk[1]=0;
  evt.nblk=0;
  evt.iflana=0;
  evtg=&evt;

  /* add on 16/09/14 for baseline subtraction */
  //Anode
  evt.baseline_fadc[0][0] = 32;
  evt.baseline_fadc[0][1] = 32;
  evt.baseline_fadc[0][2] = 31;
  evt.baseline_fadc[0][3] = 30;
  evt.baseline_fadc[0][4] = 30;
  evt.baseline_fadc[0][5] = 26;
  evt.baseline_fadc[0][6] = 29;
  evt.baseline_fadc[0][7] = 27;
  //Cathode
  evt.baseline_fadc[1][0] = 180;
  evt.baseline_fadc[1][1] = 177;
  evt.baseline_fadc[1][2] = 181;
  evt.baseline_fadc[1][3] = 178;
  evt.baseline_fadc[1][4] = 167;
  evt.baseline_fadc[1][5] = 174;
  evt.baseline_fadc[1][6] = 171;
  evt.baseline_fadc[1][7] = 169;

  evt.fadc_th[0]=15.0;
  evt.fadc_th[1]=15.0;
  evt.fadc_th[2]=15.0;
  evt.fadc_th[3]=15.0;
  evt.fadc_th[4]=15.0;
  evt.fadc_th[5]=15.0;
  evt.fadc_th[6]=15.0;
  evt.fadc_th[7]=15.0;

  parm.ppac_geo[0][0]=-1.7438+3.93-1.64;
  parm.ppac_geo[0][1]=0.5954;
  parm.ppac_geo[1][0]=-1.3177+4.635-3.68;
  parm.ppac_geo[1][1]=-0.38158;
  

  for(i=0;i<4;i++) evt.tpcclk[i][1]=0;

  strcpy(comname,argv[0]);
  flag=startup(argc,argv,&fdin,hbfile,&fdst);
  act.sa_handler=file_end;
  sigaction(SIGINT,&act,NULL);

  for(i=0;i<N_SCA;i++) {
    evt.isca[i]=0;
    evt.scv830[i]=0;
  }

  /* BDC parameter setting */
  for(i=0; i<N_BDC; i++){
    for(j=0; j<BDC_PLANE; j++){
      for(k=0; k<N_CALI_PARM; k++){
	  evt.bdc_parm[i][j][0][k]=1000000;
	  evt.bdc_parm[i][j][1][k]=10;
      }
    }
  }

  char fname[256];
  FILE *fp;
  printf("BDC_PLANE=%d \n", BDC_PLANE);
  for(i=0; i<N_BDC; i++){
    for(j=0; j<BDC_PLANE; j++){
      sprintf(fname, "./calib/bdccalib-%d.dat", 10*i+j);
      fp=fopen(fname, "r");

      printf("BDC calibration file: %s\n", fname);

      if(fp==NULL){
	printf("BDC calibration data does not exist!!!,  %s\n", fname);
      }
      
      else{
	for(k=0; k<N_CALI_PARM; k++){
	  fscanf(fp, "%f %f", 
		 &evt.bdc_parm[i][j][0][k], &evt.bdc_parm[i][j][1][k]);
	  if(feof(fp)==1) break;
	}
	fclose(fp);
      }
    }
  }

  /** Define Histgram **************/
//**   if(flag&0x8){ /* if online option is given */
//**     HLIMAP(PAWC_SIZE,hbfile);
//**     printf("Global section: %s\n",hbfile);
//**   }
//**   else{
//**     HLIMIT(PAWC_SIZE);
//**     HROPEN(1,"TEST",hbfile,"N",lreq,istat);
//**   }
//**   histdef();

  // changed by DOI (2019/9/21)
  MyAna::GetInstance().MakeFile(hbfile);
  MyAna::GetInstance().TreeDef();
  MyAna::GetInstance().Init();

  /** Open data stream **************/
  blocknum = 0;
  if(flag&0x10){ /* if online option is given */
    /* Shared Memory */
    if(!(shmid = initshm(ANSHMKEY,
			 EB_EFBLOCK_BUFFSIZE * WORDSIZE + sizeof(blocknum),
			 &shmp))){
      printf("Can't create shared memory\n");
      exit(0);
    }
    fshmp = shmp + EB_EFBLOCK_BUFFSIZE * WORDSIZE;

    /* Semaphore */
    if(!(semid = initsem(ANSEMKEY, &semunion))){
      printf("Can't create semaphore\n");
      exit(0);
    }
  }
  buf=(short unsigned int*)malloc(EB_EFBLOCK_BUFFSIZE * WORDSIZE + sizeof(blocknum));


  while(1){
    /** Layer 0 ****/
    if(flag&0x10){ /* if online option is given */
      sem_p(semid, &semb);     // Lock shared memory
      memcpy((char *)&tblocknum, fshmp, sizeof(blocknum));
      if(blocknum != tblocknum){
	blocknum = tblocknum;
	memcpy((char *)&hd, shmp, sizeof(hd));
      }
      else{
	sleep(1);
	sem_v(semid, &semb);     // Unlock shared memory
	continue;
      }
    }
    else { /* if offline analysis */
      int readbyte;
      readbyte=read(fdin,(char *)buf,sizeof(hd));
#ifdef _DEBUG
      printf("Access data: read %d bytes\n",readbyte);
#endif
      if(readbyte==0) file_end(readbyte);
      memcpy((char *)&hd, (char *)buf, sizeof(hd));      
    }

    rhdl[0] = ridf_dechd(hd);

#ifdef _DEBUG
    char null[10];
    printf("\n%s\n",cidname[rhdl[0].classid]);
    printf("Ly0: %08x  %08x\n",*(int *)buf,*(int *)(buf+2));
    printf("ly=%d, cid=%d, size=%d, efn=%d\n",
	   rhdl[0].layer, rhdl[0].classid, rhdl[0].blksize, rhdl[0].efn);
    fgets(null,10,stdin);
#endif

    if(flag&0x10){ /* if online option is given */
      memcpy((char *)buf,shmp,(rhdl[0].blksize)*2);
      sem_v(semid, &semb);     // Unlock shared memory
    }
    else{
      int readbyte;
      if(flag&0x01){ /* if dst file is made. */
	bufdst=(short unsigned int*)malloc(rhdl[0].blksize*2);
	memcpy((char *)bufdst, (char *)buf, sizeof(hd));      
      }
      readbyte=read(fdin,(char *)(buf+4),(rhdl[0].blksize)*2-8);
      wpdst=4;
#ifdef _DEBUG
      printf("Access data: read %d bytes\n",readbyte);
#endif
    }

    rp=4;
    sz[0]=rhdl[0].blksize;
    /** Layer 1 ***/
    while(rp<sz[0]){
      evt.iflana&=0xfffffffd;
      memcpy((char *)&hd,(char *)(buf+rp),sizeof(hd));
      rpdst=rp;
      rp+=4;
      rhdl[1]=ridf_dechd(hd);
      sz[1]=rp+(rhdl[1].blksize)-4;

#ifdef _DEBUG
      printf(" %s\n",cidname[rhdl[1].classid]);
      printf(" Ly1: %08x  %08x  %08x  %08x\n",hd.hd1,hd.hd2,RINT(buf[rp]),
	     RINT(buf[rp+2])
	     );
      printf(" ly=%d, cid=%d, size=%d, efn=%x\n",
	     rhdl[1].layer, rhdl[1].classid, rhdl[1].blksize, rhdl[1].efn);
#endif

      switch(rhdl[1].classid){
      case RIDF_COMMENT:
//	evt.start_time = (time_t)buf[rp];
	time = (time_t *)&buf[rp];
	printf(" Date: %s", ctime(time));
//	printf(" Date: %s",ctime((time_t *)&buf[rp]));
	evt.start_time = *time;
//	printf("%d\n", evt.start_time);
	rp+=2;
	printf(" Comment ID = %d\n ",ri(buf,&rp));
	while(rp<sz[1]){
	  printf("%c",*((char *)(buf+rp)));
	  printf("%c",*((char *)(buf+rp)+1));
	  rp++;
	}
	printf("\n");
	break;
      case RIDF_END_BLOCK:
#ifdef _DEBUG
	printf(" Size of block: Header:%d  rp:%d  Ender:%d\n",
	       sz[1],rp+2,RINT(buf[rp]));
	fgets(null,10,stdin);
#endif
	rp+=2;
	break;
      case RIDF_BLOCK_NUMBER:
	evt.iblk=ri(buf,&rp);
	if(evt.nblk==0){
	  evt.nblkoffset=evt.iblk;
	}
	evt.nblk++;
#ifdef _DEBUG
	//	printf(" Block Number:%d (Analyzed block:%d)\n",
	//	       evt.iblk,evt.nblk);
	fgets(null,10,stdin);
#endif

#ifdef _DEBUG
	if(evt.nblk%256==255 || evt.nblk==1)
	  printf(" Block Number:%d (Analyzed block:%d event:%d)\n",
		 evt.iblk,evt.nblk,evt.nev);
#endif	
	break;

      case RIDF_NCSCALER32:
	rp+=2; /* skip Date */	  
	rp+=2; /* skip Scaler ID */	  
	i=0;
	while(rp<sz[1]) {
	  evt.isca[i++]=ri(buf,&rp);
	  if(i>N_SCA){
	    fprintf(stderr,"%s: Scaler channel must be less than N_SCA.\n",
		    comname);
	    rp=sz[1];
	  }
	}
	break;
      case RIDF_EVENT:
	/** Initialize event *******/
#ifdef _DEBUG
	printf("%08x  %08x\n",RINT(buf[rp]),RINT(buf[rp+2]));
#endif
	initevt(&evt);
	evt.iev=ri(buf,&rp);
	if(evt.nev==0) evt.nevoffset=evt.iev;
	evt.nev++;

	/*
	if(evt.nev!=evt.iev){
	  fprintf(stderr,"%s:Inconsistent event number "
		  "count:%d  header:%d\n",comname,evt.nev,RINT(buf[rp-2]));
		  }*/
#ifdef _DEBUG
	printf("  Event Number:%d  %08x\n",evt.nev,RINT(buf[rp-2]));
#endif
	while(rp<sz[1]){
	  RIDFHDSEG segh;
	  memcpy((char *)&hd,(char *)(buf+rp),sizeof(hd));
	  rp+=4;
	  rhdl[2]=ridf_dechd(hd);
	  sz[2]=rp+rhdl[2].blksize-4;
#ifdef _DEBUG
	  printf("  Ly2: %08x  %08x\n",hd.hd1,hd.hd2);
	  printf("  ly=%d, cid=%d, size=%d, efn=%d\n",
		 rhdl[2].layer, rhdl[2].classid, rhdl[2].blksize, rhdl[2].efn);
#endif
	  /** Layer 2 ***/
	  switch(rhdl[2].classid){
	  case RIDF_SEGMENT:
	    segh.chd=hd;
	    segh.segid=ri(buf,&rp);
#ifdef _DEBUG
	    memcpy((char *)&bsid, (char *)&segh.segid, sizeof(segh.segid));
	    printf("  Rev %d / Dev %d / FP  %d / Det %d / Mod %d\n",
		   bsid.revision, bsid.device, bsid.focal,
		   bsid.detector, bsid.module);
#endif
	    decodeseg(buf+rp,segh,&evt);
	    rp=sz[2];
	    break;
	  default:
	    printf("%s:Unknown class ID (%d) in Layer 2.\n",
		   comname,rhdl[2].blksize);
	    rp=sz[2];
	    break;
	  }
	}
	/** End of Event Analyze here ***/
	int footer_bad = 0;
	int spark = 0;
	for(i = 0; i < 2; i++){
	  for(j = 0; j < 2; j++){
	    if(evt.tmb2_bad_footer[i][j]==1){
	      footer_bad++;
	      break;
	    }
	  }
	}
	if(footer_bad == 0){
//** 	  analysis(&evt,&parm);
	  //	  if(evt.spark == 0){
//** 	  booking(&evt);
  // changed by DOI (2019/9/21)
	  MyAna::GetInstance().Analysis(&evt);
	  MyAna::GetInstance().TreeFill();
	  MyAna::GetInstance().AutoSave();
	    //	  }
	}
	
//	if(evt.nev%1000 == 0){
//	  printf("Analyzed event: %d \n",evt.nev);
//	}
        /* changed by DOI (2019/10/12) below */
	if(evt.nev%100 == 0){
	  if(!first_verbose){
	    printf("\r\033[%dA", N_SCA/2+2);
	  }
	  first_verbose = 0;
	  int words;
	  printf("0 ");
	  for(words=0;words!=2*((evt.nev%1000)/100);++words){
	    printf("=");
	  }
	  printf(">");
	  ++words;
	  for(;words!=20;++words){
	    printf(".");
	  }
	  printf(" 1000\n");
	  printf("SCALAR:\n");
	  for(j=0;j<N_SCA/2;j++){
	    printf("SCA %2x:%10d       SCA %2x:%10d\n",j,evtg->isca[j],
		   j+N_SCA/2,evtg->isca[j+N_SCA/2]);
	  }
	}
	/* changed by DOI (2019/10/12) above

	/*** Sample event selection *************************/
	if(evt.nev%2 == 1){
	  //	  evt.iflana|=0x2;
	}
	/*** Sample event selection *************************/

	/*********************************/
	if((evt.nev>=evt.tgtnev)||(evt.iev>=evt.tgtiev)||(evt.iflana&1)){
	  printf("  Block: %d th  Event: %d th\n"
		 "  (Analyzed Block: %d  Event: %d)\n",
		 evt.iblk,evt.iev,evt.nblk,evtg->nev);
	  printf("\n\n");
	  evt.iflana&=0xfffffffe;
	  if(!first_verbose){                      // changed by DOI (2019/10/12)
	    printf("\r\033[%dA\033[K", N_SCA/2+7); // changed by DOI (2019/10/12)
	  }                                        // changed by DOI (2019/10/12)
	  setnextev(&evt);
	}
	break;
      }
      if((flag&0x11)==0x1){ /* if dst will be made */
	if((evt.iflana&0x2)!=0x2){
	  memcpy((char *)(bufdst+wpdst),(char *)(buf+rpdst),(rp-rpdst)*2);
	  wpdst+=(rp-rpdst);
#ifdef _DEBUG
	  printf("  Store this Ly1 data in the DST file (%d/%d).\n",
		 iitmp,evt.iev);
#endif
	}
      }
    }
    if((flag&0x11)==0x1){ /* if dst will be made */
	  printf("hoge\n");
      *((int *)bufdst)&=0xfc000000;
      *((int *)bufdst)+=wpdst;
      //      printf("After: %08x\n",*((int *)bufdst));
      write(fdst,bufdst,wpdst*2);
      free(bufdst);
      bufdst=NULL;
    }

  }
  // changed by DOI (2019/9/21)
  //  MyAna::GetInstance().CloseFile();

  return 0;
}


void file_end(int i){
  int istat,j;
  free(buf);
  if((flag&0x10)==0){ /* if offline */
    close(fdin);
  }
  if((flag & 0x11)==0x1){ /* DST table was made. */
    if(bufdst!=NULL){
      *((int *)bufdst)&=0xfc000000;
      *((int *)bufdst)+=wpdst;
      write(fdst,bufdst,wpdst*2);
      free(bufdst);
    }
    close(fdst);
  }
//**   if((flag&0x8)==0){ /* if hb file is created */
//**     HROUT(0,istat," ");
//**     HREND("TEST");
//**   }

//  printf("\nSCALAR:\n");
//  for(j=0;j<N_SCA/2;j++){
//    printf("SCA %2x:%10d       SCA %2x:%10d\n",j,evtg->isca[j],
//	   j+N_SCA/2,evtg->isca[j+N_SCA/2]);
//  }

  /*
    printf("V830 SCALAR:\n");
    for(j=0;j<N_SCA/2;j++){
    printf("V830 SCA %2x:%10d       SCA %2x:%10d\n",j,evtg->scv830[j],
    j+N_SCA/2,evtg->scv830[j+N_SCA/2]);
	   }*/
  MyAna::GetInstance().AutoSave();
  MyAna::GetInstance().CloseFile();

  printf("\033[%dB\r", N_SCA/2+6); // chaned by DOI (2019/10/12)
  printf("Read Block: %d   Analyzed Block: %d  Analyzed Event: %d\n",
	 evtg->iblk-evtg->nblkoffset+1,evtg->nblk,evtg->nev);
  exit(0);
}
