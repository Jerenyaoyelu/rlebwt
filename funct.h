#include<string.h>

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
int rank(char target, char *file,char *extsn, int position){
    int count = 0;
    FILE *fp = fopen(strcat(file,extsn),"rb");
    removeExt(file,strlen(extsn));
    //rank in s file
    if(extsn[1]=='s'){
        unsigned char *c = (unsigned char*)malloc(8000000*sizeof(unsigned char));
        //when position smaller than 0, 
        //then this function can achievev the purpose of tackling the problem of counting occurrence of first symbol
        //when doing backward search
        //because this will block the "if break" control flow
        while(fread(c,sizeof(unsigned char),8000000,fp)){
            for(int i = 0;i<ftell(fp);i++){
                position--;
                if (c[i] == target){
                    count++;
                };
                if(position == 0){
                    break;
                }
            }
            if(position == 0){
                break;
            }
        }
        free(c);
    }else{//rank in b/bb file
        unsigned int *bitBlock = (unsigned int*)malloc(2000000*sizeof(unsigned int));
        int read_size = sizeof(unsigned int);
        int len_read = 1;//when file is smaller than 4 bytes, this will record the number of bytes read
        if(fread(bitBlock,read_size,2000000,fp)==0){
            len_read = ftell(fp);
            read_size = 1;
        }
        fseek( fp, 0, SEEK_SET );
        while(fread(bitBlock,read_size,2000000,fp)){
            for(int i = 0;i<=ftell(fp)/4;i++){
                for(int j = 0; j <read_size*8*len_read;j++){
                    position--;
                    if (getBit(bitBlock[i],j) == (int)target){
                        count++;
                    };
                    if(position == 0){
                        break;
                    }
                }
                if(position == 0){
                    break;
                }
            }
            if(position == 0){
                break;
            }
        }
        free(bitBlock);
    }
    fclose(fp);
    return count;
}
unsigned int select(char target,char* file,char *extsn, int count){
    int index = 0;
    FILE *fp = fopen(strcat(file,extsn),"r");
    removeExt(file,strlen(extsn));
    if(extsn[1]=='s'){
        unsigned char *c = (unsigned char*)malloc(8000000*sizeof(unsigned char));
        while(fread(c,sizeof(unsigned char),8000000,fp)){
            for(int i = 0;i<ftell(fp);i++){
                if (c[i] == target){
                    count--;
                };
                index++;
                if(count == 0){
                    break;
                }
            }
            if(count == 0){
                break;
            }
        }
        free(c);
    }else{//rank in b/bb file
        unsigned int *bitBlock = (unsigned int*)malloc(2000000*sizeof(unsigned int));
        int read_size = sizeof(unsigned int);
        int len_read = 1;//when file is smaller than 4 bytes, this will record the number of bytes read
        // printf("%ld\n", ftell(fp)); //tell the position of file pointer 
        if(fread(bitBlock,read_size,2000000,fp)==0){
            len_read = ftell(fp);
            read_size = 1;
        }
        fseek( fp, 0, SEEK_SET );
        while(fread(bitBlock,read_size,2000000,fp)){
            for(int i = 0;i<=ftell(fp)/4;i++){
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
            if(count == 0){
                break;
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
char getSymbol(char *file, int count){
    // char res;
    FILE *fp = fopen(strcat(file,".s"),"rb");
    removeExt(file,2);
    // char *c = (char*)malloc(4000000*sizeof(char));
    char c = '\0';
    fseek(fp,count-1,SEEK_SET);
    fread(&c,sizeof(char),1,fp);
    fclose(fp);
    // if(count > ftell(fp)){
    //     res = '\0';
    // }else{
    //     res = c[count-1];
    // }
    // free(c);
    return c;
}
//input a index and its symbol, output the number of rows before in F table
//consists of two parts:
    //1. all symbols smaller that it
    //2. all symbols same as it but before idx in s
int RowsBef(char *file,int index, char syb){
    FILE *fp1;
    fp1 = fopen(strcat(file,".s"),"rb");
    removeExt(file,2);
    unsigned char *c = (unsigned char*)malloc(8000000*sizeof(unsigned char));
    int idx = 0;
    int sum = 0;
    while(fread(c,sizeof(unsigned char),8000000,fp1)){
        for(int i = 0;i<ftell(fp1);i++){
            idx++;
            index--;
            if((int)c[i]<(int)syb){
                sum += select(1,file,".b",idx+1)-select(1,file,".b",idx);
            }
            if((int)c[i] == (int)syb && index>0){
                sum += select(1,file,".b",idx+1)-select(1,file,".b",idx);
            }
        }
    }
    fclose(fp1);
    free(c);
    return sum;
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
int LF(char * file, int n, char symb,int *cs){
    int tmp = rank(1,file,".b",n);
    return select(1,file,".bb",cs[(int)symb]+ rank(symb,file,".s",tmp)) + n - select(1,file,".b",tmp);
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
int *Next(char *file, int *c, int *sooc, int idx){
    int *next_idx = (int*)malloc(2*sizeof(int));
    next_idx[0] = 0;
    next_idx[1] = 0;
    for(int j = 1;j<256;j++){
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
int *backwardSearch(char *file,int *cst,char *identifier){
    int *point = (int*)malloc(2*sizeof(int));
    int len = strlen(identifier);
    for(int i=len-1;i>-1;i--){
        //remember gap-filler 1s in b and bb, so position has to be positive for rank function in b and bb 
        if(i == len-1){
            point[0] = select(1,file,".bb",cst[(int)identifier[i]]+1);
            //take negative input as the position argument to get the occurrence of first letter 
            //when doing backward search
            point[1] = select(1,file,".bb",cst[(int)identifier[i]]+1+rank(identifier[i],file,".s",-1))-1;
            // printf("%d %d\n",point[0],point[1]);
        }else{
            //cs to get # of 1s before
            //rank to get the # of occ of current symbol up to the cloest 1
            //point[0]-select(1,file,".b",rank(1,file,".b",point[0])) to get the i-th current symbol if it is 0 in b
            int cloest_one = select(1,file,".b",rank(1,file,".b",point[0]));
            point[0] = select(1,file,".bb",cst[(int)identifier[i]]+1+rank(identifier[i],file,".s",rank(1,file,".b",cloest_one-1)))+(point[0]-cloest_one);
            point[1] = select(1,file,".bb",cst[(int)identifier[i]]+1+rank(identifier[i],file,".s",rank(1,file,".b",point[1])))-1;
            // printf("ss %d %d\n",point[0],point[1]);
        }
    }
    return point;
}