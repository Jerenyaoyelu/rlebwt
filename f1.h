#include<string.h>
#include <sys/stat.h>

// get n continuous bits from source starting at the offset bit
// (source >> offset) & ((1 << n) - 1) 
// offset 0 is the rightmost bit
int getBit(unsigned int num,int pos){
    int rp = (pos/8+1)*8-1-pos%8;
    return (num>>rp)&((1<<1)-1);
}
void removeExt(char *filename,int num){
    int len =strlen(filename);
    filename[len-num] = '\0';
}
unsigned int rank(char target, char *file,char *extsn, unsigned int position, unsigned int readBit){
    unsigned int count = 0;
    FILE *fp = fopen(strcat(file,extsn),"rb");
    removeExt(file,strlen(extsn));
    fseek(fp,readBit,SEEK_SET);
    //rank in s file
    if(extsn[1]=='s'){
        unsigned char *c = (unsigned char*)malloc(1000*sizeof(unsigned char));
        //when position smaller than 0, 
        //then this function can achievev the purpose of tackling the problem of counting occurrence of first symbol
        //when doing backward search
        //because this will block the "if break" control flow
        int looptimes = 0;
        fread(c,sizeof(unsigned char),1000,fp);
        if(ftell(fp)%1000!=0){
            looptimes = ftell(fp)%1000;
        }else{
            looptimes = 1000;
        }
        for(int i = 0;i<looptimes;i++){
            if(position == 0){
                break;
            }
            position--;
            if (c[i] == target){
                count++;
            };
        }
        free(c);
    }else{//rank in b/bb file
        unsigned int *bitBlock = (unsigned int*)malloc(25000*sizeof(unsigned int));
        int read_size = sizeof(unsigned int);
        int looptimes = 0;
        int len_read = 1;//when file is smaller than 4 bytes, this will record the number of bytes read
        fread(bitBlock,read_size,25000,fp);
        if(ftell(fp)<4){
            len_read = ftell(fp)%4;
            looptimes = ftell(fp)/4;
            read_size = 1;
        }else {
            if(ftell(fp)%(25000*4)!=0){
                looptimes = (ftell(fp)%(25000*4))/4;
            }else{
                looptimes = 25000;
            }
        }
        fseek( fp, readBit, SEEK_SET );
        fread(bitBlock,read_size,25000,fp);
        for(int i = 0;i<=looptimes;i++){
            if(position == 0){
                break;
            }
            for(int j = 0; j <read_size*8*len_read;j++){
                position--;
                if (getBit(bitBlock[i],j) == (int)target){
                    count++;
                };
                if(position == 0){
                    break;
                }
            }
        }
        free(bitBlock);
    }
    fclose(fp);
    return count;
}
unsigned int slect(char target, char* file, char *extsn, unsigned int count, unsigned int readBlock){
    unsigned int index = 0;
    FILE *fp = fopen(strcat(file,extsn),"r");
    removeExt(file,strlen(extsn));
    fseek( fp, readBlock, SEEK_SET );
    if(extsn[1]=='s'){
        unsigned char *c = (unsigned char*)malloc(1000*sizeof(unsigned char));
        fread(c,sizeof(unsigned char),1000,fp);
        int looptimes = 0;
        fread(c,sizeof(unsigned char),1000,fp);
        if(ftell(fp)%1000!=0){
            looptimes = ftell(fp)%1000;
        }else{
            looptimes = 1000;
        }
        for(int i = 0;i<looptimes;i++){
            if (c[i] == target){
                count--;
            };
            index++;
            if(count == 0){
                break;
            }
        }
        free(c);
    }else{//rank in b/bb file
        unsigned int *bitBlock = (unsigned int*)malloc(25000*sizeof(unsigned int));
        int read_size = sizeof(unsigned int);
        int looptimes = 0;
        int len_read = 1;//when file is smaller than 4 bytes, this will record the number of bytes read
        // printf("%ld\n", ftell(fp)); //tell the position of file pointer 
        fread(bitBlock,read_size,25000,fp);
        if(ftell(fp)<4){
            len_read = ftell(fp)%4;
            looptimes = ftell(fp)/4;
            read_size = 1;
        }else {
            if(ftell(fp)%(25000*4)!=0){
                looptimes = (ftell(fp)%(25000*4))/4;
            }else{
                looptimes = 25000;
            }
        }
        fseek( fp, readBlock, SEEK_SET );
        fread(bitBlock,read_size,25000,fp);
        for(int i = 0;i<=looptimes;i++){
            if(count == 0){
                break;
            }
            for(int j = 0;j<read_size*8*len_read;j++){
                if(count == 0){
                    break;
                }
                index++;
                if (getBit(bitBlock[i],j) == (int)target){
                    count--;
                };
            }
        }
        free(bitBlock);
    }
    fclose(fp);
    return index;
};
//get symbol from s file
//when count is large than file size, it will return '\0'
//coincidently, when return '\0', it indicates that we are reading gap-fillers 1s in b
char getSymbol(char *file, unsigned int count){
    // char res;
    FILE *fp = fopen(strcat(file,".s"),"rb");
    removeExt(file,2);
    char c = '\0';
    fseek(fp,count-1,SEEK_SET);
    fread(&c,sizeof(char),1,fp);
    fclose(fp);
    return c;
}
void inplace_reverse(char * str){
  if (str){
    char * end = str + strlen(str) - 1;
#   define XOR_SWAP(a,b) do\
    {\
      a ^= b;\
      b ^= a;\
      a ^= b;\
    } while (0)
    while (str < end){
      XOR_SWAP(*str, *end);
      str++;
      end--;
    }
#   undef XOR_SWAP
  }
}
// Merges two subarrays of arr[]. The first subarray is arr[l..m], the second subarray is arr[m+1..r] 
void merge(int arr[], int l, int m, int r) { 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 =  r - m; 
    //create temp arrays 
    int L[n1], R[n2]; 
    // Copy data to temp arrays L[] and R[] 
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1+ j]; 
    //Merge the temp arrays back into arr[l..r]
    i = 0; // Initial index of first subarray 
    j = 0; // Initial index of second subarray 
    k = l; // Initial index of merged subarray 
    while (i < n1 && j < n2) { 
        if (L[i] <= R[j]) { 
            arr[k] = L[i]; 
            i++; 
        }else{ 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
    //Copy the remaining elements of L[], if there are any 
    while (i < n1) { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
    // Copy the remaining elements of R[], if there are any 
    while (j < n2) { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
} 
//l is for left index and r is right index of the sub-array of arr to be sorted 
void mergeSort(int arr[], int l, int r){ 
    if(l<r){ 
        // Same as (l+r)/2, but avoids overflow for large l and h 
        int m = l+(r-l)/2; 
        // Sort first and second halves 
        mergeSort(arr, l, m); 
        mergeSort(arr, m+1, r); 
        merge(arr, l, m, r); 
    } 
}
unsigned int *Next(char *file, int *c, int *sooc, int idx){
    unsigned int *next_idx = (unsigned int*)malloc(2*sizeof(unsigned int));
    next_idx[0] = 0;
    next_idx[1] = 0;
    for(unsigned int j = 1;j<256;j++){
        if(c[j]>=0){
            if(c[j]<idx){next_idx[0] = j;}else{break;} 
        }
    }
    int k = 1;
    char s = '\0';
    int ith = 0;
    //get indx in b of next_syb
    while(1){
        s = getSymbol(file,k);
        // printf("sooc %c %d\n",s,sooc[k-1]);
        if(s == (char)next_idx[0]){
            ith += sooc[k-1];
            // printf("%c %d %d\n",s,ith,idx-c[next_idx[0]]);
            // printf("%d %d\n",sooc[k-1],idx-c[next_idx[0]]);
            if(ith >= idx-c[next_idx[0]]){
                // printf("%c %d %d\n",s,next_idx[1],idx-c[next_idx[0]]);
                next_idx[1] += idx-c[next_idx[0]];
                break;
            }
        }else{
            next_idx[1] += sooc[k-1];
        }
        k++;
    }
    return next_idx;
}
void indexfolder(char *path){
    mkdir(path,0777);
}
void Dict(char *file,char *ext, char *writePath){
    FILE *fp1, *fp2;
    unsigned int count = 0;
    fp1 = fopen(strcat(file,ext),"rb");
    removeExt(file,strlen(ext));
    char *dictname;
    if(strlen(ext)==2){
        dictname = "/b.dic";
    }else{
        dictname = "/bb.dic";
    }
    fp2 = fopen(strcat(writePath,dictname),"wb");
    removeExt(writePath,strlen(dictname));
    unsigned int *bitBlock = (unsigned int*)malloc(25000*sizeof(unsigned int));
    int read_size = sizeof(unsigned int);
    int len_read = 1;//when file is smaller than 4 bytes, this will record the number of bytes read
    // printf("%ld\n", ftell(fp)); //tell the position of file pointer 
    if(fread(bitBlock,read_size,25000,fp1)==0){
        len_read = ftell(fp1);
        read_size = 1;
    }
    fseek( fp1, 0, SEEK_SET );
    while(fread(bitBlock,read_size,25000,fp1)){
        for(int i = 0;i<=ftell(fp1)/4;i++){
            for(int j = 0; j <read_size*8*len_read;j++){
                if (getBit(bitBlock[i],j) == 1){
                    count++;
                };
            }
        }
        fwrite(&count,sizeof(unsigned int),1,fp2);
    }
    fclose(fp1);
    fclose(fp2);
    free(bitBlock);
}
//summarize the occurrence of each symbol in s file
void SDict(char *file, char *writePath){
    FILE *fp = fopen(strcat(file,".s"),"rb");
    removeExt(file,2);
    FILE *fp1 = fopen(strcat(writePath,"/s.dic"),"wb");
    removeExt(writePath,strlen("/s.dic"));
    char s[1000] = {'\0'};
    unsigned int occ[128]={0};
    int size = 1000;
    while(fread(&s,sizeof(char),1000,fp)){
        if(ftell(fp)< 1000){
            size = ftell(fp);
        }
        if(ftell(fp)> 1000 && ftell(fp)%1000 != 0){
            size = ftell(fp)%1000;
        }
        for(int i = 0; i<size;i++){
            occ[(int)s[i]] ++;
            // printf("%c %u %u\n",s[i],occ[(int)s[i]],realRank(s[i],file,".s",writePath,i+1));
        }
        fwrite(occ,sizeof(unsigned int),128,fp1);
    }
    fclose(fp);
    fclose(fp1);
}
//return pos of block for the target number of 1s
unsigned int *getRankReadPos(char *writePath, char *filename,unsigned int num){
    FILE *fp = fopen(strcat(writePath,filename),"rb");
    removeExt(writePath,strlen(filename));
    unsigned int *count = (unsigned int*)malloc(2*sizeof(unsigned int));
    count[0] = 0;
    count[1] = 0;
    unsigned int tmp = 0;
    unsigned int pos = 0;
    int index = 0;
    while(fread(&tmp,sizeof(unsigned int),1,fp))
    {
        index += 25000*32;
        if( index>= num ){
            break;
        }
        pos++;
        count[1] = tmp;
    }
    count[0] = pos;
    fclose(fp);
    return count;
}
//return pos of block for the target number of 1s
unsigned int *getSlectReadPos(char *writePath, char *filename,unsigned int num){
    FILE *fp = fopen(strcat(writePath,filename),"rb");
    removeExt(writePath,strlen(filename));
    unsigned int *index = (unsigned int*)malloc(2*sizeof(unsigned int));
    index[0] = 0;
    index[1] = 0;
    unsigned int count = 0;
    unsigned int pos = 0;
    while(fread(&count,sizeof(unsigned int),1,fp))
    {
        if(count >= num ){
            index[0] = pos;
            break;
        }
        pos++;
        index[1] = count;
    }
    fclose(fp);
    return index;
}
unsigned int *SRankReadPos(char target, char *writePath, char *filename, unsigned int idx){
    FILE *fp = fopen(strcat(writePath,filename),"rb");
    removeExt(writePath,strlen(filename));
    unsigned int *count = (unsigned int*)malloc(2*sizeof(unsigned int));
    count[0] = 0;
    count[1] = 0;
    unsigned int tmp[128] = {0};
    unsigned int pos = 0;
    while(fread(tmp,sizeof(unsigned int),128,fp))
    {
        if(idx != -1){
            if( (pos+1)*1000<=idx ){
                pos++;
                count[1] = tmp[(int)target];
            }else{break;}
        }else{
            pos++;
            count[1] = tmp[(int)target];
        }
    }
    count[0] = pos;
    fclose(fp);
    return count;
}
unsigned int *SSelectReadPos(char target, char *writePath, char *filename, unsigned int num){
    FILE *fp = fopen(strcat(writePath,filename),"rb");
    removeExt(writePath,strlen(filename));
    unsigned int *count = (unsigned int*)malloc(2*sizeof(unsigned int));
    count[0] = 0;
    count[1] = 0;
    unsigned int tmp[128] = {0};
    unsigned int pos = 0;
    while(fread(tmp,sizeof(unsigned int),128,fp))
    {
        if(tmp[(int)target]>=num){
            break;
        }
        pos++;
        count[1] = tmp[(int)target];
    }
    count[0] = pos;
    fclose(fp);
    return count;
}
unsigned int realRank(char target,char *file,char *extension,char *writePath, unsigned int index){
    if(extension[1] == 's'){
        unsigned int *srrp = SRankReadPos(target,writePath,"/s.dic",index);
        if(index==-1){
            return srrp[1];
        }else{
            unsigned int sct = rank(target,file,extension,index-srrp[0]*1000,srrp[0]*1000);
            return srrp[1]+sct;
        }
    }else{
        char *dicname;
        if(strlen(extension)==2){
            dicname = "/b.dic";
        }else
        {
            dicname = "/bb.dic";
        }
        unsigned int *rcnt = getRankReadPos(writePath,dicname,index);
        unsigned int ct = rank(target,file,extension,index-rcnt[0]*25000*32,rcnt[0]*25000*4);
        return ct+rcnt[1];
    }
}
unsigned int realSelect(char target,char *file,char *extension,char *writePath, unsigned int num){
    if(extension[1] == 's'){
        unsigned int *ssrp = SSelectReadPos(target,writePath,"/s.dic",num);
        unsigned int ssidx = slect(target,file,extension,num-ssrp[1],ssrp[0]*1000);
        return ssidx+ssrp[0]*1000;
    }else{
        char *dicname;
        if(strlen(extension)==2){
            dicname = "/b.dic";
        }else
        {
            dicname = "/bb.dic";
        }
        unsigned int *slidx = getSlectReadPos(writePath,dicname,num);
        unsigned int sidx = slect(target,file,extension,num-slidx[1],slidx[0]*25000*4);
        return sidx+slidx[0]*25000*32;
    }
}
unsigned int LF(char * file, unsigned int n, char symb,unsigned int *cs, char *writePath){
    unsigned int tmp = realRank(1,file,".b",writePath,n);
    return realSelect(1,file,".bb",writePath,cs[(int)symb]+ realRank(symb,file,".s",writePath,tmp)) + n - realSelect(1,file,".b",writePath,tmp);
}
//input a index and its symbol, output the number of rows before in F table
//consists of two parts:
//     //1. all symbols smaller than it
//     //2. all symbols same as it but before idx in s
unsigned int RowsBef(char *file,int index, char syb, char *writePath){
    FILE *fp1;
    fp1 = fopen(strcat(file,".s"),"rb");
    removeExt(file,2);
    unsigned char *c = (unsigned char*)malloc(8000000*sizeof(unsigned char));
    unsigned int idx = 0;
    unsigned int sum = 0;
    while(fread(c,sizeof(unsigned char),8000000,fp1)){
        for(int i = 0;i<ftell(fp1);i++){
            idx++;
            index--;
            if((int)c[i]<(int)syb){
                sum += realSelect(1,file,".b",writePath,idx+1)-realSelect(1,file,".b",writePath,idx);
            }
            if((int)c[i] == (int)syb && index>0){
                sum += realSelect(1,file,".b",writePath,idx+1)-realSelect(1,file,".b",writePath,idx);
            }
        }
    }
    fclose(fp1);
    free(c);
    return sum;
}
unsigned int *backwardSearch(char *file,unsigned int *cst,char *identifier,char *writePath){
    unsigned int *point = (unsigned int*)malloc(2*sizeof(unsigned int));
    int len = strlen(identifier);
    for(int i=len-1;i>-1;i--){
        //remember gap-filler 1s in b and bb, so position has to be positive for rank function in b and bb 
        if(i == len-1){
            point[0] = realSelect(1,file,".bb",writePath,cst[(int)identifier[i]]+1);
            //take negative input as the position argument to get the occurrence of first letter 
            //when doing backward search
            point[1] = realSelect(1,file,".bb",writePath,cst[(int)identifier[i]]+1+realRank(identifier[i],file,".s",writePath,-1))-1;
            // printf("ss %d %d\n",point[0],point[1]);
        }else{
            //cs to get # of 1s before
            //realRank to get the # of occ of current symbol up to the cloest 1
            //point[0]-realSelect(1,file,".b",realRank(1,file,".b",point[0])) to get the i-th current symbol if it is 0 in b
            if(getSymbol(file,realRank(1,file,".b",writePath,point[0]))==identifier[i] && (realRank(1,file,".b",writePath,point[0])-realRank(1,file,".b",writePath,point[0]-1)==0)){
                unsigned int cn = realRank(1,file,".b",writePath,point[0]);
                point[0] = realSelect(1,file,".bb",writePath,cst[(int)identifier[i]]+realRank(identifier[i],file,".s",writePath,cn))+point[0]-realSelect(1,file,".b",writePath,cn);
            }else{
                // printf("%u\n",realRank(1,file,".b",writePath,point[0]-1));
                // printf("%u\n",realRank(identifier[i],file,".s",writePath,realRank(1,file,".b",writePath,point[0]-1)));
                point[0] = realSelect(1,file,".bb",writePath,cst[(int)identifier[i]]+1+realRank(identifier[i],file,".s",writePath,realRank(1,file,".b",writePath,point[0]-1)));
            }
            
            if(
                getSymbol(file,realRank(1,file,".b",writePath,point[1]))==identifier[i] 
                &&(realRank(1,file,".b",writePath,point[1])-realRank(1,file,".b",writePath,point[1]-1)==0)
                // &&(realRank(1,file,".b",point[1]-1)-realRank(1,file,".b",point[1]-2)>0)
            ){
                int cloest_one = realSelect(1,file,".b",writePath,realRank(1,file,".b",writePath,point[1]));
                point[1] = realSelect(1,file,".bb",writePath,cst[(int)identifier[i]]+realRank(identifier[i],file,".s",writePath,realRank(1,file,".b",writePath,point[1])))+point[1]-cloest_one;

            }else{
                point[1] = realSelect(1,file,".bb",writePath,cst[(int)identifier[i]]+1+realRank(identifier[i],file,".s",writePath,realRank(1,file,".b",writePath,point[1])))-1;
            }
            // printf("ss %d %d\n",point[0],point[1]);
        }
    }
    return point;
}