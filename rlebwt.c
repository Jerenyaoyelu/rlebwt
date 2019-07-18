#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#include "funct.h"

void CsTable(char *file){//file is command argument
    FILE *fp = fopen(strcat(file,".s"),"r");
    removeExt(file,2);
    char s = '\0';
    int temp[256]={0};
    while(fread(&s,sizeof(s),1,fp)){
        temp[(int)s] ++;
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
//check if bb file exists, if not, then generate one, otherwise, do nothing
//idea1:(not working)
//1. initialize bb with all 1s with same size of b
//2. try to find the corresponding index in bb of all the 0s in b, and replace those 1s with 0s in bb
    //2.1 read s into an array ss
    //2.2 generate an occurrence table occs recording all the number of occurrence of each symbol in cs
    //2.3 then, index = occs[ss[rank1(b,i)]]+1+i-select1(b,rank1(b,i))
//3. i is the index of a 0 in b
//idea2:(may be to slow)
//1.construct an array similar to c table bbc
//2.read b and stops when encounter a 0
//3.get corresponding symbol,
    //3.1 read its value from the array(its binary but presented as integer e.g. 111011)
    //3.2 plus this and the bit chunk(int) so far 
    //3.4 store the new bit chunk (int) in the array
    //problem:fails when text is large, because upper limit of (int)binary is 111111111
//4.repeat this until b is exhausted
//5.loop throgh the array, construct bb
void Check_bb(char *file,char *cs){
    FILE *fp1,*fp2,*fp3;
    if(!(fp1 = fopen(strcat(file,".bbb"),"rd"))){
        removeExt(file,4);
        // fp2 = fopen(strcat(file,".s"),"r");
        // removeExt(file,2);
        // char s = '\0';
        // int occs[256]={0};
        // while(fread(&s,sizeof(s),1,fp2)){
        //     occs[(int)s]++;
        // }
        // fclose(fp2);
        //create bb
        // fp1 = fopen(strcat(file,".bb"),"wb");
        fp3 = fopen(strcat(file,".b"),"rb");
        removeExt(file,2);
        int bitBlock = fgetc(fp3);
        int block = 0;//indicates the block read
        int bit = 0;
        int len = 0;
        int ones = 0;//record the number of 1s read
        int bbc[256] = {0};
        while(bitBlock!=EOF){
            for(int i = 7;i>-1;i--){
                bit = getBit(bitBlock,i);
                if (bit == 1){
                    ones++;
                    //store the new bit block in integer
                    //upper limit is 111111111
                    //simple3 will exceed the limit
                    if(block>0){
                        char tmp = getSymbol(file,ones-1);
                        bbc[(int)tmp]=bbc[(int)tmp]*(int)pow((double)10,(double)(floor(log10(abs(block)))+1))+block;
                        //empty the block
                        block=0;
                        len=0;
                    }
                    block=block*10+bit;
                }else{
                    block=block*10+bit;
                    //correct
                    // printf("index in b %d\n",block*8+(7-i)+1);
                    // //wrong
                    // printf("occ %d\n",occs[getSymbol(file,ones)]);
                    // //
                    // printf("sel %d\n",block*8+(7-i)+1-select(1,file,".b",rank(1,file,".b",block*8+(7-i)+1)));
                }
            }
            bitBlock = fgetc(fp3);
        }
        // fclose(fp1);
        fclose(fp3);
        for(int i=0;i<256;i++){
            if(bbc[i]>0){
                printf("%c %d\n",i,bbc[i]);
            }
        }
    }
}
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
    fclose(fp);
    //display cs table for debugging
    // for(int i= 0;i<256; i++){
    //     if(cst[i]>0){
    //         printf("%c %d\n",i,cst[i]);
    //     }
    // }
    for(int i=len-1;i>-1;i--){
        //remember gap-filler 1s in b and bb, so position has to be positive for rank function in b and bb 
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
        int DupMatches = Search_m(argv[2],"cs.idx",pattern);
        // if(DupMatches>0){
        //     printf("%d\n",DupMatches);
        // }
        Check_bb(argv[2],"cs.idx");
        // printf("%c\n",getSymbol(argv[2],10));
        // printf("count %d\n",rank('a',argv[2],".s",-1));
        // printf("index %d\n",select(1,argv[2],".bb",7));
    }
    return 0;
}