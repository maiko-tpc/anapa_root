#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

/* babirl */
#include <bi-config.h>
#include <bi-common.h>
#include <ridf.h>

#include "anapa.h"
#include "kinema.h"

extern char gname[L_LINE];
extern char comname[L_LINE];

void argerror(){
  printf
  ("Usage: ana [-dst name] [-help] [-global name] [-online hostname] [-port port] [bldfile] [hbfile]\n");
  exit(-1);
}

int check_option(char *arg){
  const struct {char *option; int i;} 
  table[]={{"-dst",1},{"-help",2},{"-",3},{"-global",4},{"-online",5},
		   {"-port",6},{"-b",7},{"-rp",8}};
  int j;
  for(j=0;j<9;j++){ 
	/* when you add new option, you MUST change an above line.*/
    if(strcmp(arg,table[j].option)==0)
      return(table[j].i);
  }
  return(-1);
}


int startup(int argc,char *argv[],int *fd,char *hbfile, int *fdst) {
  int i,j,k,flag,port=0;
  int buffsize;
  int lgname;
  char *filename[2];
  char *dstname;  /* Output file name for DST file */
  char hostname[80];
  flag=0;j=0;
  filename[0]=NULL;filename[1]=NULL;

  /**** Set Default Value ****/
  strcpy(gname,"TEST");
  
  for(i=1;i<argc;i++){
    if(*(argv[i])=='-'){
      switch(check_option(argv[i])){
      case 1: /* Make DST file */
	flag|=0x1;
	if(++i==argc){
	  printf("%s:Missing DST filename.\n",argv[0]);
	  argerror();
	}
	dstname=argv[i];
	break;
      case 2: /* help */
	//printf("Usage: ana [-dst name] [-help] [-global name] "
	//"[-online hostname] [-port port] [bldfile] [hbfile] [-b blocksize] "
	//" [-rp parameter file]\n");
	printf("Usage: ana [-dst name] [-help] [-global name] "
	       "[-online hostname] [bldfile] [hbfile]\n");
	printf("-dst: anapa makes data summary table and stored in a file.\n");
	printf("      If -online is given, this option will be ignored.\n");
	printf("-help: Display this help.\n");
	printf("-global [name]: Use global section. \n");
	printf("            [name] is not case sensitive.\n");
	printf("            If the last 4 characters of [name] are "
	       "\".bld\",\n");
	printf("            ana will ignore it and use default name.\n");
        printf("            The default name is \"TEST\".\n");
	printf("-online: Online analysis. When this option is specified,\n");
	printf("          ana enables \"-global\" option automatically.\n");
	printf("         hostname must be specified following this option.\n");
	printf("-port: Specify port number. If -online option isn't given\n");
	printf("         ana ignores this option. \n");
	printf("       If this option is omitted, ana connects \n");
        printf("         the target host through default port(1126)\n");
	printf("[bldfile]: Specify build data filename. ");
	printf("If the [bldfile] is - or missing,\n");
        printf("          ana reads build data from standard input.\n");
	printf("[hbfile]: Specify output hbook filename. ");
	printf("If the [hbfile] is - or missing,\n");
	printf("         ana outputs to \"hist.dat\" .\n");
	//printf("-b blocksize: Specify the number of bytes of data block\n");
	//printf("              If -b option is omitted, \n");
	//printf("              ana uses default value(65536).\n");
	//printf("              1k=1024byte.\n");
	//printf("              If -online option is specified, ana detects\n");
	//printf("              block size automatically.\n");
	//printf("-rp parameter file: Specify run parameter file.\n");
	exit(-1);
	break;
      case 3: /* standard I/O */
		filename[j++]=NULL;
		if(j>2) {
		  printf("%s:Too many argments.\n",argv[0]);argerror();
		}
		break;
      case 4: /* use global section */
		flag|=0x8;
		if(((i+1)!=argc)&&*argv[i+1]!='-'){
		  lgname=strlen(argv[++i]);
		  if(lgname<4){
			strcpy(gname,argv[i]);
		  }
		  else{
			if(strncmp(&argv[i][lgname-4],".bld",4)!=0)
			  strcpy(gname,argv[i]);
			else i--;
		  }
		}
		lgname=strlen(gname);
		for(k=0;k<lgname;k++){
		  if((gname[k]>0x60)&&(gname[k]<0x7b)) gname[k]-=32;
		}
		break;
      case 5: /* online analysis */
		flag|=0x18;
		if(++i==argc){
		  printf("%s:Missing hostname.\n",argv[0]);
		  argerror();
		}
		strcpy(hostname,argv[i]);
		break;
      case 6: /* port option */
		flag|=0x20;
		if(++i==argc){
		  printf("%s:Missing port number.\n",argv[0]);
		  argerror();
		}
		port=atoi(argv[i]);
		if(port==0){
		  printf("%s:Illeagal port number.\n",argv[0]);
		  argerror();
		}
		break;
#if 0
      case 7: /* block size option */
		if(++i==argc){
		  printf("%s:Missing block size.\n",argv[0]);
		  argerror();
		}
		else{
		  size_t largv;
		  SIZEOFBLK=atoi(argv[i]);
		  largv=strlen(argv[i]);
		  if(((*(argv[i]+largv-1))&0x4b)==0x4b){
			*(argv[i]+largv-1)=0;
			SIZEOFBLK*=1024;
		  }
		}
		if(SIZEOFBLK==0){
		  printf("%s:Illeagal block size.\n",argv[0]);
		  argerror();
		}
		break;
#endif
	  case 8: /* run parameter file */
		if(++i==argc){
		  printf("%s:Missing run parameter file.\n",argv[0]);
		  argerror();
		}
		//		else{
		//		  readpar(argv[i],&grcr);
		//		}
		break;
      default:
		printf("%s:Illegal option %s .\n",argv[0],argv[i]);
		argerror();
		break;
      }
    }
    else{
      filename[j++]=argv[i];
      if(j>2) {
	printf("%s:Too many argments.\n",argv[0]);argerror();
      }
    }

  }
  
  if(flag & 0x10){ /* online analysis */
#if 0 /* do something here to open shared memory */
    if(tcp_open(hostname,port,1,O_SAMPLE,argv[0],&buffsize)){
	  printf("%s:Fail to tcp_open %s.\n",argv[0],hostname);
	  exit(-1);
	}
	if(buffsize<0){
	  printf("%s:Inconsistent buffer size.(%d)\n",argv[0],buffsize);
	  exit(-1);
	}
	else {
	  printf("Buffer size is %d.\n",buffsize);
	  SIZEOFBLK=buffsize;
	}
#endif
  }
  else { /* online analysis with global section */
    if(filename[0]!=NULL ){
      *fd=open(filename[0],O_RDONLY,NULL);
      if(*fd == -1) {
	printf("%s:Cannnot open %s .\n",argv[0],filename[0]);
	switch(errno){
	case ENOENT:
	  printf("%s:%s:No such file!\n",argv[0],filename[0]);break;
	case EACCES:
	  printf("%s:%s:Perimission denied! \n",argv[0],filename[0]);break;
	case EISDIR:
	  printf("%s:%s is a diretory!\n",argv[0],filename[0]);break;
	default :
	  printf("%s:Error(file open)! \n",argv[0]);
	}
	printf("%s:Exit.\n",argv[0]);
	exit(-1);
      }
    }
    else *fd=STDIN_FILENO;
  }
  if(!(flag & 0x8)){  /*  offline analysis without global section */
    if(filename[1]!=NULL){
      strcpy(hbfile,filename[1]);
      printf("hbookfile:%s\n",hbfile);}
    else
      strcpy(hbfile,"hist.dat");
  }
  else{
      strcpy(hbfile,gname);
  }

  if((flag & 0x11)==0x1){ /* DST table will be made. */
    *fdst=open(dstname,O_WRONLY|O_TRUNC|O_CREAT,
	       S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if(*fdst == -1) {
      printf("%s:Cannnot open %s .\n",argv[0],dstname);
      switch(errno){
      case EACCES:
	printf("%s:%s:Perimission denied! \n",argv[0],dstname);break;
      case EISDIR:
	printf("%s:%s is a diretory!\n",argv[0],dstname);break;
      default :
	printf("%s:Error(file open)! \n",argv[0]);
      }
      printf("%s:Exit.\n",argv[0]);
      exit(-1);
    }
  }

#if 0
  /* memory allocation */
  buff=(unsigned short *)malloc(SIZEOFBLK);
  rawbuff=(unsigned short *)malloc(SIZEOFBLK);
  WORDOFBLK=SIZEOFBLK/sizeof(short);

  if(buff==NULL || rawbuff==NULL){
    printf("Memory allocating error.\n");
    exit(-1);
  }
#endif

  return(flag);
}

void fopenerror(char *s){
  switch(errno){
  case EACCES:
    printf("%s:Permission denied!\n",s);break;
  case EISDIR:
    printf("%s is directory!\n",s);break;
  case ENOENT:
    printf("%s:No such a file!\n",s);break;
  default:
    printf("%s:Error(fffopen)! \n",s);break;
  }
  printf("Exit.\n");
  exit(-1);
}

int setnextev(struct evtdata *evt){
  char charbuf[L_LINE];
  int tmpnev=0;

 again:
  printf("EventNum> ");
  fgets(charbuf,L_LINE,stdin);

  if(strlen(charbuf)==1){
    evt->tgtnev=evt->nev+1;
    evt->tgtiev=0x7fffffff;
    return(evt->tgtnev);
  }

  if(sscanf(charbuf,"%d",&tmpnev)!=1) {
    printf(" Positive integer must be given. \n");
    goto again;
  }

  if(charbuf[0]=='+'){
    evt->tgtnev=evt->nev+tmpnev;
    evt->tgtiev=0x7fffffff;
    return(evt->tgtnev);
  }

  if(tmpnev > evt->iev){
    evt->tgtnev=0x7fffffff;;
    evt->tgtiev=tmpnev;
    return(-1.0*evt->tgtnev);
  }
  printf(" Event number must be larger than the present number (%d)\n"
	 "unless the '+' sign is given.\n",evt->iev);
  goto again;
}
