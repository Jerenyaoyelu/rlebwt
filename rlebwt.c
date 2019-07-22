#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#include "funct.h"

//Attention:
//1. because of writing numbers larger than 255 which can not be represented in 8-bits, 
    //then by using fwrite in 32-bits representation, but when the number is smaller than 255, three blocks of 8-bits will all be 0s
//2. because of writing integers into binary files, the offset 0 starts from left instead of right as normal binary sense
//3. but all these things did not create problems is that when reading cs file, the consistency is kept.
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
//modified idea1:(works, but fail in shopping(too slow))
//1. initialize bb with all 1s with same size of b
//2. try to find the corresponding index in bb of all the 0s in b, and replace those 1s with 0s in bb
    //2.1 read b bit by bit, encounter a 0 in index i
    //2.2 find corresponding symbol si = getSymbol(b,rank1(b,i))
    //2.3 count1 = rank1(b,i)
    //2.4 read s one by one and get all symbol not larger than si, count all its occurrences in B
        //2.4.1 say symbol sj, the j-th one in s
        //2.4.2 select1(b,j), and then count rows since row select1(b,j) until encountering another 1, denote result as cj
        //2.4.3 get all this counts, cj,ck,....,cz
    //2.5 get the distance bewteen i and the closest 1, i-select1(b,rank1(b,i))
//so the corresponding index in bb of 0s in b is sum(cj,ck,....,cz)+i-select1(b,rank1(b,i))+1
void Check_bb(char *file){
    FILE *fp1,*fp3;
    char extn[] = ".bb";
    fp1 = fopen(strcat(file,extn),"rb");
    removeExt(file,strlen(extn));
    if(fp1 == NULL){
        fclose(fp1);
        //init bb
        fp1 = fopen(strcat(file,".b"),"rb");
        removeExt(file,2);
        int bitB = fgetc(fp1);
        int *bb = (int*)malloc(2250000*sizeof(int));
        int j = 0;
        while(bitB != EOF){
            bb[j] = 255;
            j++;
            bitB = fgetc(fp1);
        }
        fclose(fp1);
        //change 0s
        fp3 = fopen(strcat(file,".b"),"rb");
        removeExt(file,2);
        int bitBlock = fgetc(fp3);
        int block=0;//record the block read
        char syb = '\0';
        int bit = 0;
        while(bitBlock!=EOF){
            for(int i = 7;i>-1;i--){
                bit = getBit(bitBlock,i);
                //reduction of the time of reading files may be an approach to improve
                if(bit == 0){
                    //get index in s
                    int idx = rank(1,file,".b",block*8+(7-i)+1);
                    //get the symbol
                    syb = getSymbol(file,idx);
                    //get place of 0s in bb
                    int place = block*8+(7-i)+1-select(1,file,".b",idx)+RowsBef(file,idx,syb)+1;
                    //change the specific bit into 0
                    //to change the 1st bit from right hand side, need 1<<0; 1<<8 will do nothing
                    if(place % 8 == 0){
                        bb[place/8-1] = bb[place/8-1] & ~(1<<(place % 8));
                    }else{
                        bb[place/8] = bb[place/8] & ~(1<<(8 - (place % 8)));
                    }
                }
            }
            bitBlock = fgetc(fp3);
            block++;
        }
        fclose(fp3);
        fp1 = fopen(strcat(file,extn),"wb");
        removeExt(file,strlen(extn));
        for(int k =0;k<j;k++){
            fputc(bb[k],fp1);
        }
        fclose(fp1);
        free(bb);
    }
}
int *Search_m(char *file,char *cs, char *pattern){
    int *res = (int*)malloc(2*sizeof(int));
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
    for(int i=len-1;i>-1;i--){
        //remember gap-filler 1s in b and bb, so position has to be positive for rank function in b and bb 
        if(i == len-1){
            fr = select(1,file,".bb",cst[(int)pattern[i]]+1);
            //take negative input as the position argument to get the occurrence of first letter 
            //when doing backward search
            ls = select(1,file,".bb",cst[(int)pattern[i]]+1+rank(pattern[i],file,".s",-1))-1;
        }else{
            //cs to get # of 1s before
            //rank to get the # of occ of current symbol up to the cloest 1
            //fr-select(1,file,".b",rank(1,file,".b",fr)) to get the i-th current symbol if it is 0 in b
            int cloest_one = select(1,file,".b",rank(1,file,".b",fr));
            fr = select(1,file,".bb",cst[(int)pattern[i]]+1+rank(pattern[i],file,".s",rank(1,file,".b",cloest_one-1)))+(fr-cloest_one);
            ls = select(1,file,".bb",cst[(int)pattern[i]]+1+rank(pattern[i],file,".s",rank(1,file,".b",ls)))-1;
        }
    }
    res[0]=fr;
    res[1]=ls;
    return res;
}

// char *Search_r(char *file,int *startingPosi){

// }
int main(int argc, char* argv[]){
    // strcpy(filepath,argv[2]);
    FILE *fp;
    int bb;
    char *pattern = argv[argc-1];
    if(!strcmp(argv[1],"-m")){
        CsTable(argv[2]);
        Check_bb(argv[2]);
        int *DupMatches = Search_m(argv[2],"cs.idx",pattern);
        if(DupMatches[1]- DupMatches[0]+1>0){
            printf("%d\n",DupMatches[1]-DupMatches[0]+1);
        }
        // printf("%c\n",getSymbol(argv[2],10));
        // printf("count %d\n",rank('a',argv[2],".s",-1));
        // printf("index %d\n",select(1,argv[2],".bb",7));
    }
    return 0;
}