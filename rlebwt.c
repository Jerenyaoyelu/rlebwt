#include<stdio.h>

#include "funct.h"

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
        printf("count %d\n",rank(1,argv[2],".bb",11));
        printf("index %d\n",select(1,argv[2],".bb",7));
    }
    return 0;
}