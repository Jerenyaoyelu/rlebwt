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
// int *RowsTable(char *file){
//     FILE *fp;
//     fp = fopen(strcat(file,".s"),"rb");
//     removeExt(file,2);
//     int i = 1;
//     int *rt = (int*)malloc(1024*sizeof(int));
//     char s = fgetc(fp);
//     while(s != EOF){
//         rt[i] = RowsBef(file,i,s);
//         s = fgetc(fp);
//         i++;
//     }
//     rt[0] = i-1;
//     fclose(fp);
//     return rt;
// }
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
    char extn[] = ".bbb";
    fp1 = fopen(strcat(file,extn),"rb");
    removeExt(file,strlen(extn));
    if(fp1 == NULL){
        fclose(fp1);
        //init bb
        fp1 = fopen(strcat(file,".b"),"rb");
        removeExt(file,2);
        int bitB = fgetc(fp1);
        int *bb = (int*)malloc(1024*sizeof(int));
        int j = 0;
        while(bitB != EOF){
            bb[j] = 255;
            j++;
            bitB = fgetc(fp1);
        }
        fclose(fp1);
        //pre compute Rowsbefore Table
        //same, still two slow in shopping
        // int *p = RowsTable(file);
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
                if(bit == 0){
                    //get index in s
                    int idx = rank(1,file,".b",block*8+(7-i)+1);
                    //get the symbol
                    syb = getSymbol(file,idx);
                    //get place of 0s in bb
                    int place = block*8+(7-i)+1-select(1,file,".b",idx)+RowsBef(file,idx,syb)+1;
                    // int place = block*8+(7-i)+1-select(1,file,".b",idx)+p[idx]+1;
                    // printf("%d %d\n",block*8+(7-i)+1,place);
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
        //because of the way to implementat getSymbol, bbc[0] is the # of gap-filler 1s
        for(int k =0;k<j;k++){
            fputc(bb[k],fp1);
        }
        fclose(fp1);
        free(bb);
        // free(p);
    }
    fclose(fp1);
}
//idea2:(works, but fail in dblp(too slow), write will be a big problem!)
//1.construct an array similar to c table bbc
//2.read b and stops when encounter a 0
//3.get corresponding symbol,
    //3.1 read its value from the array(its binary but presented as integer e.g. 111011)
    //3.2 plus this and the bit chunk(int) so far 
    //3.4 store the new bit chunk (int) in the array
//4.repeat this until b is exhausted
//5.loop throgh the array, construct bb
// void Check_bb(char *file){
//     FILE *fp1,*fp3;
//     if(!(fp1 = fopen(strcat(file,".bbb"),"rb"))){
//         fclose(fp1);
//         removeExt(file,4);
//         fp3 = fopen(strcat(file,".b"),"rb");
//         removeExt(file,2);
//         int bitBlock = fgetc(fp3);
//         int block=0;//record the block read
//         int bit = 0;
//         int len = 0;
//         int ones = 0;//record the number of 1s read
//         int bbc[256] = {0};
//         int totlBits = 0;
//         //problem: cannot distinguish the gap-filler 1s
//         //last 1(gap-filler or not) in b is still in block
//         while(bitBlock!=EOF){
//             for(int i = 7;i>-1;i--){
//                 totlBits++;
//                 bit = getBit(bitBlock,i);
//                 if (bit == 1){
//                     //store the new bit block in integer
//                     //upper limit is 111111111
//                     //simple3 will exceed the limit
//                     if(block>0){
//                         char tmp = getSymbol(file,ones);
//                         // bbc[(int)tmp]=bbc[(int)tmp]*(int)pow((double)10,(double)(floor(log10(abs(block)))+1))+block;
//                         bbc[(int)tmp]=bbc[(int)tmp]*(int)pow((double)2,(double)len)+block;
//                         //empty the block
//                         block = 0;
//                         totlBits -= len;
//                         len = 0;
//                         // printf("\n");
//                     }
//                     block = block*2+bit;
//                     len++;
//                     // printf("%d",bit);
//                     ones++;
//                 }else{
//                     block = block*2+bit;
//                     len++;
//                     // printf("%d",bit);
//                 }
//             }
//             bitBlock = fgetc(fp3);
//         }
//         char tmp = getSymbol(file,ones);
//         bbc[(int)tmp]=bbc[(int)tmp]*(int)pow((double)2,(double)len)+block;
//         // fclose(fp1);
//         fclose(fp3);
//         fp1 = fopen(strcat(file,".bbb"),"wb");
//         removeExt(file,4);
//         //because of the way to implementat getSymbol, bbc[0] is the # of gap-filler 1s
//         for(int i=1;i<256;i++){
//             if(bbc[i]>0){
//                 fwrite(&bbc[i],sizeof(i),1,fp1);
//             }
//         }
//         fwrite(&bbc[0],sizeof(int),1,fp1);
//         fclose(fp1);
//     }
// }
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
            fr = select(1,file,".bb",cst[(int)pattern[i]]+1+rank(pattern[i],file,".s",rank(1,file,".b",fr-1)));
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