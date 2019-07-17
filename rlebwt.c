#include<stdio.h>

#include "funct.h"

void CsTable(char *file){//file is command argument
    FILE *fp = fopen(strcat(file,".s"),"r");
    char s = fgetc(fp);
    int temp[256]={0};
    while(s!=EOF){
        temp[(int)s] ++;
        s = fgetc(fp);
    }
    fclose(fp);
    int sum = 0;
    fp = fopen("cs.idx","wb");
    for(int i = 0;i<256;i++){
        if(temp[i]>0){
            fwrite(&i,sizeof(i),1,fp);
            //when sum excceeds 256, how to write it into files??
            //solved by using fwrite instead of fputc
            fwrite(&sum,sizeof(i),1,fp);
            sum += temp[i];
        }
    }
    fclose(fp);
}
// void OccTable(char *file){
//     FILE *fp = fopen(strcat(file,".s"),"r");

// }
int Search_m(char *file,char *cs, char *pattern){
    int len = strlen(pattern);
    int fr = -1,ls = -1;
    char c = pattern[len-1];
    int cst[256] = {0};
    //read cs table
    FILE *fp = fopen(cs,"rb");
    int tmp = 0;
    int identifier = 0;//0 stands for symbol, 1 stands for #lessthan
    int symbol = -1;
    while(fread(&tmp,sizeof(tmp),1,fp)){
        if(identifier ==1){
            cst[symbol] = tmp;
            identifier = 0;
        }else{
            symbol = tmp;
            identifier = 1;
        }
    }
    //display cs table for debugging
    // for(int i= 0;i<256; i++){
    //     if(cst[i]>0){
    //         printf("%c %d\n",i,cst[i]);
    //     }
    // }
    fclose(fp);
    for(int i=len-1;i>-1;i--){
        if(i == len-1){
            fr = select(1,file,".bb",cst[(int)pattern[i]]+1);
            //take negative input as the position argument to get the occurrence of first letter 
            //when doing backward search
            ls = select(1,file,".bb",cst[(int)pattern[i]]+1+rank(pattern[i],file,".s",-1))-1;
        }else{
            fr = select(1,file,".bb",cst[(int)pattern[i]]+1+rank(pattern[i],file,".s",rank(1,file,".b",fr-1)));
            ls = select(1,file,".bb",cst[(int)pattern[i]]+1+rank(pattern[i],file,".s",rank(1,file,".b",ls)))-1;
        }
    }
    return ls-fr+1;
}
int main(int argc, char* argv[]){
    // strcpy(filepath,argv[2]);
    FILE *fp;
    int bb;
    char *pattern = argv[argc-1];
    if(!strcmp(argv[1],"-m")){
        CsTable(argv[2]);
        removeExt(argv[2],2);
        printf("%d\n",Search_m(argv[2],"cs.idx",pattern));
        // printf("count %d\n",rank('a',argv[2],".s",-1));
        // printf("index %d\n",select(1,argv[2],".bb",7));
    }
    return 0;
}