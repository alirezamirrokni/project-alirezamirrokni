#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <dir.h>
#include <conio.h>
#include <sys/stat.h>
#define mxn 1000
#define mxn1 500
int placeHolder[mxn1] , lenPlaceHolder , sizeHolder[mxn1];
char clipBoard[mxn] , undoFiles[mxn1][mxn1] , cntUndoFiles;
void intToStr(int a , char ans[]){
    char tmp[mxn1] = "\0";
    if(!a){
        strcat(ans,"0");
        return;
    }
    while(a){
        char c = (a%10)+'0';
        strncat(tmp,&c,1) , a/=10;
    }
    for(int i = strlen(tmp)-1 ; i>=0 ; i--)strncat(ans,tmp+i,1);
}
char** create2Darray(){
    char** out = (char**)malloc(sizeof(char*)*mxn1);
    for(int i=0 ; i<mxn1 ; i++)*(out+i) = (char*)malloc(sizeof(char)*mxn1);
    return out;
}
int max(int a , int b){
    return (a>b)*a + (a<=b)*b;
}
int min(int a, int b){
    return (a>b)*b + (b>=a)*a;
}
void split(char str[] , char c , char ans[][mxn1]){
    char** out = create2Darray();
    int cnt1=0 , cnt2=0;
    for(int i=0 ; i<strlen(str)+1 ; i++){
        if(str[i]==c || i==strlen(str))*(*(out+cnt1)+cnt2)='\0' , cnt1++ , cnt2=0 ;
        else *(*(out+cnt1)+cnt2)=str[i] , cnt2++ ;
    }
    *(out+cnt1)=NULL;
    convert2Darray(ans,out);
}
int charcnt(char str[] , char c){
    int ans=0;
    for(int i=0 ; i<strlen(str) ; i++)if(str[i]==c)ans++;
    return ans+1;
}
void convert2Darray(char arr[][mxn1] , char** tmp){
    int cnt1=0 , cnt2=0;
    while(*(tmp+cnt1)!=NULL){
        while(*(*(tmp+cnt1)+cnt2)!='\0')arr[cnt1][cnt2] = *(*(tmp+cnt1)+cnt2) , cnt2++;
        cnt1++ , cnt2=0;
    }
}
int validPath(char path[]){
    char dirs[mxn1][mxn1] = {'\0'} , pathtmp[mxn1]="\0";
    split(path , '/' , dirs);
    int n = charcnt(path , '/');
    if(strcmp(dirs[0],"root"))return 0;
    strcpy(pathtmp,dirs[0]);
    DIR *folder;
    struct dirent *entry;
    for(int i = 1; i<n-1 ; i++){
        int flag = 0 , len = strlen(pathtmp) , folder = opendir(pathtmp);
        while(entry=readdir(folder)){
            if(!strcmp(dirs[i],entry->d_name)){
                flag=1;
                break;
            }
        }
        if(!flag)return 0;
    }
    FILE * fp = fopen(path , "r");
    if(!fp){
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 1;
}
int validPos(i);
void startProgram(){
    mkdir("root");
    mkdir("undoFiles");
}
void undoSaver(char path[]){
    char undopath[mxn1]="undoFiles/" , content[mxn]="\0";
    for(int i = 5;i<strlen(path);i++)(path[i]=='\\')?strcat(undopath,"-") : strncat(undopath,path+i,1);
    if(!validPath(undopath))strcpy(undoFiles[cntUndoFiles++],undopath);
    FILE * fp1 = fopen(undopath , "w+") , * fp2 = fopen(path , "r");
    while(fgets(content,mxn,fp2)!=NULL)fputs(content,fp1);
    fclose(fp1);
    fclose(fp2);
}
void create(char path[]){
    char dirs[mxn1][mxn1] = {'\0'} , pathtmp[mxn1]="\0";
    split(path , '/' , dirs);
    int n = charcnt(path , '/');
    strcpy(pathtmp,dirs[0]);
    DIR *folder;
    struct dirent *entry;
    for(int i = 1; i<n ; i++){
        int flag = 0 , len = strlen(pathtmp) , folder = opendir(pathtmp);
        while(entry=readdir(folder)){
            if(!strcmp(dirs[i],entry->d_name)){
                flag=1;
                break;
            }
        }
        closedir(pathtmp) , pathtmp[len++] = '/';
        strcat(pathtmp , dirs[i]);
        if(flag && i==n-1){
            printf("Invalid command : This file already exists\n");
            return;
        }
        if(flag)continue;
        if(i<n-1)mkdir(pathtmp);
        else{
            FILE *fp;
            fp = fopen (pathtmp, "w");
            fclose(fp);
        }
    }
    printf("Your file has been successfully created\n");
}
void insert(char path[] , char content[] , int line , int pos){
    if(!validPath(path)){
        printf("Invalid command : wrong address\n");
        return;
    }
    if(validPath(path)==-1){
        printf("Invalid command : requested file does not exist\n");
        return;
    }
    undoSaver(path);
    FILE * fp , * tmp;
    fp = fopen(path , "r+") , tmp = fopen("replace.tmp" , "w+") ;
    char contenttmp[mxn1]="\0" , *buffer;
    int cnt=0;
    while((buffer = fgets(contenttmp , mxn1 ,fp))!=NULL || cnt<=line){
        cnt++;
        if(buffer == NULL){
            if(cnt!=line)fputs("\n" , tmp);
            else{
                char rep[mxn]="\0";
                for(int i=0;i<pos;i++)rep[i] = ' ';
                for(int i=pos;i<pos+strlen(content);i++)rep[i] = content[i-pos];
                if(rep[strlen(rep)-1]!='\n')rep[strlen(rep)] = '\n';
                fputs(rep,tmp);
            }
            continue;
        }
        if(cnt == line){
            char rep[mxn] = "\0";
            for(int i=0;i<min(pos,strlen(contenttmp)-1);i++)rep[i] = contenttmp[i];
            for(int i=min(pos,strlen(contenttmp)-1);i<pos ;i++)rep[i] = ' ';
            for(int i=pos;i<pos+strlen(content);i++)rep[i] = content[i-pos];
            for(int i=pos+strlen(content);i<strlen(content)+strlen(contenttmp);i++)rep[i] = contenttmp[i-strlen(content)];
            if(rep[strlen(rep)-1]!='\n')rep[strlen(rep)] = '\n';
            fputs(rep,tmp);
        }
        else {
            if(contenttmp[strlen(contenttmp)-1]!='\n')contenttmp[strlen(contenttmp)] = '\n';
            fputs(contenttmp,tmp);
        }
    }
    fclose(fp) ,  fclose(tmp) , remove(path) , rename("replace.tmp",path);
    printf("Your input has been successfully inserted in the requested position\n");
}
void cat(char path[],char armn[],int fromArman){
    if(!validPath(path)){
        printf("Invalid command : wrong address\n");
        return;
    }
    if(validPath(path)==-1){
        printf("Invalid command : requested file does not exist\n");
        return;
    }
    FILE * fp;
    fp = fopen(path , "r");
    char content[mxn1] = "\0";
    while(fgets(content , mxn , fp)!=NULL){
        if(fromArman)strcat(armn,content);
        else printf("%s" , content);
    }
    fclose(fp);
}
void rmv(char path[] , int line , int pos , int size , char attribute , int fromCut , int fromRep){
    if(!fromCut && !fromRep && !validPath(path)){
        printf("Invalid command : wrong address\n");
        return;
    }
    if(!fromCut && !fromRep && validPath(path)==-1){
        printf("Invalid command : requested file does not exist\n");
        return;
    }
    if(!fromRep)undoSaver(path);
    FILE * fp , * tmp , *tmp1 ;
    fp = fopen(path , "r+") , tmp = fopen("replace.tmp" , "w+"), tmp1 = fopen("remove.txt" , "w+");
    char contenttmp[mxn1]="\0";
    int cnt=0 , charNum = 0;
    while(fgets(contenttmp , mxn , fp)!=NULL){
        cnt++ , charNum+=strlen(contenttmp);
        fputs(contenttmp,tmp1);
        if(cnt == line-1)pos += charNum;
    }
    fclose(tmp1) , tmp1 = fopen("remove.txt" , "r") , charNum=0;
    if(attribute == 'f'){
        while(fgets(contenttmp , mxn , tmp1)!=NULL){
            for(int i=0;i<strlen(contenttmp);i++){
                if(charNum<pos || charNum>=pos+size)fputc(contenttmp[i],tmp);
                charNum++;
            }
        }
    }
    else{
        while(fgets(contenttmp , mxn , tmp1)!=NULL){
            for(int i=0;i<strlen(contenttmp);i++){
                if(charNum<pos-size || charNum>=pos)fputc(contenttmp[i],tmp);
                charNum++;
            }
        }
    }
    fclose(fp) , fclose(tmp) , fclose(tmp1) , remove(path) , remove("remove.txt") , rename("replace.tmp",path);
    if(!fromCut)printf("successfully deleted\n");
}
void copy(char path[] , int line , int pos , int size , char attribute , int fromCut){
    if(!fromCut && !validPath(path)){
        printf("Invalid command : No such file or directory exists\n");
        return;
    }
    FILE * fp , *tmp1;
    fp = fopen(path , "r+") , tmp1 = fopen("copy.txt" , "w+");
    char contenttmp[mxn1]="\0";
    int cnt=0 , charNum = 0;
    while(fgets(contenttmp , mxn , fp)!=NULL){
        cnt++ , charNum+=strlen(contenttmp);
        fputs(contenttmp,tmp1);
        if(cnt == line-1)pos += charNum;
    }
    fclose(tmp1) , tmp1 = fopen("copy.txt" , "r") , charNum=0;
    strcpy(clipBoard," ");
    if(attribute == 'f'){
        while(fgets(contenttmp , mxn , tmp1)!=NULL){
            for(int i=0;i<strlen(contenttmp);i++){
                if(charNum>=pos && charNum<pos+size)clipBoard[charNum-pos] = contenttmp[i];
                charNum++;
            }
        }
    }
    else{
        while(fgets(contenttmp , mxn , tmp1)!=NULL){
            for(int i=0;i<strlen(contenttmp);i++){
                if(charNum>=pos-size && charNum<pos)clipBoard[charNum-pos+size] = contenttmp[i];
                charNum++;
            }
        }
    }
    fclose(fp)  , fclose(tmp1) , remove("copy.txt");
    printf("successfully copied into clipboard\n");
}
void cut(char path[] , int line , int pos , int size , char attribute){
    if(!validPath(path)){
        printf("Invalid command : wrong address\n");
        return;
    }
    if(validPath(path)==-1){
        printf("Invalid command : requested file does not exist\n");
        return;
    }
    copy(path,line,pos,size,attribute,1);
    rmv(path,line,pos,size,attribute,1,0);
}
void paste(char path[] , int line , int pos){
    insert(path , clipBoard , line , pos);
}
void find(char path[] , char content[] , int count , int at , int byword , int all , int fromReplace , char armn[] , int fromArman,int flag){
    if(!fromReplace && !validPath(path)){
        printf("Invalid command : wrong address\n");
        return;
    }
    if(!fromReplace && validPath(path)==-1){
        printf("Invalid command : requested file does not exist\n");
        return;
    }
    if(at&&all){
        printf("Invalid command : Two incompatible attributes\n");
        return;
    }
    FILE * fp;
    fp = fopen(path , "r") , lenPlaceHolder=0;
    char contentHolder[mxn1]="\0"  , sub[mxn1]="\0";
    int bywordHolder[mxn1] , len = strlen(content) , cnt=1 , numOfChars=0 , lastSpace=-1;
    if(flag==1){
        for(int i=1;i<len;i++)sub[i-1]=content[i];
        len--;
    }
    else if(flag==2){
        for(int i=0;i<len-1;i++)sub[i]=content[i];
        len--;
    }

    if(!flag){
        while(fgets(contentHolder , mxn1 , fp)!=NULL){
            if(contentHolder[strlen(contentHolder)-1]!='\n')strcat(contentHolder,"\n");
            int length = strlen(contentHolder);
            for(int i=0;i<length-len+1;i++){
                if(contentHolder[i]==' ')cnt++;
                char substitute[mxn1] = "\0";
                if(contentHolder[i+len]!=' ' && contentHolder[i+len]!='\n')continue;
                if(i&&contentHolder[i-1]!=' '&&contentHolder[i-1]!='\n')continue;
                for(int j=i;j<i+len;j++)substitute[j-i]=contentHolder[j];
                if(!strcmp(substitute,content))placeHolder[lenPlaceHolder]=i+numOfChars , bywordHolder[lenPlaceHolder]=cnt , sizeHolder[lenPlaceHolder] = len , lenPlaceHolder++;
            }
            numOfChars+=length,cnt++ , lastSpace=numOfChars;
        }
    }
    else if(flag==1){
        while(fgets(contentHolder , mxn1 , fp)!=NULL){
            if(contentHolder[strlen(contentHolder)-1]!='\n')strcat(contentHolder,"\n");
            int length = strlen(contentHolder);
            for(int i=0;i<length-len+1;i++){
                if(contentHolder[i]==' ')cnt++ , lastSpace = i+numOfChars;
                if(contentHolder[i+len]!=' '&&contentHolder[i+len]!='\n')continue;
                char substitute[mxn1] = "\0";
                for(int j=i;j<i+len;j++)substitute[j-i]=contentHolder[j];
                if(!strcmp(substitute,sub))placeHolder[lenPlaceHolder]=lastSpace+1 , bywordHolder[lenPlaceHolder]=cnt , sizeHolder[lenPlaceHolder] = i+numOfChars-lastSpace+len-1  , lenPlaceHolder++;
            }
            numOfChars+=length , cnt++ ,lastSpace=numOfChars;
        }
    }
    else if(flag==2){
        while(fgets(contentHolder , mxn1 , fp)!=NULL){
            if(contentHolder[strlen(contentHolder)-1]!='\n')strcat(contentHolder,"\n");
            int length = strlen(contentHolder);
            for(int i=0;i<length-len+1;i++){
                if(contentHolder[i]==' ')cnt++;
                if(i&&contentHolder[i-1]!=' '&&contentHolder[i-1]!='\n')continue;
                char substitute[mxn1] = "\0";
                for(int j=i;j<i+len;j++)substitute[j-i]=contentHolder[j];
                if(!strcmp(substitute,sub)){
                    if(fromReplace){
                        int j=i;
                        for(j;j<length;j++)if(contentHolder[j]==' ' || contentHolder[j]=='\n')break;
                        sizeHolder[lenPlaceHolder]=j-i;
                    }
                    placeHolder[lenPlaceHolder]=i+numOfChars , bywordHolder[lenPlaceHolder]=cnt , lenPlaceHolder++;
                }
            }
            numOfChars+=length , cnt++ , lastSpace = numOfChars;
        }
    }
    if(!lenPlaceHolder&&!fromReplace){
        if(fromArman)strcat(armn,"-1");
        else printf("-1\n");
        fclose(fp);
        return;
    }
    if(count){
        if(fromArman) intToStr(lenPlaceHolder,armn);
        else printf("%d\n",lenPlaceHolder);
    }
    if(at){
        if(at>lenPlaceHolder){
            if(fromArman)strcat(armn,"-1");
            else printf("-1\n");
            fclose(fp);
            return;
        }
        at--;
        if(byword){
            if(fromArman)intToStr(bywordHolder[at],armn);
            else printf("%d\n",bywordHolder[at]);
        }
        else{
            if(fromArman)intToStr(placeHolder[at],armn);
            else printf("%d\n",placeHolder[at]);
        }
    }
    if(all){
        if(byword){
            if(fromArman){
                for(int i=0;i<lenPlaceHolder-1;i++)intToStr(bywordHolder[i],armn) , strcat(armn,", ");
                intToStr(bywordHolder[lenPlaceHolder-1],armn);
            }
            else{
                for(int i=0;i<lenPlaceHolder-1;i++)printf("%d,",bywordHolder[i]);
                printf("%d\n" , bywordHolder[lenPlaceHolder-1]);
            }

        }
        else{
            if(!fromReplace){
                if(fromArman){
                    for(int i=0;i<lenPlaceHolder-1;i++)intToStr(placeHolder[i],armn) , strcat(armn,", ");
                    intToStr(placeHolder[lenPlaceHolder-1],armn);
                }
                else{
                    for(int i=0;i<lenPlaceHolder-1;i++)printf("%d,",placeHolder[i]);
                    printf("%d\n" , placeHolder[lenPlaceHolder-1]);
                }
            }
        }
    }
    if(!at&&!all && !count){
        if(byword){
            if(fromArman)intToStr(bywordHolder[0],armn);
            else printf("%d\n",bywordHolder[0]);
        }
        else{
            if(fromArman)intToStr(placeHolder[0],armn);
            else printf("%d\n",placeHolder[0]);
        }
    }
    fclose(fp);
}
void insertForReplace(char path[] , char content[] , int pos){
    FILE * fp , * tmp;
    fp = fopen(path , "r+") , tmp = fopen("replace.tmp" , "w+") ;
    char contenttmp[mxn1]="\0" , *buffer;
    int cnt=0 , flag = 0;
    while(fgets(contenttmp , mxn1 ,fp)!=NULL){
        cnt+=strlen(contenttmp);
        if(cnt>pos && !flag){
            flag = 1;
            pos+=strlen(contenttmp)-cnt;
            char rep[mxn1] = "\0";
            for(int i=0;i<min(pos,strlen(contenttmp)-1);i++)rep[i] = contenttmp[i];
            for(int i=min(pos,strlen(contenttmp)-1);i<pos ;i++)rep[i] = ' ';
            for(int i=pos;i<pos+strlen(content);i++)rep[i] = content[i-pos];
            for(int i=pos+strlen(content);i<strlen(content)+strlen(contenttmp);i++)rep[i] = contenttmp[i-strlen(content)];
            if(rep[strlen(rep)-1]!='\n')rep[strlen(rep)] = '\n';
            fputs(rep,tmp);
        }
        else {
            if(contenttmp[strlen(contenttmp)-1]!='\n')contenttmp[strlen(contenttmp)] = '\n';
            fputs(contenttmp,tmp);
        }
    }
    fclose(fp) ,  fclose(tmp) , remove(path) , rename("replace.tmp",path);
}
//bia replace * sho doros kon //oonjahaei ke * daran kareshhon ro ra bendaz //too replace age * ro dorost nakoni tooli ke hazf mikone tanz mishe
void replace(char path[] , char findContent[] , char newContent[] , int at , int all , int flag){
   if(!validPath(path)){
        printf("Invalid command : wrong address\n");
        return;
    }
    if(validPath(path)==-1){
        printf("Invalid command : requested file does not exist\n");
        return;
    }
    if(at&&all){
        printf("Invalid command : Two incompatible attributes\n");
        return;
    }
    undoSaver(path);
    if(!at&&!all){
        find(path,findContent,0,0,0,1,1," ",0,flag);
        if(!lenPlaceHolder){
            printf("-1\n");
            return;
        }
        rmv(path,1,placeHolder[0],sizeHolder[0],'f',1,1);
        insertForReplace(path , newContent , placeHolder[0]);
    }
    if(all){
        find(path,findContent,0,0,0,1,1," ",0,flag);
        if(!lenPlaceHolder){
            printf("-1\n");
            return;
        }
        int x = lenPlaceHolder;
        for(int i=0 ; i<x ; i++){
            rmv(path,1,placeHolder[0],sizeHolder[0],'f',1,1);
            insertForReplace(path , newContent , placeHolder[0]);
            find(path,findContent,0,0,0,1,1," ",0,flag);
        }
    }
    if(at){
        find(path,findContent,0,0,0,1,1," ",0,flag);
        if(at>lenPlaceHolder){
            printf("-1\n");
            return;
        }
        at--;
        rmv(path,1,placeHolder[at], sizeHolder[at] ,'f',1,1);
        insertForReplace(path , newContent , placeHolder[at]);
    }
    printf("successfully replaced\n");
}
void grep(char files[][mxn1] , int filesNum , char content[] , int l , int c , char armn[],int fromArman){
    for(int i=0;i<filesNum;i++){
        if(!validPath(files[i])){
            printf("Invalid command : wrong address\n");
            return;
        }
        if(validPath(files[i])==-1){
            printf("Invalid command : requested file does not exist\n");
            return;
        }
    }
    if(!l&&!c){
        for(int k=0;k<filesNum;k++){
            char contenttmp[mxn1]="\0" ;
            FILE * fp = NULL;
            fp = fopen(files[k],"r");
            while(fgets(contenttmp,mxn1,fp)!=NULL){
                if(strstr(contenttmp,content)!=NULL){
                    if(contenttmp[strlen(contenttmp)-1]!='\n')strcat(contenttmp,"\n");
                    if(fromArman)strcat(armn,files[k]),strcat(armn,": ") , strcat(armn,contenttmp);
                    else printf("%s: %s",files[k],contenttmp);
                }
            }
            fclose(fp);
        }
    }
    if(c){
        int ans=0;
        for(int k=0;k<filesNum;k++){
            char contenttmp[mxn1]="\0";
            FILE * fp = NULL;
            fp = fopen(files[k],"r");
            while(fgets(contenttmp,mxn1,fp)!=NULL)if(strstr(contenttmp,content)!=NULL)ans++;
            fclose(fp);
        }
        if(fromArman)intToStr(ans,armn);
        else printf("%d\n",ans);
    }
    if(l){
        for(int k=0;k<filesNum;k++){
            char contenttmp[mxn1]="\0";
            FILE * fp = NULL;
            fp = fopen(files[k],"r");
            while(fgets(contenttmp,mxn1,fp)!=NULL){
                if(strstr(contenttmp,content)!=NULL){
                    if(fromArman)strcat(armn,files[k]),strcat(armn,"\n");
                    else printf("%s\n",files[k]);
                    break;
                }
            }
            fclose(fp);
        }
    }
}
void undo(char path[]){
    if(!validPath(path)){
        printf("Invalid command : wrong address\n");
        return;
    }
    if(validPath(path)==-1){
        printf("Invalid command : requested file does not exist\n");
        return;
    }
    char undopath[mxn1]="undoFiles/" , content[mxn]="\0";
    for(int i = 5;i<strlen(path);i++)(path[i]=='\\')?strcat(undopath,"-") : strncat(undopath,path+i,1);
    FILE * fp = fopen(path,"r") , *fp1 = fopen("temp.txt","w+");
    while(fgets(content,mxn,fp)!=NULL)fputs(content,fp1);
    fclose(fp) , fclose(fp1);
    FILE * fp2 = fopen(undopath , "r") , * fp3 = fopen(path , "w+");
    while(fgets(content,mxn,fp2)!=NULL)fputs(content,fp3);
    fclose(fp2) , fclose(fp3);
    FILE* fp4 = fopen(undopath , "w+") , *fp5 = fopen("temp.txt","r");
    while(fgets(content,mxn,fp5)!=NULL)fputs(content,fp4);
    fclose(fp4) , fclose(fp5) , remove("temp.txt");
}
void indent(char path[]){
   if(!validPath(path)){
        printf("Invalid command : wrong address\n");
        return;
    }
    if(validPath(path)==-1){
        printf("Invalid command : requested file does not exist\n");
        return;
    }
    undoSaver(path);
    FILE * fp , * tmp;
    fp = fopen(path , "r+") , tmp = fopen("replace.tmp" , "w+") ;
    char content[mxn1] = "\0" , lastchar = ' ';
    int opCnt = 0 , flag = 0 ,i=0;
    while(fgets(content,mxn1,fp)!=NULL){
        i = 0;
        while(content[i]==' ')i++;
        while(i<strlen(content)){
            flag = 1;
            if(content[i]=='}'){
                opCnt--;
                if(lastchar!='\n')fputs("\n",tmp) , lastchar='\n';
            }
            if(lastchar=='\n'){
                int j = 0 , k=i+1 ;
                for(k;content[k]==' ';k++);
                for(j;j<4*opCnt;j++){
                   fputs(" ",tmp) , lastchar=' ';
                }
                if(content[i]=='}'){
                    if(content[i+1]=='\n')fputs("}",tmp) , i++ ,lastchar='}';
                    else fputs("}\n",tmp) , i=k , lastchar='\n';
                    continue;
                }
                if(i<strlen(content)-1 && content[i]=='{'&& content[i+1]!='\n')fputc(content[i],tmp) , fputs("\n",tmp) , flag = 0 , lastchar='\n';
                if(flag && content[i]!=' ')fputc(content[i],tmp),lastchar=content[i];
                if(content[i]=='{')opCnt++;
                i = k;
                continue;
            }
            if(content[i]=='{'&& lastchar!=' ')fputs(" ",tmp);
            if(i<strlen(content)-1 && content[i]=='{'&& content[i+1]!='\n')fputc(content[i],tmp) , fputs("\n",tmp) , flag = 0 , lastchar='\n';
            if(flag)fputc(content[i],tmp) , lastchar=content[i];
            if(content[i]=='{')opCnt++;
            i++;
        }
    }

    printf("Your file has been successfully indented\n");
    fclose(fp) , fclose(tmp) , remove(path) , rename("replace.tmp",path);
}
void compare(char path1[] , char path2[]){
    if(!validPath(path1) || !validPath(path2)){
        printf("Invalid command : wrong address\n");
        return;
    }
    if(validPath(path1)==-1 || validPath(path2)==-1){
        printf("Invalid command : requested file does not exist\n");
        return;
    }
    FILE *fp1 , *fp2;
    fp1=fopen(path1,"r") , fp2=fopen(path2,"r");
    int line = 0;
    char content1[mxn1]="\0" , content2[mxn1]="\0" , *buffer1 , *buffer2;
    do{
        buffer1=fgets(content1,mxn1,fp1) , buffer2=fgets(content2,mxn1,fp2);
        if(content1[strlen(content1)-1]=='\n')content1[strlen(content1)-1]='\0';
        if(content2[strlen(content2)-1]=='\n')content2[strlen(content2)-1]='\0';
        line++;
        if(strcmp(content1,content2)&&buffer1!=NULL&&buffer2!=NULL){
            char contenttmp1[mxn1] = "\0" , contenttmp2[mxn1] = "\0" , words1[mxn1][mxn1] = {'\0'} , words2[mxn1][mxn1] = {'\0'};
            int diff = 0 , n1 = charcnt(content1,' ') , n2 = charcnt(content2,' ');
            split(content1,' ',words1) , split(content2,' ',words2);
            if(abs(n1-n2)>=2)diff=2;
            for(int i=0;i<max(n1,n2)&&abs(n1-n2)<2;i++){
                if(words1[i][strlen(words1[i])-1]=='\n')words1[i][strlen(words1[i])-1]='\0';
                if(words2[i][strlen(words2[i])-1]=='\n')words2[i][strlen(words2[i])-1]='\0';
                if(strcmp(words1[i],words2[i])){
                    const char *tmp1 = words1[i] , *tmp2 = words2[i] , maram1[3] = ">>" , mara2[4] = "<< ";
                    strcat(contenttmp1,maram1),strcat(contenttmp1,tmp1),strcat(contenttmp1,mara2);
                    strcat(contenttmp2,maram1),strcat(contenttmp2,tmp2),strcat(contenttmp2,mara2);
                    diff++;
                }
                else{
                    const char *tmp1 = words1[i] , *tmp2 = words2[i] , maram1[2] = " ";
                    strcat(contenttmp1,tmp1) , strcat(contenttmp1,maram1);
                    strcat(contenttmp2,tmp2) , strcat(contenttmp2,maram1);
                }
            }
            if(diff<2){
                if(contenttmp1[strlen(contenttmp1)-1]!='\n')strcat(contenttmp1,"\n");
                if(contenttmp2[strlen(contenttmp2)-1]!='\n')strcat(contenttmp2,"\n");
                printf("============ #%d ============\n",line) , printf("%s",contenttmp1) , printf("%s",contenttmp2);
                continue;
            }
            if(content1[strlen(content1)-1]!='\n')strcat(content1,"\n");
            if(content2[strlen(content2)-1]!='\n')strcat(content2,"\n");
            printf("============ #%d ============\n",line) , printf("%s",content1) , printf("%s",content2);
        }
    }
    while(buffer1!=NULL && buffer2!=NULL);
    if(buffer1!=NULL){
        int tmp = line;
        char contenttmp[mxn]="\0";
        do strcat(contenttmp,content1) , line++;
        while(fgets(content1,mxn1,fp1)!=NULL);
        printf(">>>>>>>>>>>> #%d - #%d >>>>>>>>>>>>\n%s",tmp,line-1,contenttmp);
    }
    if(buffer2!=NULL){
        int tmp = line;
        char contenttmp[mxn]="\0";
        do strcat(contenttmp,content2) , line++;
        while(fgets(content2,mxn1,fp2)!=NULL);
        printf(">>>>>>>>>>>> #%d - #%d >>>>>>>>>>>>\n%s",tmp,line-1,contenttmp);
    }
    fclose(fp1) , fclose(fp2);
}
void dirtree(char basepath[] , int root ,int depth , char armn[] , int fromArman){
    if(depth<-1){
        printf("Invalid command : Invalid depth\n");
        return;
    }
    char path[mxn] , c1 = (char)(179) , c2 = (char)(195) , c3 = (char)(196);
    if(root==2){
        if(fromArman)strncat(armn,&c2,1), strncat(armn,&c3,1), strcat(armn,"root\n");
        else printf("%c%croot\n",195,196);
    }
    struct dirent *entry;
    DIR *folder = opendir(basepath);
    if(!folder||root==2*depth+2)return;
    while((entry = readdir(folder))!=NULL){
        if(strcmp(entry->d_name, ".")&& strcmp(entry->d_name, "..")){
            for(int i=0; i<root; i++){
                if (!((i%2)&&i)){
                    if(fromArman)strncat(armn,&c1 , 1);
                    else printf("%c", 179);
                }
                else{
                    if(fromArman)strcat(armn," ");
                    else printf(" ");
                }
            }
            if(fromArman)strncat(armn,&c2,1), strncat(armn,&c3,1), strcat(armn,entry->d_name), strcat(armn,"\n");
            else printf("%c%c%s\n", 195, 196, entry->d_name);
            strcpy(path, basepath) , strcat(path, "/") , strcat(path, entry->d_name);
            dirtree(path, root + 2 ,depth,armn,fromArman);
        }
    }
    closedir(folder);
}
void exitProgram(){
    for(int i=0;i<cntUndoFiles;i++)remove(undoFiles[i]);
}
void commandSplitter(char input[] , char command[]){
    for(int i = 0; i<strlen(input) && input[i]!=' ';i++)strncat(command ,input+i,1);
}
int strToInt(char str[]){
    int powerOfTen=1 , ans = 0;
    for(int i=strlen(str)-1;i>=0;i--)ans+=(str[i]-'0')*powerOfTen , powerOfTen*=10;
    return ans;
}
void arman(char input[]){
    int i=0 , k=1,flag=0,commNum;
    char holder[mxn] = "\0";
    strcat(input," ");
    while(strstr(input+i,"=D")!=NULL)k++,i=(strstr(input+i,"=D")-input)+2;
    commNum = k-1 , i=0;
    while(k){
        int count;
        if(strstr(input+i,"=D")!=NULL)count = (strstr(input+i,"=D")-(input+i));
        else count = (strlen(input)-i);
        k--;
        char comm[mxn1]="\0";
        for(int j=0;j<count;j++)strncat(comm ,input+i+j,1);
        if(strstr(comm,"cat")!=NULL){
            if(k!=commNum){
                printf("Invalid input : passing str to a command without str input\n");
                return;
            }
            i = (strstr(input+i,"--file")-input)+7;
            char file[mxn1] = "\0";
            if(input[i]=='"')i++,flag=1;
            if(input[i]=='/')i++;
            for(i;input[i]!=' ' || flag;strncat(file,&input[i++],1)){
                if(flag && input[i]=='"')flag = 0;
            }
            if(file[strlen(file)-1]=='\"')file[strlen(file)-1]='\0';
            cat(file,holder,1);
        }
        else if(strstr(comm,"tree")!=NULL){
            if(k!=commNum){
                printf("Invalid input : passing str to a command without str input\n");
                return;
            }
            i+=5;
            char Depth[5]="\0";
            while(input[i]>='0'&&input[i]<='9')strncat(Depth,input+i,1),i++;
            int depth = strToInt(Depth);
            dirtree("root",2,depth,holder,1);
        }
        else if(strstr(comm,"grep")!=NULL){
            if(k==commNum){
                printf("Invalid input : not passing str to a command with str input\n");
                return;
            }
            char  files [mxn1][mxn1] = {"\0"} ,content[mxn]="\0";
            int   flag=0 , s=0;
            i=(strstr(input+i,"--files")-input)+7;
            while(i<strlen(input)-1 && input[i+1]!='-' && !(input[i+1]=='=' && input[i+2]=='D')){
                i++;
                if(input[i]=='"')i++ , flag=1;
                if(input[i]=='/')i++;
                for(i;input[i]!=' ' || flag;strncat(files[s],&input[i++],1)){
                    if(flag && input[i]=='"')flag = 0;
                }
                if(files[s][strlen(files[s])-1]=='\"')files[s][strlen(files[s])-1]='\0';
                if(files[s][strlen(files[s])-1]==' ')files[s][strlen(files[s])-1]='\0';
                s++;
            }
            for(int j=0 ; j<strlen(holder);j++){
                if((!j || j==strlen(holder)-1) && holder[j]=='"')continue;
                if(j<strlen(holder)-1 && holder[j]=='\\' && holder[j+1]=='\"')continue;
                if(j<strlen(holder)-1 && holder[j]=='\\' && holder[j+1]=='\*')continue;
                else strncat(content,holder+j,1);
            }
            if(strstr(files[s-1],"root")==NULL)s--;
            if(!k){
                if(strstr(comm,"-c")!=NULL && strstr(comm,"-l")!=NULL)grep(files,s,content,1,1," ",0);
                else if(strstr(comm,"-c")!=NULL)grep(files,s,content,0,1," ",0);
                else if(strstr(comm,"-l")!=NULL)grep(files,s,content,1,0," ",0);
                else grep(files,s,content,0,0," ",0);
            }
            else{
                strcpy(holder,"\0");
                if(strstr(comm,"-c")!=NULL && strstr(comm,"-l")!=NULL)grep(files,s,content,1,1,holder,1);
                else if(strstr(comm,"-c")!=NULL)grep(files,s,content,0,1,holder,1);
                else if(strstr(comm,"-l")!=NULL)grep(files,s,content,1,0,holder,1);
                else grep(files,s,content,0,0,holder,1);
            }

        }
        else if(strstr(comm,"insertstr")!=NULL){
            if(k){
                printf("Invalid input : insert can not be used this way\n");
                return;
            }
            int line , pos , flag = 0 , fpholder = i;
            i=(strstr(input+i,"--file")-input)+7;
            char file[mxn1]="\0" , content[mxn]="\0";
            if(input[i]=='"')i++ , flag=1;
            if(input[i]=='/')i++;
            for(i;input[i]!=' ' || flag;strncat(file,&input[i++],1)){
                if(flag && input[i]=='"')flag = 0;
            }
            if(file[strlen(file)-1]=='\"')file[strlen(file)-1]='\0';
            i = (strstr(input+i,"--pos")-input)+2;
            sscanf(comm+i-fpholder,"pos %d:%d",&line,&pos);
            for(int j=0 ; j<strlen(holder);j++){
                if(j<strlen(holder)-1 && holder[j]=='\\' && holder[j+1]=='\"')continue;
                if(j<strlen(holder)-1 && holder[j]=='\\' && holder[j+1]=='\*')continue;
                else strncat(content,holder+j,1);
            }
            insert(file,content,line,pos);
        }

        else if(strstr(comm,"find")!=NULL){
            if(k==commNum){
                printf("Invalid input : not passing str to a command with str input\n");
                return;
            }
            int count = 0, at = 0, byword = 0, all=0 , flag=0,star=0;
            i=(strstr(input+i,"--file")-input)+7;
            char content[mxn]="\0" , file[mxn1]="\0";
            if(strstr(comm,"count")!=NULL)count=1;
            if(strstr(comm,"byword")!=NULL)byword=1;
            if(strstr(comm,"at")!=NULL){
                int j=0;
                j = (strstr(input+i,"at")-input)+3;
                char At[mxn1] = "\0";
                while(input[j]>='0' && input[j]<='9')strncat(At,input+j,1) , j++;
                at = strToInt(At);
            }
            if(strstr(comm,"all")!=NULL)all=1;
            if(input[i]=='"')i++ , flag=1;
            if(input[i]=='/')i++;
            for(i;input[i]!=' ' || flag;strncat(file,&input[i++],1)){
                if(flag && input[i]=='"')flag = 0;
            }
            if(file[strlen(file)-1]=='\"')file[strlen(file)-1]='\0';
            if(holder[0]=='*')star=1;
            if(strlen(holder)>1 && holder[strlen(holder)-1]=='*' && holder[strlen(holder)-2]!='\\')star=2;
            for(int j=0 ; j<strlen(holder);j++){
                if(j<strlen(holder)-1 && holder[j]=='\\' && holder[j+1]=='\"')continue;
                if(j<strlen(holder)-1 && holder[j]=='\\' && holder[j+1]=='\*')continue;
                else strncat(content,holder+j,1);
            }
            if(!k)find(file,content,count,at,byword,all,0," ",0,star);
            else{
                strcpy(holder,"\0");
                find(file,content,count,at,byword,all,0,holder,1,star);
            }
        }
          else if(strstr(comm,"replace")!=NULL){
            if(k){
                printf("Invalid input : replace can not be used this way\n");
                return;
            }
            int at = 0, all=0,flag=0,star=0;
            i=(strstr(input+i,"--file")-input)+7;
            char content[mxn]="\0" , file[mxn1]="\0" , str[mxn1]="\0" , content2[mxn1]="\0";
            if(strstr(comm,"at")!=NULL){
                int j=0;
                char * pain = strstr(input,"at")+3;
                char At[mxn1] = "\0";
                while(*(pain+j)>='0' && *(pain+j)<='9')At[j]=*(pain+j) , j++;
                at = strToInt(At);
            }
            if(strstr(comm,"all")!=NULL)all=1;
            if(input[i]=='"')i++ , flag=1;
            if(input[i]=='/')i++;
            for(i;input[i]!=' ' || flag;strncat(file,&input[i++],1)){
                if(flag && input[i]=='"')flag = 0;
            }
            i = (strstr(input+i,"--str2")-input)+7;
            if(input[i]=='"')i++ , flag=1;
            for(i;input[i]!=' ' || flag;strncat(str,&input[i++],1)){
                if(flag && input[i]=='"')flag = 0;
            }
            if(file[strlen(file)-1]=='\"')file[strlen(file)-1]='\0';
            if(str[strlen(str)-1]=='\"')str[strlen(str)-1]='\0';
            if(holder[0]=='*')star=1;
            if(strlen(holder)>1 && holder[strlen(holder)-1]=='*' && holder[strlen(holder)-2]!='\\')star=2;
            for(int j=0 ; j<strlen(holder);j++){
                if(j<strlen(holder)-1 && holder[j]=='\\' && holder[j+1]=='\"')continue;
                if(j<strlen(holder)-1 && holder[j]=='\\' && holder[j+1]=='\*')continue;
                else strncat(content,holder+j,1);
            }
            for(int j=0 ; j<strlen(str);j++){
                if(j<strlen(str)-1 && str[j]=='\\' && str[j+1]=='\"')continue;
                if(j<strlen(str)-1 && str[j]=='\\' && str[j+1]=='\*')continue;
                if(j && str[j]=='\\' && str[j-1]=='\\')continue;
                if(j<strlen(str)-1 && str[j]=='\\' && str[j+1]=='n'){
                    strcat(content2,"\n");
                    continue;
                }
                if(j && str[j]=='n' && str[j-1]=='\\' && content2[strlen(content2)-1]!='\\')continue;
                else strncat(content2,str+j,1);
            }
            printf("%s - %s",content,content2);
            replace(file,content,content2,at,all,star);
        }
        else{
            if(k<commNum)printf("Invalid command : passing str to a command without str input\n");
            else printf("Invalid command : %scan not be used this way\n" , comm);
            return;
        }
        i = (strstr(input+i,"=D")-input)+3;
    }
}
int main(){
    startProgram();
    while(1){
        printf("\n>>> ");
        char input[mxn]="\0" , command[mxn1]="\0";
        fgets(input,mxn,stdin);
        for(int i=strlen(input)-1; i>=0 && (input[i]==' '||input[i]=='\n');i--)input[i]='\0';
        commandSplitter(input,command);
        if(strstr(input,"=D")!=NULL)arman(input);
        else if(!strcmp(command,"exit")){
            exitProgram();
            return 0;
        }
        else if(!strcmp(command,"createfile")){
            int i= (strstr(input,"--file")-input)+7;
            if(input[i]=='\"')i++ , input[strlen(input)-1]='\0';
            if(input[i]=='/')i++;
            create(input+i);
        }
        else if(!strcmp(command,"insertstr")){
            int  i=(strstr(input,"--file")-input)+7 , line , pos , flag = 0;
            char file[mxn1]="\0" , str[mxn]="\0" , content[mxn]="\0";
            if(input[i]=='"')i++ , flag=1;
            if(input[i]=='/')i++;
            for(i;input[i]!=' ' || flag;strncat(file,&input[i++],1)){
                if(flag && input[i]=='"')flag = 0;
            }
            i=(strstr(input,"--str")-input)+6;
            if(input[i]=='"')i++,flag=1;
            for(i;input[i]!=' '||flag;strncat(str,&input[i++],1) ){
                if(flag && i && input[i]=='\"' && input[i-1]!='\\')flag=0;
            }
            if(file[strlen(file)-1]=='\"')file[strlen(file)-1]='\0';
            if(str[strlen(str)-1]=='\"')str[strlen(str)-1]='\0';
            i = (strstr(input,"--pos")-input)+2;
            sscanf(input+i,"pos %d:%d",&line,&pos);
            for(int j=0 ; j<strlen(str);j++){
                if(j<strlen(str)-1 && str[j]=='\\' && str[j+1]=='\"')continue;
                if(j<strlen(str)-1 && str[j]=='\\' && str[j+1]=='\*')continue;
                if(j && str[j]=='\\' && str[j-1]=='\\')continue;
                if(j<strlen(str)-1 && str[j]=='\\' && str[j+1]=='n'){
                    strcat(content,"\n");
                    continue;
                }
                if(j && str[j]=='n' && str[j-1]=='\\' && content[strlen(content)-1]!='\\')continue;
                else strncat(content,str+j,1);
            }
            insert(file,content,line,pos);
        }
        else if(!strcmp(command,"cat")){
            int i=(strstr(input,"--file")-input)+7;
            if(input[i]=='\"')i++ , input[strlen(input)-1]='\0';
            if(input[i]=='/')i++;
            cat(input+i," ",0);
        }
        else if(!strcmp(command,"removestr")){
            int i=(strstr(input,"--file")-input)+7 , line , pos , size , flag=0;
            char file[mxn1]="\0" , c;
            if(input[i]=='"')i++ , flag=1;
            if(input[i]=='/')i++;
            for(i;input[i]!=' ' || flag;strncat(file,&input[i++],1)){
                if(flag && input[i]=='"')flag = 0;
            }
            i = (strstr(input,"--pos")-input)+2;
            if(file[strlen(file)-1]=='\"')file[strlen(file)-1]='\0';
            sscanf(input+i,"pos %d:%d -size %d -%c",&line,&pos,&size,&c);
            rmv(file,line,pos,size,c,0,0);
        }
        else if(!strcmp(command,"copystr")){
            int i=(strstr(input,"--file")-input)+7 , line , pos , size , flag=0;
            char file[mxn1]="\0" , c;
            if(input[i]=='"')i++ , flag=1;
            if(input[i]=='/')i++;
            for(i;input[i]!=' ' || flag;strncat(file,&input[i++],1)){
                if(flag && input[i]=='"')flag = 0;
            }
            i = (strstr(input,"--pos")-input)+2;
            if(file[strlen(file)-1]=='\"')file[strlen(file)-1]='\0';
            sscanf(input+i,"pos %d:%d -size %d -%c",&line,&pos,&size,&c);
            copy(file,line,pos,size,c,0);
        }
        else if(!strcmp(command,"cutstr")){
            int i=(strstr(input,"--file")-input)+7 , line , pos , size , flag=0;
            char file[mxn1]="\0" , c;
            if(input[i]=='"')i++ , flag=1;
            if(input[i]=='/')i++;
            for(i;input[i]!=' ' || flag;strncat(file,&input[i++],1)){
                if(flag && input[i]=='"')flag = 0;
            }
            i = (strstr(input,"--pos")-input)+2;
            if(file[strlen(file)-1]=='\"')file[strlen(file)-1]='\0';
            sscanf(input+i,"pos %d:%d -size %d -%c",&line,&pos,&size,&c);
            cut(file,line,pos,size,c);
        }
        else if(!strcmp(command,"paste")){
            int  i=(strstr(input,"--file")-input)+7 , line , pos , flag=0;
            char file[mxn1]="\0";
            if(input[i]=='"')i++ , flag = 1;
            if(input[i]=='/')i++;
            for(i;input[i]!=' ' || flag;strncat(file,&input[i++],1)){
                if(flag && input[i]=='"')flag = 0;
            }
            if(file[strlen(file)-1]=='\"')file[strlen(file)-1]='\0';
            i = (strstr(input,"--pos")-input)+2;
            sscanf(input+i,"pos %d:%d",&line,&pos);
            paste(file,line,pos);
        }
        else if(!strcmp(command,"undo")){
            int i=(strstr(input,"--file")-input)+7;
            if(input[i]=='\"')i++ , input[strlen(input)-1]='\0';
            if(input[i]=='/')i++;
            undo(input+i);
        }
        else if(!strcmp(command,"auto-indent")){
            int i=(strstr(input,"--file")-input)+7;
            if(input[i]=='\"')i++ , input[strlen(input)-1]='\0';
            if(input[i]=='/')i++;
            indent(input+i);
        }
        else if(!strcmp(command,"compare")){
            int i = 0 , spcCnt = 0 , flag =0;
            char file1[20]="\0";
            while(spcCnt<1)if(input[i++]==' ')spcCnt++;
            if(input[i]=='"')i++ , flag =1;
            if(input[i]=='/')i++;
            for(i;input[i]!=' ' || flag;strncat(file1,&input[i++],1)){
                if(flag && input[i]=='"')flag = 0;
            }
            if(file1[strlen(file1)-1]=='\"')file1[strlen(file1)-1]='\0';
            i++,spcCnt++;
            if(input[i]=='\"')i++ , input[strlen(input)-1]='\0';
            if(input[i]=='/')i++;
            compare(file1,input+i);
        }
        else if(!strcmp(command,"tree")){
            int depth;
            sscanf(input+5,"%d",&depth) , dirtree("root",2,depth," ",0);
        }
        else if(!strcmp(command,"grep")){
            char  files [mxn1][mxn1] = {"\0"} , str[mxn]="\0" , content[mxn]="\0";
            int  i=(strstr(input,"--str")-input)+6  , flag=0 , k=0;
            strcat(input," ");
            if(input[i]=='"')i++,flag=1;
            for(i;input[i]!=' '||flag;strncat(str,&input[i++],1) ){
                if(flag && i && input[i]=='\"' && input[i-1]!='\\')flag=0;
            }
            if(str[strlen(str)-1]=='\"')str[strlen(str)-1]='\0';
            for(int j=0 ; j<strlen(str);j++){
                if(j<strlen(str)-1 && str[j]=='\\' && str[j+1]=='\"')continue;
                if(j<strlen(str)-1 && str[j]=='\\' && str[j+1]=='\*')continue;
                else strncat(content,str+j,1);
            }
            i = (strstr(input,"--files")-input)+7;
            while(i<strlen(input)-1 && input[i+2]!='-' &&input[i+1]!='-'){
                i++;
                if(input[i]=='"')i++ , flag=1;
                if(input[i]=='/')i++;
                for(i;input[i]!=' ' || flag;strncat(files[k],&input[i++],1)){
                    if(flag && input[i]=='"')flag = 0;
                }
                if(files[k][strlen(files[k])-1]=='\"')files[k][strlen(files[k])-1]='\0';
                k++;
            }
            if(strstr(input,"-c")!=NULL && strstr(input,"-l")!=NULL)grep(files,k,content,1,1," ",0);
            else if(strstr(input,"-c")!=NULL)grep(files,k,content,0,1," ",0);
            else if(strstr(input,"-l")!=NULL)grep(files,k,content,1,0," ",0);
            else grep(files,k,content,0,0," ",0);
        }
        else if(!strcmp(command,"find")){
            int count = 0, at = 0, byword = 0, all=0 , i=(strstr(input,"str")-input)+4 , flag=0 , star = 0;
            strcat(input," ");
            char str[mxn]="\0" , content[mxn]="\0" , file[mxn1]="\0";
            if(strstr(input,"count")!=NULL)count=1;
            if(strstr(input,"byword")!=NULL)byword=1;
            if(strstr(input,"at")!=NULL){
                int j=0;
                char * pain = strstr(input,"at")+3;
                char At[mxn1] = "\0";
                while(*(pain+j)>='0' && *(pain+j)<='9')At[j]=*(pain+j) , j++;
                at = strToInt(At);
            }
            if(strstr(input,"all")!=NULL)all=1;
            if(input[i]=='"')i++ , flag=1;
            for(i;input[i]!=' '||flag;strncat(str,&input[i++],1) ){
                if(flag && i && input[i]=='\"' && input[i-1]!='\\')flag=0;
            }
            if(str[strlen(str)-1]=='"')str[strlen(str)-1]='\0';
            if(str[0]=='*')star=1;
            if(strlen(str)>1 && str[strlen(str)-1]=='*' && str[strlen(str)-2]!='\\')star=2;
            for(int j=0 ; j<strlen(str);j++){
                if(j<strlen(str)-1 && str[j]=='\\' && str[j+1]=='\"')continue;
                if(j<strlen(str)-1 && str[j]=='\\' && str[j+1]=='\*')continue;
                if(j && str[j]=='\\' && str[j-1]=='\\')continue;
                if(j<strlen(str)-1 && str[j]=='\\' && str[j+1]=='n'){
                    strcat(content,"\n");
                    continue;
                }
                if(j && str[j]=='n' && str[j-1]=='\\' && content[strlen(content)-1]!='\\')continue;
                else strncat(content,str+j,1);
            }
            i=(strstr(input,"file")-input)+5;
            if(input[i]=='"')i++ , flag=1;
            if(input[i]=='/')i++;
            for(i;input[i]!=' ' || flag;strncat(file,&input[i++],1)){
                if(flag && input[i]=='"')flag = 0;
            }
            if(file[strlen(file)-1]=='\"')file[strlen(file)-1]='\0';
            find(file,content,count,at,byword,all,0," ",0,star);
        }
        else if(!strcmp(command,"replace")){
            int at = 0  , all=0 , i=(strstr(input,"str1")-input)+5 , flag=0 , star = 0;
            strcat(input," ");
            char str[mxn]="\0" , content[mxn]="\0" , content2[mxn] = "\0" , file[mxn1]="\0" , str2[mxn]="\0";
            if(strstr(input,"at")!=NULL){
                int j=0;
                char * pain = strstr(input,"at")+3;
                char At[mxn1] = "\0";
                while(*(pain+j)>='0' && *(pain+j)<='9')At[j]=*(pain+j) , j++;
                at = strToInt(At);
            }
            if(strstr(input,"all")!=NULL)all=1;
            if(input[i]=='"')i++ , flag=1;
            for(i;input[i]!=' '||flag;strncat(str,&input[i++],1) ){
                if(flag && i && input[i]=='\"' && input[i-1]!='\\')flag=0;
            }
            i=(strstr(input,"str2")-input)+5;
            if(input[i]=='"')i++ , flag=1;
            for(i;input[i]!=' '||flag;strncat(str2,&input[i++],1) ){
                if(flag && i && input[i]=='\"' && input[i-1]!='\\')flag=0;
            }
            if(str[strlen(str)-1]=='"')str[strlen(str)-1]='\0';
            if(str2[strlen(str2)-1]=='"')str2[strlen(str2)-1]='\0';
            if(str[0]=='*')star=1;
            if(strlen(str)>1 && str[strlen(str)-1]=='*' && str[strlen(str)-2]!='\\')star=2;
            for(int j=0 ; j<strlen(str);j++){
                if(j<strlen(str)-1 && str[j]=='\\' && str[j+1]=='\"')continue;
                if(j<strlen(str)-1 && str[j]=='\\' && str[j+1]=='\*')continue;
                if(j && str[j]=='\\' && str[j-1]=='\\')continue;
                if(j<strlen(str)-1 && str[j]=='\\' && str[j+1]=='n'){
                    strcat(content,"\n");
                    continue;
                }
                if(j && str[j]=='n' && str[j-1]=='\\' && content[strlen(content)-1]!='\\')continue;
                else strncat(content,str+j,1);
            }
            for(int j=0 ; j<strlen(str2);j++){
                if(j<strlen(str2)-1 && str2[j]=='\\' && str2[j+1]=='\"')continue;
                if(j<strlen(str2)-1 && str2[j]=='\\' && str2[j+1]=='\*')continue;
                if(j && str2[j]=='\\' && str2[j-1]=='\\')continue;
                if(j<strlen(str2)-1 && str2[j]=='\\' && str2[j+1]=='n'){
                    strcat(content2,"\n");
                    continue;
                }
                if(j && str2[j]=='n' && str2[j-1]=='\\' && content2[strlen(content2)-1]!='\\')continue;
                else strncat(content2,str2+j,1);
            }
            i=(strstr(input,"file")-input)+5;
            if(input[i]=='"')i++ , flag=1;
            if(input[i]=='/')i++;
            for(i;input[i]!=' ' || flag;strncat(file,&input[i++],1)){
                if(flag && input[i]=='"')flag = 0;
            }
            if(file[strlen(file)-1]=='\"')file[strlen(file)-1]='\0';
            replace(file,content,content2,at,all,star);
        }

    }
}






























/*void find(char path[] , char content[] , int count , int at , int byword , int all , int fromReplace , char armn[] , int fromArman,int flag){
    if(!fromReplace && !validPath(path)){
        printf("Invalid command : wrong address\n");
        return;
    }
    if(!fromReplace && validPath(path)==-1){
        printf("Invalid command : requested file does not exist\n");
        return;
    }
    if(at&&all){
        printf("Invalid command : Two incompatible attributes\n");
        return;
    }
    FILE * fp;
    fp = fopen(path , "r") , lenPlaceHolder=0;
    char contentHolder[mxn]="\0"  ,  contenttmp[mxn1] = "\0" , sub[mxn1]="\0";
    int bywordHolder[mxn1] , len = strlen(content) , cnt=1 , numofChars=0 , lastSpace;
    while(fgets(contenttmp,mxn1,fp)!=NULL)strcat(contentHolder,contenttmp);
    if(flag==1){
        for(int i=1;i<len;i++)sub[i-1]=content[i];
        len--;
    }
    else if(flag==2){
        for(int i=0;i<len-1;i++)sub[i]=content[i];
        len--;
    }

    if(!flag){
        for(int i=0;i<strlen(contentHolder)-len+1;i++){
            if(contentHolder[i]==' ')cnt++;
            char substitute[mxn1] = "\0";
            if(contentHolder[i+len]!=' ' && contentHolder[i+len]!='\n' && contentHolder[i+len]!=EOF && contentHolder[i+len]!='\0')continue;
            if(i&&contentHolder[i-1]!=' '&&contentHolder[i-1]!='\n')continue;
            for(int j=i;j<i+len;j++)substitute[j-i]=contentHolder[j];
            if(!strcmp(substitute,content))placeHolder[lenPlaceHolder]=i , bywordHolder[lenPlaceHolder]=cnt , sizeHolder[lenPlaceHolder] = len , lenPlaceHolder++;
        }
    }
    else if(flag==1){
        for(int i=0;i<strlen(contentHolder)-len+1;i++){
            if(contentHolder[i+len]!=' '&&contentHolder[i+len]!='\n'&&contentHolder[i+len]!=EOF)continue;
            if(contentHolder[i]==' ')cnt++;
            char substitute[mxn1] = "\0";
            for(int j=i;j<i+len;j++)substitute[j-i]=contentHolder[j];
            if(!strcmp(substitute,sub)){
                int j=i;
                for(j;j>=0;j--)if(contentHolder[j]==' '|| contentHolder[j]=='\n')break;
                placeHolder[lenPlaceHolder]=j+1 , bywordHolder[lenPlaceHolder]=cnt , sizeHolder[lenPlaceHolder] = i-j+len-1  , lenPlaceHolder++;
            }
        }
    }
    else if(flag==2){
        for(int i=0;i<strlen(contentHolder)-len+1;i++){
            if(i&&contentHolder[i-1]!=' '&&contentHolder[i-1]!='\n')continue;
            if(contentHolder[i]==' ')cnt++;
            char substitute[mxn1] = "\0";
            for(int j=i;j<i+len;j++)substitute[j-i]=contentHolder[j];
            if(!strcmp(substitute,sub)){
                if(fromReplace){
                    int j=i;
                    for(j;j<strlen(contentHolder);j++)if(contentHolder[j]==' ' || contentHolder[j]==EOF || contentHolder[j]=='\n')break;
                    sizeHolder[lenPlaceHolder]=j-i;
                }
                placeHolder[lenPlaceHolder]=i , bywordHolder[lenPlaceHolder]=cnt , lenPlaceHolder++;
            }
        }
    }
    if(!lenPlaceHolder&&!fromReplace){
        if(fromArman)strcat(armn,"-1");
        else printf("-1\n");
        fclose(fp);
        return;
    }
    if(count){
        if(fromArman) intToStr(lenPlaceHolder,armn);
        else printf("%d\n",lenPlaceHolder);
    }
    if(at){
        if(at>lenPlaceHolder){
            if(fromArman)strcat(armn,"-1");
            else printf("-1\n");
            fclose(fp);
            return;
        }
        at--;
        if(byword){
            if(fromArman)intToStr(bywordHolder[at],armn);
            else printf("%d\n",bywordHolder[at]);
        }
        else{
            if(fromArman)intToStr(placeHolder[at],armn);
            else printf("%d\n",placeHolder[at]);
        }
    }
    if(all){
        if(byword){
            if(fromArman){
                for(int i=0;i<lenPlaceHolder-1;i++)intToStr(bywordHolder[i],armn) , strcat(armn,", ");
                intToStr(bywordHolder[lenPlaceHolder-1],armn);
            }
            else{
                for(int i=0;i<lenPlaceHolder-1;i++)printf("%d,",bywordHolder[i]);
                printf("%d\n" , bywordHolder[lenPlaceHolder-1]);
            }

        }
        else{
            if(!fromReplace){
                if(fromArman){
                    for(int i=0;i<lenPlaceHolder-1;i++)intToStr(placeHolder[i],armn) , strcat(armn,", ");
                    intToStr(placeHolder[lenPlaceHolder-1],armn);
                }
                else{
                    for(int i=0;i<lenPlaceHolder-1;i++)printf("%d,",placeHolder[i]);
                    printf("%d\n" , placeHolder[lenPlaceHolder-1]);
                }
            }
        }
    }
    if(!at&&!all && !count){
        if(byword){
            if(fromArman)intToStr(bywordHolder[0],armn);
            else printf("%d\n",bywordHolder[0]);
        }
        else{
            if(fromArman)intToStr(placeHolder[0],armn);
            else printf("%d\n",placeHolder[0]);
        }
    }
    fclose(fp);
}*/
