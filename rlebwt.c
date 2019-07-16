#include<stdio.h>
#include<string.h>


// int rank(char target, char *file, int position){
//     int index;
//     FILE *fp = fopen(fp,"rb");
//     fclose(fp);
//     return index;
// }
// int select(char target,char* file, int count){
//     return 0;
// };
void generatingC(char *file){
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
int main(int argc, char* argv[]){
    char* filePath;
    // strcpy(filepath,argv[2]);
    FILE *fp;
    int bb;
    char *pattern = argv[argc-1];
    if(!strcmp(argv[1],"-m")){
        generatingC(argv[2]);
        // if((fp = fopen(strcat(filePath,".bb"),"rb"))){
        //     bb = fgetc(fp);
        //     while(bb!=EOF){
        //         printf("%d\n",bb);
        //         bb = fgetc(fp);
        //     }
        //     fclose(fp);
        // }
        // printf("%s\n",argv[2])
    }
    return 0;
}