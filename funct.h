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
        char c[400];
        //when position smaller than 0, 
        //then this function can achievev the purpose of tackling the problem of counting occurrence of first symbol
        //when doing backward search
        //because this will block the "if break" control flow
        while(fread(&c,sizeof(char),400,fp)){
            for(int i = 0;i<400;i++){
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
    }else{//rank in b/bb file
        unsigned int bitBlock[100];
        int read_size = sizeof(unsigned int);
        int len_read = 1;//when file is smaller than 4 bytes, this will record the number of bytes read
        if(fread(bitBlock,read_size,100,fp)==0){
            len_read = ftell(fp);
            read_size = 1;
        }
        fseek( fp, 0, SEEK_SET );
        while(fread(bitBlock,read_size,100,fp)){
            for(int i = 0;i<100;i++){
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
    }
    fclose(fp);
    return count;
}
unsigned int select(char target,char* file,char *extsn, int count){
    int index = 0;
    FILE *fp = fopen(strcat(file,extsn),"r");
    removeExt(file,strlen(extsn));
    if(extsn[1]=='s'){
        char c[400];
        while(fread(&c,sizeof(char),400,fp)){
            for(int i = 0;i<400;i++){
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
    }else{//rank in b/bb file
        unsigned int bitBlock[100];
        int read_size = sizeof(unsigned int);
        int len_read = 1;//when file is smaller than 4 bytes, this will record the number of bytes read
        // printf("%ld\n", ftell(fp)); //tell the position of file pointer 
        if(fread(bitBlock,read_size,100,fp)==0){
            len_read = ftell(fp);
            read_size = 1;
        }
        fseek( fp, 0, SEEK_SET );
        while(fread(bitBlock,read_size,100,fp)){
            for(int i = 0;i<100;i++){
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
    }
    fclose(fp);
    return index;
};
//get symbol from s file
//coincidently, when return '\0', it indicates that we are reading gap-fillers 1s in b
char getSymbol(char *file, int count){
    FILE *fp = fopen(strcat(file,".s"),"rb");
    removeExt(file,2);
    char c = fgetc(fp);
    while(c!=EOF){
        count--;
        if(count == 0){
            fclose(fp);
            return c;
        }
        c = fgetc(fp);
    }
    fclose(fp);
    return '\0';
}
//input a index and its symbol, output the number of rows before in F table
//consists of two parts:
    //1. all symbols smaller that it
    //2. all symbols same as it but before idx in s
int RowsBef(char *file,int index, char syb){
    FILE *fp1;
    fp1 = fopen(strcat(file,".s"),"rb");
    removeExt(file,2);
    char c = fgetc(fp1);
    int idx = 0;
    int sum = 0;
    while(c!=EOF){
        idx++;
        index--;
        if((int)c<(int)syb){
            sum += select(1,file,".b",idx+1)-select(1,file,".b",idx);
        }
        if((int)c == (int)syb && index>0){
            sum += select(1,file,".b",idx+1)-select(1,file,".b",idx);
        }
        c = fgetc(fp1);
    }
    fclose(fp1);
    return sum;
}