#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "funct.h"

//Attention:
//1. because of writing numbers larger than 255 which can not be represented in 8-bits, 
    //then by using fwrite in 32-bits representation, but when the number is smaller than 255, three blocks of 8-bits will all be 0s
//2. because of writing integers into binary files, the offset 0 starts from left instead of right as normal binary sense
//3. but all these things did not create problems is that when reading cs file, the consistency is kept.
void CsTable(char *file,char *path){//file is command argument
    FILE *fp = fopen(strcat(file,".s"),"r");
    removeExt(file,2);
    char s = '\0';
    int temp[256]={0};
    while(fread(&s,sizeof(s),1,fp)){
        temp[(int)s] ++;
    }
    fclose(fp);
    int sum = 0;
    fp = fopen("cs.txt","wb");
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
        //2.4.2 slect1(b,j), and then count rows since row slect1(b,j) until encountering another 1, denote result as cj
        //2.4.3 get all this counts, cj,ck,....,cz
    //2.5 get the distance bewteen i and the closest 1, i-slect1(b,rank1(b,i))
//so the corresponding index in bb of 0s in b is sum(cj,ck,....,cz)+i-slect1(b,rank1(b,i))+1
void Check_bb(char *file){
    FILE *fp1;
    char extn[] = ".bbb";
    fp1 = fopen(strcat(file,extn),"rb");
    removeExt(file,strlen(extn));
    if(fp1 != NULL){
        fclose(fp1);
        //init bb
        fp1 = fopen(strcat(file,".b"),"rb");
        removeExt(file,2);
        FILE *fp2 = fopen(strcat(file,extn),"wb");
        removeExt(file,strlen(extn));
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
            //write initial bb file
            if(ftell(fp1)%4==0){
                // printf("%ld %ld\n",ftell(fp1)/4,ftell(fp1)%4);
                fwrite(bb,sizeof(unsigned int),ftell(fp1)/4,fp2);
            }else{
                // printf("%ld %ld\n",ftell(fp1)/4,ftell(fp1)%4);
                fwrite(bb,sizeof(unsigned int),ftell(fp1)/4,fp2);
                fwrite(&bb[ftell(fp1)/4],1,ftell(fp1)%4,fp2);
            }
            free(bb);
            bb=(unsigned int*)malloc(1000000*sizeof(unsigned int));
        }
        free(bb);
        fclose(fp2);

        FILE *fp3 = fopen(strcat(file,extn),"r+");
        removeExt(file,strlen(extn));
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
                        int place = block*32+i+1-slect(1,file,".b",idx)+RowsBef(file,idx,syb)+1;
                        // printf("%d %d\n",block*32+i+1,place);
                        //change the specific bit into 0
                        //to change the 1st bit from right hand side, need 1<<0; 1<<8 will do nothing
                        int atc = 0;
                        if(place%8 == 0){
                            fseek(fp3,place/8-1,SEEK_SET);
                            fread(&atc,1,1,fp3);
                            atc = atc & ~(1<<(place%8));
                            fseek(fp3,place/8-1,SEEK_SET);
                            fwrite(&atc,1,1,fp3);
                        }else{
                            fseek(fp3,place/8,SEEK_SET);
                            fread(&atc,1,1,fp3);
                            atc = atc & ~(1<<(8-place%8));
                            fseek(fp3,place/8,SEEK_SET);
                            fwrite(&atc,1,1,fp3);
                        }
                    }
                }
                block++;
            }
            free(bitBlock);
            bitBlock = (unsigned int*)malloc(1000000*sizeof(unsigned int));
        }
        fclose(fp1);
        fclose(fp3);
        free(bitBlock);
    }
}
int *Search(char *file,char *cs, char *pattern,char *command){
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
    int *frls = backwardSearch(file,cst,pattern);
    if(!strcmp(command,"-m")){
        return frls;
    }else{
        // printf("%d %d\n",frls[0],frls[1]);
        //problems: can't find the third 'ana' in shopping,why?
        //possible reason: file is too large, cannot read all
        int *idtf = (int*)malloc(5001*sizeof(int));
        int count = 0;
        for(int i = frls[0];i<=frls[1];i++){
            char sb = getSymbol(file,rank(1,file,".b",i));
            int Sybfront = i;
            identifier = 0;
            char id[30] = {'\0'}; 
            //too slow
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
}
char *findRecord(char *file,char *cs,char *identifier){
    //generate c table and occ table(here occ table is a table recording the number of rows of each block in b file/each row in s file)
    FILE *fp1,*fp2;
    fp1 = fopen(strcat(file,".b"),"rb");
    removeExt(file,2);
    fp2 = fopen("sooc.txt","wb");
    char symbol = '\0';
    int bit = 0;
    int block = 0;
    int read_size = sizeof(unsigned int);
    int len_read = 1;
    int ones = 1;
    int c[256];
    //init c table
    //then I can grab the first symbol of bb in c
    for(int j = 1;j<256;j++){
        c[j] = -1;
    }
    int temp[256] = {0};
    int amount = 0;
    int *sooc = (int*)malloc(500000*sizeof(int));
    unsigned int *b = (unsigned int*)malloc(1000000*sizeof(unsigned int));
    //to see if the size of file is smaller than 4 bytes
    if(fread(b,read_size,1000000,fp1)==0){
        read_size = 1;
        len_read = ftell(fp1);
    }
    fseek(fp1,0,SEEK_SET);
    int isFirstSymbol = 1;
    while(fread(b,read_size,1000000,fp1)){
        for(int i = 0; i<=ftell(fp1)/4;i++){
            if(i == ftell(fp1)/4){len_read = ftell(fp1)%4;}
            for(int j = 0; j<read_size*8*len_read;j++){
                bit = getBit(b[i],i*32+j);
                if(bit == 1){
                    if(isFirstSymbol == 0){
                        symbol = getSymbol(file,ones);
                        temp[(int)symbol] += (amount+1);
                        // int tmp = amount+1;
                        if(symbol != '\0'){
                            // fwrite(&tmp,sizeof(int),1,fp2);
                            sooc[ones-1] = amount+1;
                        }
                        amount = 0;
                        ones++;
                    }else{isFirstSymbol = 0;}
                }else{amount++;}
            }
        }
        free(b);
        b = (unsigned int*)malloc(1000000*sizeof(unsigned int));
    }
    //when last bit is 0, this step is needed
    if(amount>0){
        symbol = getSymbol(file,ones);
        temp[(int)symbol] += (amount+1);
        amount = 0;
    }
    fclose(fp1);
    fclose(fp2);
    free(b);
    //c[0] stores the number of gap-filler ones
    int rowsBf = 0;
    for(int i = 1;i<256;i++){
        if(temp[i]>0){
            c[i] = rowsBf;
            rowsBf += temp[i];
            // printf("%c %d\n",i,c[i]);
        }
    }
    //search record
    //read cs table
    int cst[256] = {0};
    FILE *fp = fopen(cs,"rb");
    int tmp = 0;
    int idf = 0;//0 stands for symbol, 1 stands for #lessthan
    int sybl = -1;
    while(fread(&tmp,sizeof(tmp),1,fp)){
        if(idf ==1){
            cst[sybl] = tmp;
            idf = 0;
        }else{
            sybl = tmp;
            idf = 1;
        }
    }
    fclose(fp);
    //locate identifier and get the index in b file of '[' to do forward search 
    int *fr_ls = backwardSearch(file,cst,identifier);
    // printf("aa%d %d\n",fr_ls[0],fr_ls[1]);
    //first, backward search to find the "[" before identifier[0]
    //then, loop through all index of matched "[" to do forward search to find the "]" after identifier[-1]
    //then start to decode the text by using forward decoding
    int foundID = 0;
    int *nxt;
    //get the indx in b of ']' after the identifier
    for(int i = fr_ls[0]; i<=fr_ls[1];i++){
        char readID[20] = {'\0'};
        // printf("%d\n",i);
        int tp = rank(1,file,".b",i);
        // printf("tp %d\n",tp);
        if(getSymbol(file,tp) == '['){
            int l = 0;
            nxt = Next(file,c,sooc,i);
            while(1){
                // printf("%c %d\n",nxt[0],nxt[1]);
                if((char)nxt[0] == ']'){
                    if(!strcmp(readID,identifier)){
                        foundID = 1;
                        break;
                    }else{break;}
                }else{
                    if(strlen(readID)<strlen(identifier)){
                        readID[l] = (char) nxt[0];
                        nxt = Next(file,c,sooc,nxt[1]);
                        l++;
                    }else{break;}
                }
            }
            if(foundID == 1){
                break;
            }
        }
    }
    // printf("%c %d\n",nxt[0],nxt[1]);
    // printf("%s\n",readID);
    char *record = (char*)malloc(50000*sizeof(char));
    nxt = Next(file,c,sooc,nxt[1]);
    // printf("%c %d",nxt[0],nxt[1]);
    int length = 0;
    while((char)nxt[0]!='['){
        record[length] = (char)nxt[0];
        length++;
        nxt = Next(file,c,sooc,nxt[1]);
    }
    free(nxt);
    free(sooc);
    return record;
}
int main(int argc, char* argv[]){
    // strcpy(filepath,argv[2]);
    FILE *fp;
    int bb;
    char *pattern = argv[argc-1];
    //check if index folder exist
    
    // printf("%c\n",getSymbol(argv[2],5));
    CsTable(argv[2],"~/a2/dummy");
    Check_bb(argv[2]);
    if(strcmp(argv[1],"-n")){
        int *DupMatches = Search(argv[2],"cs.txt",pattern,argv[1]);
        if(!strcmp(argv[1],"-m")){
            if(DupMatches[1]-DupMatches[0]>=0){printf("%d\n",DupMatches[1]-DupMatches[0]+1);}
        }else{
            int last_elmt = -1;
            int uniq = 0;
            mergeSort(DupMatches,1,DupMatches[0]);
            for(int i = 1; i<DupMatches[0]+1;i++){
                if(DupMatches[i] != last_elmt){
                    if(!strcmp(argv[1],"-a")){printf("[%d]\n",DupMatches[i]);}
                    uniq++;
                    last_elmt = DupMatches[i];
                }
            }
            if(!strcmp(argv[1],"-r")){printf("%d\n",uniq);}
        }
        free(DupMatches);
    }else{
        char *tmp = findRecord(argv[2],"cs.txt",pattern);
        if(strlen(tmp)>0){
            printf("%s\n",tmp);
        }
        free(tmp);
    }
    return 0;
}