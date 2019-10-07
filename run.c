#include <stdio.h>
#include <stdlib.h>

int main(){
  char temp[512];
  int i;
  for(i = 595; i < 620; i++){
    if(i != 491){
      sprintf(temp,"%s%04d%s%04d%s","./anaall ../../data/maiko17_",i,".ridf ./hbook/",i,".hb");
      //    printf("%s\n",temp);
      system(temp);
    }
  }
  return 0;
}
