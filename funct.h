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
//coincidently, when return '\0', it indicates that we are reading gap-fillers 1s in b
char getSymbol(char *file, int count){
    FILE *fp = fopen(strcat(file,".s"),"rb");
    removeExt(file,2);
    char *c = (char*)malloc(4000000*sizeof(char));
    fread(c,sizeof(char),4000000,fp);
    fclose(fp);
    if(count > strlen(c)){
        return '\0';
    }else{
        return c[count-1];
    }
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
int compare( const void* a, const void* b){
     int int_a = * ( (int*) a );
     int int_b = * ( (int*) b );
     return (int_a > int_b) - (int_a < int_b);
}
int LF(char * file, int n, char symb,int *cs){
    int tmp = rank(1,file,".b",n);
    return select(1,file,".bb",cs[(int)symb]+ rank(symb,file,".s",tmp)) + n - select(1,file,".b",tmp);
}