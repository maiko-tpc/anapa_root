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
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>


/* babirl */
#include <bi-config.h>
#include <bi-common.h>
#include <ridf.h>

#define N_SCA 32

void file_end(FILE *,unsigned int *);
char comname[400];


int main(int argc, char *argv[]){
  int i,j,k;
  int flag; /* option flag */
  struct sigaction act;
  char hbfile[100];
  FILE fdin;

  RIDFHD hd;
  RIDFRHD rhd;
  struct sembuf semb;
  union semun semunion;
  char *shmp, *fshmp;
  unsigned int blocknum, tblocknum;
  int shmid, semid, i;
  unsigned short sval;


  /* Variables for PAPA */
  unsigned int rp[4];  /* Read pointer for Layer 0, 1, 2, 3*/
  unsigned int sz[4];  /* Size for Layer 0, 1, 2, 3 */
  unsigned int *buf; /* Buffer for block data */
  RIDFRHD rhdl[4]; /* RIDFRHD for Layer 0, 1, 2, 3 */

  unsigned int isca[N_SCA];  /* Scaler data */
  unsigned int nev=0; /* Number of event */


  strcpy(comname,argv[0]);
  flag=statup(argc,argv,&fdin,hbfile);
  act.sa_handler=file_end;
  sigaction(SIGINT,&act,NULL);

  for(i=0;i<N_SCA;i++) isca[i]=0;
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


  buf=malloc(EB_EFBLOCK_BUFFSIZE * WORDSIZE + sizeof(blocknum));

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
	continue;
      }
    }
    else { /* if offline analysis */
      int readbyte;
      readbyte=read(fdin,(char *)buf,sizeof(hd));
    }

    rhd = ridf_dechd(hd);
    printf("ly=%d, cid=%d, size=%d, efn=%d\n",
	   rhd.layer, rhd.classid, rhd.blksize, rhd.efn);         

    rhdl[0] = ridf_dechd(hd);

    if(flag&0x10){ /* if online option is given */
      memcpy((char *)buf,shmp,rhd.blksize*2);
      sem_v(semid, &semb);     // Unlock shared memory
    }
    else{
      int readbyte;
      readbyte=read(fdin,(char *)(buf+2),rhd.blksize*2-8);
    }

    rp=2;
    sz[0]=rhd.blksize/2;
    /** Layer 1 ***/
    while(rp<sz){
      hd.hd1=buf[rp++];
      hd.hd2=buf[rp++];
      rhdl[1]=ridf_dechd(hd);
      sz[1]=rp+rhdl[1].blksize/2-2;
      switch(rhd[1].classid){
      case RIDF_COMMENT:
	printf("Date: %s\n",ctime((time_t *)&(buf[rp++])));
	printf("Comment ID = %d\n",buf[rp++]);
	while(rp<sz[1]){
	  printf("%c",&(char *)(buf+rp));
	  printf("%c",&(char *)(buf+rp)+1);
	  printf("%c",&(char *)(buf+rp)+2);
	  printf("%c",&(char *)(buf+rp)+3);
	  rp++
	    }
	break;
      case RIDF_END_BLOCK:
	printf("Size of block: Header:%d  rp:%d  Ender:%d\n",
	       sz[1],rp+1,buf[rp++]);
	break;
      case RIDF_BLOCK_NUMBER:
	printf("Block Number:%d\n",
	       buf[rp++]);
	break;
      case RIDF_NCSCALER32:
	rp++; /* skip Date */	  
	rp++; /* skip Scaler ID */	  
	i=0;
	if(sz[1]==36){
	  while(rp<sz[1]) isca[i++]+=buf[rp++];
	}
	else{
	  fprintf(stderr,"%s: Scaler channel must be less than 32.\n",
		  comname);
	}
	break;
      case RIDF_EVENT
	if(nev!=buf[rp++]){
	  fprintf(stderr,"%c:Inconsistent event number "
		  "count:%d  header:%d\n",nev,buf[rp-1]);
	}
      while(rp<sz[1]){
	hd.hd1=buf[rp++];
	hd.hd2=buf[rp++];
	rhdl[2]=ridf_dechd(hd);
	sz[2]=rp+rhdl[2].blksize/2-2;
	/** Layer 2 ***/
	switch(rhdl[2].classid){
	case RIDF_SEGMENT:
	  RIDFHDSEG segh;
	  segh.chd=hd;
	  segh.segid=buf[rp++];
	  rp=sz[2];
	  break;
	default:
	  printf("%s:Unknown class ID (%d) in Layer 2.\n",
		 comname,rhdl[2].blksize);
	  rp=sz[2];
	  break;
	}
      }
      nev++;
      break;
      }
    }
  }

  return 0;
}


void file_end(FILE *fdin,unsigned int *buf){
  free(buf);
  close(fdin);
}
