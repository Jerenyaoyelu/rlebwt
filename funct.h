#include<string.h>

// get n continuous bits from source starting at the offset bit
// (source >> offset) & ((1 << n) - 1) 
// offset 0 is the rightmost bit
int getBit(int num,int pos){
    return (num>>pos)&((1<<1)-1);
}
void removeExt(char *filename,int num){
    int len =strlen(filename);
    filename[len-num] = '\0';
}
int rank(char target, char *file,char *extsn, int position){
    int count = 0;
    FILE *fp = fopen(strcat(file,extsn),"r");
    removeExt(file,strlen(extsn));
    //rank in s file
    if(extsn[1]=='s'){
        char c = '\0';
        //when position smaller than 0, 
        //then this function can achievev the purpose of tackling the problem of counting occurrence of first symbol
        //when doing backward search
        //because this will block the "if break" control flow
        while(fread(&c,sizeof(c),1,fp)){
            position--;
            if (c == target){
                count++;
            };
            if(position == 0){
                break;
            }
        }
    }else{//rank in b/bb file
        //if using fread, will get wrong answer, probably it is because the binary is read chunk by chunk, 
        //which is easliy to count wrong?
        int bitBlock = fgetc(fp);
        while(bitBlock!=EOF){
            for(int i = 7;i>-1;i--){
                if(position == 0){
                    break;
                }
                position--;
                if (getBit(bitBlock,i) == (int)target){
                    count++;
                };
            }
            if(position == 0){
                break;
            }
            bitBlock = fgetc(fp);
        }
    }
    fclose(fp);
    return count;
}
int select(char target,char* file,char *extsn, int count){
    int index = 0;
    FILE *fp = fopen(strcat(file,extsn),"r");
    removeExt(file,strlen(extsn));
    if(extsn[1]=='s'){
        char c = fgetc(fp);
        while(c!=EOF){
            if (c == target){
                count--;
            };
            index++;
            if(count == 0){
                break;
            }
            c = fgetc(fp);
        }
    }else{//rank in b/bb file
        int bitBlock = fgetc(fp);
        while(bitBlock!=EOF){
            for(int i = 7;i>-1;i--){
                if(count == 0){
                    break;
                }
                index++;
                if (getBit(bitBlock,i) == (int)target){
                    count--;
                };
            }
            if(count == 0){
                break;
            }
            bitBlock = fgetc(fp);
        }
    }
    fclose(fp);
    return index;
};
//get symbol from s file
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