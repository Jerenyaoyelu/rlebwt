#include<stdio.h>
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
    removeExt(file,2);
    //rank in s file
    if(extsn[1]=='s'){
        count = 0;
    }else{//rank in b/bb file
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
// int select(char target,char* file, int count){
//     return 0;
// };
void CsTable(char *file){//file is command argument
    FILE *fp = fopen(strcat(file,".s"),"r");
    char s = fgetc(fp);
    int temp[256]={0};
    while(s!=EOF){
        temp[(int)s] ++;
        s = fgetc(fp);
    }
    fclose(fp);
    int sum = 0;
    fp = fopen("cs.idx","a+");
    for(int i = 0;i<256;i++){
        if(temp[i]>0){
            fputc(i,fp);
            fputc(sum,fp);
            sum += temp[i];
        }
    }
    fclose(fp);
}
// void OccTable(char *file){
//     FILE *fp = fopen(strcat(file,".s"),"r");

// }
int main(int argc, char* argv[]){
    // strcpy(filepath,argv[2]);
    FILE *fp;
    int bb;
    char *pattern = argv[argc-1];
    if(!strcmp(argv[1],"-m")){
        CsTable(argv[2]);
        removeExt(argv[2],2);
        // if((fp = fopen(strcat(argv[2],".b"),"rb"))){
        //     removeExt(argv[2],2);
        //     bb = fgetc(fp);
        //     while(bb!=EOF){
        //         for(int i = 7;i>-1;i--){
        //             printf("%d",getBit(bb,i));
        //         }
        //         printf("\n");
        //         bb = fgetc(fp);
        //     }
        //     fclose(fp);
        // }
        printf("count %d\n",rank(1,argv[2],".b",15));
    }
    return 0;
}