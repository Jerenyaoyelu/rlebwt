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
void Check_bb(char *file, int fsk_pos){
    FILE *fp1;
    char extn[] = ".bbb";
    fp1 = fopen(strcat(file,extn),"rb");
    removeExt(file,strlen(extn));
    if(fp1 == NULL){
        fclose(fp1);
        //init bb
        fp1 = fopen(strcat(file,".b"),"rb");
        removeExt(file,2);
        unsigned int *bb=(unsigned int*)malloc(1000000*sizeof(unsigned int));
        int read_size = sizeof(unsigned int);
        int len_read = 1;
        if(fread(bb,read_size,1000000,fp1)==0){
            len_read = ftell(fp1);
            read_size = 1;
        }
        fseek( fp1, 0, SEEK_SET );
        while(fread(bb,read_size,1000000,fp1)){
            for(int i = 0;i<=ftell(fp1)/4;i++){
                if(ftell(fp1)-4*i<4){
                    bb[i] = pow(2,((ftell(fp1)-4*i)*8))-1;
                }else{
                    bb[i] = pow(2,32)-1;
                }
            }
        }
        //what to do with if not reaching the end with 8MB
        if(ftell(fp1)!=SEEK_END){
            printf("55\n");
        }
        //change 0s
        unsigned int *bitBlock = (unsigned int*)malloc(1000000*sizeof(unsigned int));
        fseek(fp1,0,SEEK_SET);
        int block=0;//record the block read
        char syb = '\0';
        int bit = 0;
        int bb_size = 0;
        int write_size = 0;
        while(fread(bitBlock,read_size,1000000,fp1)){
            if(ftell(fp1)%4 == 0){
                bb_size = ftell(fp1)/4;
                write_size = ftell(fp1)/4;
            }else{
                bb_size = ftell(fp1)/4+1;
                write_size = ftell(fp1);
            }
            for(int j = 0; j <bb_size;j++){
                // printf("j %d\n",j);
                for(int i = 0;i<read_size*8*len_read;i++){
                    bit = getBit(bitBlock[j],block*32+i);
                    // printf("%d %d\n",bit,block*32+i);
                    //reduction of the time of reading files may be an approach to improve
                    if(bit == 0){
                        // get index in s
                        int idx = rank(1,file,".b",block*32+i+1);
                        // //get the symbol
                        syb = getSymbol(file,idx);
                        // printf("%c %d\n",syb,block*32+i+1);
                        // //get place of 0s in bb
                        int place = block*32+i+1-select(1,file,".b",idx)+RowsBef(file,idx,syb)+1;
                        // printf("%d %d\n",block*32+i+1,place);
                        // //change the specific bit into 0
                        // //to change the 1st bit from right hand side, need 1<<0; 1<<8 will do nothing
                        // bb[place/(read_size*8*len_read)] = bb[place/(read_size*8*len_read)] & ~(1<<((place/8+1)*8-place%8));
                        if(place % 8 == 0){
                            bb[place/(read_size*8*len_read)-1] = bb[place/(read_size*8*len_read)-1] & ~(1<<((place/8)*8-(8-place%8)));
                        }else{
                            bb[place/(read_size*8*len_read)] = bb[place/(read_size*8*len_read)] & ~(1<<((place/8+1)*8-place%8));
                        }
                    }
                }
                block++;
            }
        }
        fclose(fp1);
        // printf("%ld\n",ftell(fp1));
        fp1 = fopen(strcat(file,extn),"wb");
        // printf("%ld\n",ftell(fp1));
        removeExt(file,strlen(extn));
        if(bb_size == write_size){
            fwrite(bb,read_size,write_size,fp1);
        }else{
            fwrite(bb,read_size,bb_size-1,fp1);
        }
        
        // for(int k =0;k<bb_size;k++){
        //     printf("%d\n",bb[k]);
        // }
        fclose(fp1);
        free(bb);
        free(bitBlock);
    }
}
int *Search(char *file,char *cs, char *pattern){
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
    int *idtf = (int*)malloc(5001*sizeof(int));
    int count = 0;
    for(int i = fr;i<=ls;i++){
        char sb = getSymbol(file,rank(1,file,".b",i));
        int Sybfront = i;
        identifier = 0;
        char id[30] = {'\0'}; 
        while(sb != '['){
            Sybfront = LF(file,Sybfront,sb,cst);
            if(sb == ']'){identifier = 1;}
            if(identifier == 1 && sb != ']'){
                id[strlen(id)] = sb;
            }
            sb = getSymbol(file,rank(1,file,".b",Sybfront));
        }
        //reverse the string
        inplace_reverse(id);
        if(atoi(id)>0){
            idtf[count+1] = atoi(id);
            count++;
        }
    }
    idtf[0] = count;
    return idtf;
}
int main(int argc, char* argv[]){
    // strcpy(filepath,argv[2]);
    FILE *fp;
    int bb;
    char *pattern = argv[argc-1];
    // printf("%c\n",getSymbol(argv[2],5));
    CsTable(argv[2]);
    // Check_bb(argv[2],0);
    int *DupMatches = Search(argv[2],"cs.idx",pattern);
    if(!strcmp(argv[1],"-m")){
        if(DupMatches[0]> 0){
            printf("%d\n",DupMatches[0]);
        }
    }else if(strcmp(argv[1],"-n")){
        int last_elmt = -1;
        int uniq = 0;
        qsort(DupMatches,DupMatches[0],sizeof(int),compare);
        for(int i = 1; i<DupMatches[0]+1;i++){
            if(DupMatches[i] != last_elmt){
                if(!strcmp(argv[1],"-a")){
                    printf("[%d]\n",DupMatches[i]);
                }
                uniq++;
                last_elmt = DupMatches[i];
            }
        }
        if(!strcmp(argv[1],"-r")){
            printf("%d\n",uniq);
        }
    }
    free(DupMatches);
    return 0;
}