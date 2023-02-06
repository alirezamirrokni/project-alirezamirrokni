#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#define mxn1 100
#define mxn 1000
int x = 4, y = 0 , width = 50 , height=9 , line=1 , lineHolder = 1 ,  saved = 0 , command=0 , numOfChars[500] , firstline=0 , startline = 4 , lastline =0 , Find=0 , lineFirstHappen , posFirstHappen;
int xcomm , ycomm , findFlag;
int placeHolder[mxn1] , lenPlaceHolder , sizeHolder[mxn1];
char clipBoard[mxn] , undoFiles[mxn1][mxn1] , cntUndoFiles;
char name[100] = "Untitled.txt" , state[10] = "NORMAL" , commandStr[100];
int min(int a,int b){
    return (a>b)*b + (b>=a)*a;
}
void notValidPath(){
    move(height-1,0) , clrtoeol();
    printw("Invalid Command : this file doesnt exist");
    getch();
    move(height-1,0) , clrtoeol() , y=ycomm , x=xcomm , move(y,x);
}
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
    FILE * fp = fopen(path,"r");
    return fp;
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
void undo(char path[]){
    if(!validPath(path)){
        notValidPath();
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
void undo2(){
    char undopath[100]="undoFiles/" , content[100]="\0";
    if(strstr(name,"root")==NULL)strcat(undopath,"Home-");
    for(int i = (strstr(name,"root")==NULL) ? 0:5 ;i<strlen(name);i++)name[i]=='\\' ? strcat(undopath,"-") : strncat(undopath,name+i,1);
    FILE * fp = fopen("Untitled.txt","r") , *fp1 = fopen("temp.txt","w+");
    while(fgets(content,100,fp)!=NULL)fputs(content,fp1);
    fclose(fp) , fclose(fp1);
    FILE * fp2 = fopen(undopath , "r") , * fp3 = fopen("Untitled.txt" , "w+");
    while(fgets(content,100,fp2)!=NULL)fputs(content,fp3);
    fclose(fp2) , fclose(fp3) ;
    FILE* fp4 = fopen(undopath , "w+") , *fp5 = fopen("temp.txt","r");
    while(fgets(content,100,fp5)!=NULL)fputs(content,fp4);
    fclose(fp4) , fclose(fp5) , remove("temp.txt");
}
void undoSaver2(){
    char undopath[100]="undoFiles/" , content[100]="\0";
    if(strstr(name,"root")==NULL)strcat(undopath,"Home-") ;
    for(int i = (strstr(name,"root")==NULL)? 0:5 ; i<strlen(name);i++)name[i]=='\\' ? strcat(undopath,"-") : strncat(undopath,name+i,1);
    FILE * fp1 = fopen(undopath , "w+") , * fp2 = fopen("Untitled.txt" , "r");
    while(fgets(content,100,fp2)!=NULL)fputs(content,fp1);
    fclose(fp1);
    fclose(fp2);
}
void create(char path[]){
    if(!validPath(path)){
        fopen(path , "w") , notValidPath();
        return;
    }
    else {
        move(height-1,0) , clrtoeol();
        printw("Invalid Command : this file already exists");
        getch();
        move(height-1,0) , clrtoeol();
    }
}
void insert(char path[] , char content[] , int line , int pos){
    if(!validPath(path)){
        notValidPath();
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
}
void cat(char path[]){
    if(!validPath(path)){
        notValidPath() ;
        return;
    }
    char untitled[] = "Untitled.txt";
    FILE * unt = fopen(untitled , "w");
    FILE * fp;
    fp = fopen(path , "r");
    char content[mxn1] = "\0";
    while(fgets(content , mxn , fp)!=NULL){
        fputs(content , unt);
    }
    fclose(fp) , fclose(unt) , strcmp(name , untitled) , openFile();
}
void rmv(char path[] , int line , int pos , int size , char attribute , int fromCut , int fromRep){
    if(!validPath(path)){
        notValidPath();
        return;
    }
    if(!fromRep)undoSaver(path);
    FILE * fp , * tmp , *tmp1 ;
    fp = fopen(path , "r+") , tmp = fopen("replace.tmp" , "w+"), tmp1 = fopen("remove.txt" , "w+");
    char contenttmp[mxn1]="\0";
    int cnt=0 , charNum = 0;
    while(fgets(contenttmp , mxn1 , fp)!=NULL){
        cnt++ , charNum+=strlen(contenttmp);
        fputs(contenttmp,tmp1);
        if(cnt == line-1)pos += charNum;
    }
    fclose(tmp1) , tmp1 = fopen("remove.txt" , "r") , charNum=0;
    if(attribute == 'f'){
        while(fgets(contenttmp , mxn1 , tmp1)!=NULL){
            for(int i=0;i<strlen(contenttmp);i++){
                if(charNum<pos || charNum>=pos+size)fputc(contenttmp[i],tmp);
                charNum++;
            }
        }
    }
    else{
        while(fgets(contenttmp , mxn1 , tmp1)!=NULL){
            for(int i=0;i<strlen(contenttmp);i++){
                if(charNum<pos-size || charNum>=pos)fputc(contenttmp[i],tmp);
                charNum++;
            }
        }
    }
}
void copy(char path[] , int line , int pos , int size , char attribute , int fromCut){
    if(!fromCut && !validPath(path)){
        notValidPath();
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
}
void cut(char path[] , int line , int pos , int size , char attribute){
    if(!validPath(path)){
        notValidPath();
        return;
    }
    copy(path,line,pos,size,attribute,1);
    rmv(path,line,pos,size,attribute,1,0);
}
void paste(char path[] , int line , int pos){
    insert(path , clipBoard , line , pos);
}
void find(char path[] , char content[] , int count , int at , int byword , int all , int fromReplace ,int flag){
    if(!validPath(path)){
        notValidPath();
        return;
    }
    if(at&&all){
        move(height-1,0) , clrtoeol();
        printw("Invalid command : Two incompatible attributes");
        getch();
        move(height-1,0) , clrtoeol();
        return;
    }
    strcpy(name,"Untitled.txt");
    char untitled[] = "Untitled.txt";
    FILE * unt;
    if(!fromReplace)unt = fopen(untitled , "w");
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
        fputs("-1",unt);
        fclose(fp) , fclose(unt);
        return;
    }
    if(count && !fromReplace){
        char ans[100] = "\0";
        intToStr(lenPlaceHolder,ans);
        fputs(ans,unt);
    }
    if(at){
        if(at>lenPlaceHolder && !fromReplace){
            fputs("-1",unt);
            fclose(fp) , fclose(unt);
            return;
        }
        at--;
        if(byword && !fromReplace){
            char ans[100] = "\0";
            intToStr(bywordHolder[at],ans);
            fputs(ans,unt);
        }
        else if(!fromReplace){
            char ans[100] = "\0";
            intToStr(placeHolder[at],ans);
            fputs(ans,unt);
        }
    }
    if(all){
        if(byword && !fromReplace){
            for(int i=0;i<lenPlaceHolder-1;i++){
                char ans[100] = "\0";
                intToStr(bywordHolder[i],ans);
                fputs(ans,unt) , fputs(", " , unt);
            }
            char ans[100] = "\0";
            intToStr(bywordHolder[lenPlaceHolder-1],ans);
            fputs(ans,unt);
        }
        else if(!fromReplace){
            for(int i=0;i<lenPlaceHolder-1;i++){
                char ans[100] = "\0";
                intToStr(placeHolder[i],ans);
                fputs(ans,unt) , fputs(", " , unt);
            }
            char ans[100] = "\0";
            intToStr(placeHolder[lenPlaceHolder-1],ans);
            fputs(ans,unt);
        }
    }
    if(!at&&!all && !count){
        if(byword && !fromReplace){
            char ans[100] = "\0";
            intToStr(bywordHolder[0],ans);
            fputs(ans,unt);
        }
        else if(!fromReplace){
            char ans[100] = "\0";
            intToStr(placeHolder[0],ans);
            fputs(ans,unt);
        }
    }
    if(!fromReplace)fclose(fp) , fclose(unt) , strcmp(name , untitled) , openFile();
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
        notValidPath();
        return;
    }
    if(at&&all){
        move(height-1,0) , clrtoeol();
        printw("Invalid command : Two incompatible attributes");
        getch();
        move(height-1,0) , clrtoeol();
        return;
    }
    undoSaver(path);
    if(!at&&!all){
        find(path,findContent,0,0,0,1,1,flag);
        if(!lenPlaceHolder){
            printf("-1\n");
            return;
        }
        rmv(path,1,placeHolder[0],sizeHolder[0],'f',1,1);
        insertForReplace(path , newContent , placeHolder[0]);
    }
    if(all){
        find(path,findContent,0,0,0,1,1,flag);
        if(!lenPlaceHolder){
            printf("-1\n");
            return;
        }
        int x = lenPlaceHolder;
        for(int i=0 ; i<x ; i++){
            rmv(path,1,placeHolder[0],sizeHolder[0],'f',1,1);
            insertForReplace(path , newContent , placeHolder[0]);
            find(path,findContent,0,0,0,1,1,flag);
        }
    }
    if(at){
        find(path,findContent,0,0,0,1,1,flag);
        if(at>lenPlaceHolder){
            printf("-1\n");
            return;
        }
        at--;
        rmv(path,1,placeHolder[at], sizeHolder[at] ,'f',1,1);
        insertForReplace(path , newContent , placeHolder[at]);
    }
}
void grep(char files[][mxn1] , int filesNum , char content[] , int l , int c ){
    for(int i=0;i<filesNum;i++){
        if(!validPath(files[i])){
            notValidPath();
            return;
        }
    }
    strcpy(name,"Untitled.txt");
    char untitled[] = "Untitled.txt";
    FILE * unt = fopen(untitled , "w");
    if(!l&&!c){
        for(int k=0;k<filesNum;k++){
            char contenttmp[mxn1]="\0" ;
            FILE * fp = NULL;
            fp = fopen(files[k],"r");
            while(fgets(contenttmp,mxn1,fp)!=NULL){
                if(strstr(contenttmp,content)!=NULL){
                    if(contenttmp[strlen(contenttmp)-1]!='\n')strcat(contenttmp,"\n");
                    fputs(files[k] , unt) , fputs(": " , unt) , fputs(contenttmp , unt);
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
        char Ans[100] = "\0";
        intToStr(ans,Ans);
        fputs(Ans,unt);
    }
    if(l){
        for(int k=0;k<filesNum;k++){
            char contenttmp[mxn1]="\0";
            FILE * fp = NULL;
            fp = fopen(files[k],"r");
            while(fgets(contenttmp,mxn1,fp)!=NULL){
                if(strstr(contenttmp,content)!=NULL){
                    fputs(files[k] , unt) , fputs("\n",unt);
                    break;
                }
            }
            fclose(fp);
        }
    }
    fclose(unt) , strcmp(name , untitled) , openFile();
}
void compare(char path1[] , char path2[]){
    if(!validPath(path1) || !validPath(path2)){
        notValidPath();
        return;
    }
    strcpy(name , "Untitled.txt");
    char untitled[] = "Untitled.txt";
    FILE * unt = fopen(untitled , "w");
    FILE *fp1 , *fp2;
    fp1=fopen(path1,"r") , fp2=fopen(path2,"r");
    int Line = 0;
    char content1[mxn1]="\0" , content2[mxn1]="\0" , *buffer1 , *buffer2;
    do{
        buffer1=fgets(content1,mxn1,fp1) , buffer2=fgets(content2,mxn1,fp2);
        if(content1[strlen(content1)-1]=='\n')content1[strlen(content1)-1]='\0';
        if(content2[strlen(content2)-1]=='\n')content2[strlen(content2)-1]='\0';
        Line++;
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
                char ans[100] = "\0";
                intToStr(Line,ans);
                fputs("============ #",unt) , fputs(ans,unt) , fputs("============\n",unt) , fputs(contenttmp1,unt) , fputs(contenttmp2,unt);
                continue;
            }
            if(content1[strlen(content1)-1]!='\n')strcat(content1,"\n");
            if(content2[strlen(content2)-1]!='\n')strcat(content2,"\n");
            char ans[100] = "\0";
            intToStr(Line,ans);
            fputs("============ #",unt) , fputs(ans,unt) , fputs("============\n",unt) , fputs(content1,unt) , fputs(content2,unt);
        }
    }
    while(buffer1!=NULL && buffer2!=NULL);
    if(buffer1!=NULL){
        int tmp = Line;
        char contenttmp[mxn]="\0";
        do strcat(contenttmp,content1) , Line++;
        while(fgets(content1,mxn1,fp1)!=NULL);
        char ans1[100] = "\0" , ans2[100] = "\0";
        intToStr(tmp,ans1) , intToStr(Line-1,ans2) ;
        fputs(">>>>>>>>>>>> #",unt) , fputs(ans1,unt) , fputs(" - #",unt) , fputs(ans2,unt) , fputs(" >>>>>>>>>>>>\n" , unt) , fputs(contenttmp,unt);
    }
    if(buffer2!=NULL){
        int tmp = line;
        char contenttmp[mxn]="\0";
        do strcat(contenttmp,content2) , line++;
        while(fgets(content2,mxn1,fp2)!=NULL);
        char ans1[100] = "\0" , ans2[100] = "\0";
        intToStr(tmp,ans1) , intToStr(Line-1,ans2) ;
        fputs(">>>>>>>>>>>> #",unt) , fputs(ans1,unt) , fputs(" - #",unt) , fputs(ans2,unt) , fputs(" >>>>>>>>>>>>\n" , unt) , fputs(contenttmp,unt);
    }
    fclose(fp1) , fclose(fp2)  ,fclose(unt) , strcmp(name , untitled) , openFile();
}
/*void dirtree(char basepath[] , int root ,int depth){
    if(depth<-1){
        move(height-1,0) , clrtoeol();
        printw("Invalid Command : invalid depth");
        getch();
        move(height-1,0) , clrtoeol();
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
                    fputc(c1,unt);
                }
                else{
                    fputs(" ",unt);
                }
            }
            if(fromArman)strncat(armn,&c2,1), strncat(armn,&c3,1), strcat(armn,entry->d_name), strcat(armn,"\n");
            else printf("%c%c%s\n", 195, 196, entry->d_name);
            strcpy(path, basepath) , strcat(path, "/") , strcat(path, entry->d_name);
            dirtree(path, root + 2 ,depth,armn,fromArman);
        }
    }
    closedir(folder);
}*/

void commandSplitter(char input[] , char command[]){
    for(int i = 0; i<strlen(input) && input[i]!=' ';i++)strncat(command ,input+i,1);
}
int strToInt(char str[]){
    int powerOfTen=1 , ans = 0;
    for(int i=strlen(str)-1;i>=0;i--)ans+=(str[i]-'0')*powerOfTen , powerOfTen*=10;
    return ans;
}
void indent(char path[]){
    FILE * fp , * tmp;
    fp = fopen(path , "r+") , tmp = fopen("replace.tmp" , "w+") ;
    char content[100] = "\0" , lastchar = ' ';
    int opCnt = 0 , flag = 0 ,i=0;
    while(fgets(content,100,fp)!=NULL){
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
    fclose(fp) , fclose(tmp) , remove(path) , rename("replace.tmp",path);
}
void rmv2(int line1 , int pos1 , int line2 , int pos2){
    if(line1 > line2 || (line1 == line2 && pos1 > pos2)){
        int tmp1 = line1 , tmp2 = pos1;
        line1 = line2 , line2 = tmp1 , pos1 = pos2 , pos2 = tmp2;
    }
    char path[] = "Untitled.txt";
    FILE * fp , * tmp , *tmp1 ;
    fp = fopen(path , "r+") , tmp = fopen("replace.tmp" , "w+"), tmp1 = fopen("remove.txt" , "w+");
    char contenttmp[100]="\0";
    int cnt=0 , charNum = 0;
    while(fgets(contenttmp , 100 , fp)!=NULL){
        cnt++ , charNum+=strlen(contenttmp);
        fputs(contenttmp,tmp1);
        if(cnt == line1-1)pos1 += charNum;
        if(cnt == line2-1)pos2 += charNum;
    }
    strcpy(clipBoard , "\0");
    fclose(tmp1) , tmp1 = fopen("remove.txt" , "r") , charNum=0;
    while(fgets(contenttmp , 100 , tmp1)!=NULL){
        for(int i=0;i<strlen(contenttmp);i++){
            if(charNum<pos1 || charNum>=pos2)fputc(contenttmp[i],tmp);
            else strncat(clipBoard,&contenttmp[i],1);
            charNum++;
        }
    }
    fclose(fp) , fclose(tmp) , fclose(tmp1) , remove(path) , remove("remove.txt") , rename("replace.tmp",path);
}
 void copy2(int line1 , int pos1 , int line2 , int pos2){
    if(line1 > line2 || (line1 == line2 && pos1 > pos2)){
        int tmp1 = line1 , tmp2 = line2;
        line1 = line2 , line2 = tmp1 , pos1 = pos2 , pos2 = tmp2;
    }
     char path[] = "Untitled.txt";
     FILE * fp , *tmp1;
     fp = fopen(path , "r+") , tmp1 = fopen("copy.txt" , "w+");
     char contenttmp[100]="\0";
     int cnt=0 , charNum = 0;
     while(fgets(contenttmp , 100 , fp)!=NULL){
         cnt++ , charNum+=strlen(contenttmp);
         fputs(contenttmp,tmp1);
         if(cnt == line1-1)pos1 += charNum;
         if(cnt == line2-1)pos2 += charNum;
     }
     fclose(tmp1) , tmp1 = fopen("copy.txt" , "r") , charNum=0;
     strcpy(clipBoard," ");
     while(fgets(contenttmp , 100 , tmp1)!=NULL){
         for(int i=0;i<strlen(contenttmp);i++){
             if(charNum>=pos1 && charNum<pos2)clipBoard[charNum-pos1] = contenttmp[i];
             charNum++;
         }
     }
     fclose(fp)  , fclose(tmp1) , remove("copy.txt");
 }
 void insertForPhase2(char content[] , int line , int pos){
    char path[] = "Untitled.txt";
    FILE * fp , * tmp;
    fp = fopen(path , "r+") , tmp = fopen("replace.tmp" , "w+") ;
    char contenttmp[100]="\0" , *buffer;
    int cnt=0;
    while((buffer = fgets(contenttmp ,100 ,fp))!=NULL || cnt<=line){
        cnt++;
        if(buffer == NULL){
            if(cnt!=line)fputs("\n" , tmp);
            else{
                char rep[500]="\0";
                for(int i=0;i<pos;i++)rep[i] = ' ';
                for(int i=pos;i<pos+strlen(content);i++)rep[i] = content[i-pos];
                if(rep[strlen(rep)-1]!='\n')rep[strlen(rep)] = '\n';
                fputs(rep,tmp);
            }
            continue;
        }
        if(cnt == line){
            char rep[500] = "\0";
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
char * correctLine(int a ){
    char * ans = (char *)malloc(4);
    *(ans+3) = '\0';
    if(!(a/10))*ans = '0' , *(ans+1) = '0' , *(ans+2) = a+'0';
    else if((a/10) && !(a/100))*ans = '0' , *(ans+1) = (a/10) + '0' , *(ans+2) = (a%10) +'0';
    else *ans = (a/100)+'0' , *(ans+1) = (a%100)/10 +'0' , *(ans+2) = (a%10)+'0';
    return ans;
}
void adjustScreen(int up){
    FILE * fp = fopen("Untitled.txt" , "r");
    if(up==-1){
        line = 0;
        char content[100] = "\0";
        for(int i=0;i<firstline;i++)fgets(content,100,fp) , numOfChars[i+1]=strlen(content)-1 , line++;
        for(int i=0;i<height-2;i++)move(i,0) , clrtoeol();
        if(Find){
            lineFirstHappen = -1;
            int happen = 0;
            init_pair(4,COLOR_BLACK , COLOR_YELLOW);
            int exist = 0 , l = 0 , len = strlen(commandStr) ,cnt=0;
            while(fgets(content , 100 , fp)!=NULL && cnt<height-2){
                if(content[strlen(content)-1] == '\n')content[strlen(content)-1] ='\0';
                numOfChars[cnt+firstline+1]=strlen(content) ,move(cnt,0);
                printw("%s ",correctLine(cnt+firstline+1) , len);
                strcat(content," ");
                int length = strlen(content);
                for(int i=0;i<length;i++){
                    if(exist && l<len){
                        attron(COLOR_PAIR(4));
                        printw("%c" , content[i]) , l++;
                        attroff(COLOR_PAIR(4));
                        continue;
                    }
                    else if(exist && l>=len) exist=0 , l=0;
                    char substitute[100] = "\0";
                    for(int j=i;j<min(i+len,length);j++)strncat(substitute,&content[j],1);
                    if(!strcmp(substitute,commandStr) && (content[i+len]==' ' || content[i+len]=='\n') && (!i || content[i-1]==' ' || content[i-1]=='\n')){
                        exist =1;
                        attron(COLOR_PAIR(4));
                        if(!happen && (cnt+firstline+1>lineHolder || (cnt+firstline+1==lineHolder && i+startline > xcomm)))happen=1 , lineFirstHappen = firstline+cnt , posFirstHappen = i;
                        printw("%c" , content[i]) , l++;
                        attroff(COLOR_PAIR(4));
                    }
                    else printw("%c" , content[i]);
                }
                cnt++ , line++ , clrtoeol();
            }
        }
        else{
            for(int i=0;i<height-2 && fgets(content,100,fp)!=NULL; i++){
                if(content[strlen(content)-1]=='\n')content[strlen(content)-1]='\0';
                numOfChars[i+firstline+1]=strlen(content) ,move(i,0) , printw("%s %s",correctLine(firstline+i+1),content) , line++;
            }
        }
        for(int i=firstline+height-1;fgets(content,100,fp)!=NULL;i++)numOfChars[i] = (content[strlen(content)-1]=='\n') ? strlen(content)-1 : strlen(content) , line++;
    }
    if(!up){
        line = 0, firstline++;
        char content[100] = "\0";
        for(int i=0;i<firstline;i++)fgets(content,100,fp),numOfChars[i+1]=strlen(content)-1,line++;
        for(int i=0;i<height-2; i++){
            fgets(content,100,fp);
            if(content[strlen(content)-1]=='\n')content[strlen(content)-1]='\0';
            numOfChars[i+firstline+1]=strlen(content) ,move(i,0) , clrtoeol() , move(i,0) , printw("%s %s",correctLine(firstline+i+1),content) , line++;
        }
        for(int i=firstline+height-1;fgets(content,100,fp)!=NULL;i++)numOfChars[i] = (content[strlen(content)-1]=='\n') ? strlen(content)-1 : strlen(content) , line++;
    }
    if(up==1){
        line = 0, firstline-- ;
        char content[100] = "\0";
        for(int i=0;i<firstline;i++)fgets(content,100,fp) ,numOfChars[i+1]=strlen(content)-1, line++;
        for(int i=0;i<height-2; i++){
            fgets(content,100,fp);
            if(content[strlen(content)-1]=='\n')content[strlen(content)-1]='\0';
            numOfChars[i+firstline+1]=strlen(content) ,move(i,0) , clrtoeol() , move(i,0) , printw("%s %s",correctLine(firstline+i+1),content) , line++;
        }
        for(int i=firstline+height-1;fgets(content,100,fp)!=NULL;i++)numOfChars[i] = (content[strlen(content)-1]=='\n') ? strlen(content)-1 : strlen(content) , line++;
    }
    move(y,x) , lastline = min(line-1,firstline+height-3);
    refresh();
    fclose(fp);
}
void insertChar(char content , int line , int pos){
    char path[] = "Untitled.txt";
    FILE * fp , * tmp;
    fp = fopen(path , "r+") , tmp = fopen("replace.tmp" , "w+") ;
    char contenttmp[mxn1]="\0" , *buffer;
    int cnt=0;
    while((buffer = fgets(contenttmp , mxn1 ,fp))!=NULL || cnt<=line){
        cnt++;
        if(buffer == NULL){
            if(cnt!=line)fputs("\n" , tmp);
            else{
                char rep[mxn1]="\0";
                for(int i=0;i<pos;i++)rep[i] = ' ';
                for(int i=pos;i<pos+1;i++)rep[i] = content;
                if(rep[strlen(rep)-1]!='\n')rep[strlen(rep)] = '\n';
                fputs(rep,tmp);
            }
            continue;
        }
        if(cnt == line){
            char rep[mxn1] = "\0";
            for(int i=0;i<min(pos,strlen(contenttmp)-1);i++)rep[i] = contenttmp[i];
            for(int i=min(pos,strlen(contenttmp)-1);i<pos ;i++)rep[i] = ' ';
            for(int i=pos;i<pos+1;i++)rep[i] = content;
            for(int i=pos+1;i<1+strlen(contenttmp);i++)rep[i] = contenttmp[i-1];
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
void saver(){
    FILE * fp = fopen("Untitled.txt" , "r") , *fp1 = fopen(name,"w+");
    char content[1000] = "\0";
    while(fgets(content,1000,fp)!=NULL)fputs(content,fp1);
    fclose(fp) , fclose(fp1);
}
void adjusDownScreen(int a , int b){
    move(height-2,0) , clrtoeol();
    init_pair(1,COLOR_BLACK , COLOR_BLUE);
    attron(COLOR_PAIR(1));
    mvprintw(height-2,0," ");
    mvprintw(height-2,1,"%s",state);
    mvprintw(height-2,strlen(state)+1," ");
    attroff(COLOR_PAIR(1));
    init_pair(2,COLOR_BLACK,COLOR_GREEN);
    attron(COLOR_PAIR(2));
    mvprintw(height-2,strlen(state)+2," ");
    mvprintw(height-2,strlen(state)+3,"%s",name);
    if(!saved){
        mvprintw(height-2,strlen(state)+strlen(name)+3,"   + ");
        attroff(COLOR_PAIR(2));
        init_pair(3,COLOR_BLACK,COLOR_YELLOW);
        attron(COLOR_PAIR(3));
        for(int i=strlen(state) + strlen(name)+8 ; i<width ; i++)mvprintw(height-2,i," ");
        attroff(COLOR_PAIR(3));
    }
    else {
        mvprintw(height-2,strlen(state)+strlen(name)+3," ");
        attroff(COLOR_PAIR(2));
        init_pair(3,COLOR_BLACK,COLOR_YELLOW);
        attron(COLOR_PAIR(3));
        for(int i=strlen(state) + strlen(name)+4 ; i<width ; i++)mvprintw(height-2,i," ");
        attroff(COLOR_PAIR(3));
    }
    move(b,a);
}
void openFile(){
    if(strcmp(name,"Untitled.txt"))saver();
    lineHolder = 1 , firstline = 0 , x = startline , y=0 , adjustScreen(-1);
    return;
}
int main(){
    mkdir("root",0700) , mkdir("undoFiles",0700);
    initscr();
    start_color();
    FILE * fp = fopen("Untitled.txt","w");
    WINDOW * win = newwin(width,height,0,0);
    refresh() , noecho() , adjusDownScreen(0,0) , mvprintw(0,0,"001 ") , fclose(fp);
    while(1){
        char c = getch();
        if(Find && c=='n' && lineFirstHappen!=-1)x=posFirstHappen+startline , y=lineFirstHappen , lineHolder=y+1, move(y,x);
        else if(Find)Find=0 , adjustScreen(-1);
        else if(c== 127 && (!strcmp(state,"INSERT") || command)){
            if(command && x)x-- , move(y,x) , delch() , commandStr[strlen(commandStr)-1]='\0';
            else if(!command && x>startline)rmv2(lineHolder,x-startline,lineHolder,x-startline-1), x-- , move(y,x) , delch() ,numOfChars[lineHolder]-- ;
        }
        else if(c=='i' && strcmp(state , "INSERT") && !command)strcpy(state , "INSERT") , adjusDownScreen(x,y);
        else if(c== '~' && strcmp(state , "NORMAL"))strcpy(state , "NORMAL") , adjusDownScreen(x,y);
        else if(c== 'v' && !strcmp(state , "NORMAL") && !command)strcpy(state , "VISUALIZE") ,xcomm=x-startline , ycomm=lineHolder, adjusDownScreen(x,y);
        else if(c=='\n' && (!strcmp(state,"INSERT") || command)){
            if(saved) saved = 0 , adjusDownScreen(y,x);
            if(command){
                move(height-1,0) , clrtoeol();
                if(!strcmp(commandStr,":save")){
                    if(!strcmp(name,"Untitled.txt"))printw("At first you should choose a name for your file!"),getch(),move(height-1,0) , clrtoeol() ,x=xcomm , y=ycomm, move(y,x);
                    else   saver() ,undoSaver2(), saved=1 , x=xcomm , y=ycomm , adjusDownScreen(x,y);
                }
                else if(strstr(commandStr , ":saveas")!=NULL){
                    if(commandStr[8]=='"')commandStr[strlen(commandStr)-1]='\0' , strcpy(name,commandStr+9);
                    else strcpy(name,commandStr+8);
                    saver() , undoSaver2() , saved = 1 , x=xcomm , y=ycomm ,  adjusDownScreen(x,y);
                }
                else if(strstr(commandStr,":open")!= NULL){
                    if(strcmp(name , "Untitled.txt"))saver();
                    if(commandStr[6]=='"')commandStr[strlen(commandStr)-1]='\0' , strcpy(name,commandStr+7);
                    else strcpy(name,commandStr+6);
                    move(0,0) , clrtobot() , saved=1 , adjusDownScreen(0,0);
                    FILE * fp = fopen(name,"r") , *fp1 = fopen("Untitled.txt", "w");
                    char content[100] = "\0";
                    firstline = 0 , lastline = 0 , lineHolder = 1;
                    for(int i=0;i<height-2 && fgets(content,100,fp)!=NULL;i++)move(i,0) , printw("%s %s",correctLine(i+1),content) , line++ , fputs(content,fp1);
                    while ((fgets(content,100,fp))!=NULL)line++ , fputs(content,fp1);
                    fclose(fp) , fclose(fp1) , x=startline , y=0 ,  move(y,x);
                }
                else if(!strcmp(commandStr,":undo")){
                    if(!saved)undo2() , adjustScreen(-1) ;
                    x=startline , y=0 , lineHolder = firstline+1 ,  move(y,x);
                }
                else if(commandStr[0]=='\\'){
                    strcpy(commandStr,commandStr+1);
                    Find = 1 , adjustScreen(-1) , y=ycomm , x=xcomm , move(y,x);
                }
                else if(commandStr[0]==':'){
                    char input[mxn1]="\0" , Command[mxn1]="\0";
                    strcpy(commandStr , commandStr+1) , strcpy(input,commandStr);
                    commandSplitter(input,Command);
                    if(!strcmp(Command,"createfile")){
                        int i= (strstr(input,"--file")-input)+7;
                        if(input[i]=='\"')i++ , input[strlen(input)-1]='\0';
                        if(input[i]=='/')i++;
                        create(input+i);
                        y=ycomm , x=xcomm , move(y,x);
                    }
                    else if(!strcmp(Command,"insertstr")){
                        int  i=(strstr(input,"--file")-input)+7 , Line , pos , flag = 0;
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
                        sscanf(input+i,"pos %d:%d",&Line,&pos);
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
                        insert(file,content,Line,pos);
                        y=ycomm , x=xcomm , move(y,x);
                    }
                    else if(!strcmp(Command,"cat")){
                        int i=(strstr(input,"--file")-input)+7;
                        if(input[i]=='\"')i++ , input[strlen(input)-1]='\0';
                        if(input[i]=='/')i++;
                        cat(input+i);
                    }
                    else if(!strcmp(Command,"removestr")){
                        int i=(strstr(input,"--file")-input)+7 , Line , pos , size , flag=0;
                        char file[mxn1]="\0" , c;
                        if(input[i]=='"')i++ , flag=1;
                        if(input[i]=='/')i++;
                        for(i;input[i]!=' ' || flag;strncat(file,&input[i++],1)){
                            if(flag && input[i]=='"')flag = 0;
                        }
                        i = (strstr(input,"--pos")-input)+2;
                        if(file[strlen(file)-1]=='\"')file[strlen(file)-1]='\0';
                        sscanf(input+i,"pos %d:%d -size %d -%c",&Line,&pos,&size,&c);
                        rmv(file,Line,pos,size,c,0,0);
                        y=ycomm , x=xcomm , move(y,x);
                    }
                    else if(!strcmp(Command,"copystr")){
                        int i=(strstr(input,"--file")-input)+7 , Line , pos , size , flag=0;
                        char file[mxn1]="\0" , c;
                        if(input[i]=='"')i++ , flag=1;
                        if(input[i]=='/')i++;
                        for(i;input[i]!=' ' || flag;strncat(file,&input[i++],1)){
                            if(flag && input[i]=='"')flag = 0;
                        }
                        i = (strstr(input,"--pos")-input)+2;
                        if(file[strlen(file)-1]=='\"')file[strlen(file)-1]='\0';
                        sscanf(input+i,"pos %d:%d -size %d -%c",&Line,&pos,&size,&c);
                        copy(file,Line,pos,size,c,0);
                        y=ycomm , x=xcomm , move(y,x);
                    }
                    else if(!strcmp(Command,"cutstr")){
                        int i=(strstr(input,"--file")-input)+7 , Line , pos , size , flag=0;
                        char file[mxn1]="\0" , c;
                        if(input[i]=='"')i++ , flag=1;
                        if(input[i]=='/')i++;
                        for(i;input[i]!=' ' || flag;strncat(file,&input[i++],1)){
                            if(flag && input[i]=='"')flag = 0;
                        }
                        i = (strstr(input,"--pos")-input)+2;
                        if(file[strlen(file)-1]=='\"')file[strlen(file)-1]='\0';
                        sscanf(input+i,"pos %d:%d -size %d -%c",&Line,&pos,&size,&c);
                        cut(file,Line,pos,size,c);
                        y=ycomm , x=xcomm , move(y,x);
                    }
                    else if(!strcmp(Command,"paste")){
                        int  i=(strstr(input,"--file")-input)+7 , Line , pos , flag=0;
                        char file[mxn1]="\0";
                        if(input[i]=='"')i++ , flag = 1;
                        if(input[i]=='/')i++;
                        for(i;input[i]!=' ' || flag;strncat(file,&input[i++],1)){
                            if(flag && input[i]=='"')flag = 0;
                        }
                        if(file[strlen(file)-1]=='\"')file[strlen(file)-1]='\0';
                        i = (strstr(input,"--pos")-input)+2;
                        sscanf(input+i,"pos %d:%d",&Line,&pos);
                        paste(file,Line,pos);
                        y=ycomm , x=xcomm , move(y,x);
                    }
                    else if(!strcmp(Command,"undo")){
                        int i=(strstr(input,"--file")-input)+7;
                        if(input[i]=='\"')i++ , input[strlen(input)-1]='\0';
                        if(input[i]=='/')i++;
                        undo(input+i);
                    }
                    else if(!strcmp(Command,"auto-indent")){
                        int i=(strstr(input,"--file")-input)+7;
                        if(input[i]=='\"')i++ , input[strlen(input)-1]='\0';
                        if(input[i]=='/')i++;
                        indent(input+i);
                    }
                    else if(!strcmp(Command,"compare")){
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
                    /*else if(!strcmp(Command,"tree")){
                        int depth;
                        sscanf(input+5,"%d",&depth) , dirtree("root",2,depth," ",0);
                    }*/
                    else if(!strcmp(Command,"grep")){
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
                        if(strstr(input,"-c")!=NULL && strstr(input,"-l")!=NULL)grep(files,k,content,1,1);
                        else if(strstr(input,"-c")!=NULL)grep(files,k,content,0,1);
                        else if(strstr(input,"-l")!=NULL)grep(files,k,content,1,0);
                        else grep(files,k,content,0,0);
                    }
                    else if(!strcmp(Command,"find")){
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
                        find(file,content,count,at,byword,all,0,star);
                    }
                    else if(!strcmp(Command,"replace")){
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
                        y=ycomm , x=xcomm , move(y,x);
                    }
                    else printw("Invalid command!") , getch(),move(height-1,0) , clrtoeol() , x=xcomm , y=ycomm ,  move(y,x);
                }
                else printw("Invalid command!") , getch(),move(height-1,0) , clrtoeol() , x=xcomm , y=ycomm ,  move(y,x);
                command=0 , strcpy(commandStr,"\0");
                continue;
            }
            insertChar(c,lineHolder,x-startline) , lineHolder++ ;
            x = startline;
            if(!((y>height-6 && line>height-3) || (y==height-3))){
                y++ , adjustScreen(-1);
            }
            else adjustScreen(0);
        }
        else if(c =='\033'){
            getch();
            c = getch();
            if(c == 'A' && lineHolder>1 && !command){
                lineHolder--;
                if(firstline && y<=3){
                    if(x>numOfChars[lineHolder]+startline)x=numOfChars[lineHolder]+startline;
                    adjustScreen(1);
                }
                else{
                    y--;
                    if(x>numOfChars[lineHolder]+2)x=numOfChars[lineHolder]+startline;
                    move(y,x);
                }
            }
            else if(c == 'B' && !command && lineHolder<line){
                lineHolder++ ;
                if(lastline!=line-1 && lineHolder>=height-5){
                    if(x>numOfChars[lineHolder]+startline)x=numOfChars[lineHolder]+startline;
                    adjustScreen(0)  ;
                }
                else {
                    y++ ;
                    if(x>numOfChars[lineHolder]+startline)x=numOfChars[lineHolder]+startline;
                    move(y,x);
                }

            }
            else if(c == 'D' && x>startline)x-- , move(y,x);
            else if(c == 'C' && x<width-1)x++ , move(y,x);
        }
        else if((c=='/' || c==':') && !strcmp(state,"NORMAL")&&!command)xcomm=x , ycomm=y , x=0 , y=height-1,move(y,x) , command=1;
        else if(c=='u' && !saved && !strcmp(state , "NORMAL") && !command)undo2() , adjustScreen(-1) , y=0 , x=startline , move(y,x) , lineHolder = firstline+1;
        else if(c=='d' && !strcmp(state , "VISUALIZE"))rmv2(ycomm,xcomm,lineHolder,x-startline) ,  firstline = min(firstline,ycomm-1) , adjustScreen(-1)  , lineHolder = firstline+1+y  , strcpy(state,"NORMAL") , adjusDownScreen(x,y);
        else if(c=='y' && !strcmp(state , "VISUALIZE"))copy2(ycomm,xcomm,lineHolder,x-startline) , strcpy(state,"NORMAL") , adjusDownScreen(x,y);
        else if(c =='p'&& !strcmp(state,"NORMAL") && !command){
            if(strcmp(clipBoard,"\0"))insertForPhase2(clipBoard,lineHolder,x-startline) , adjustScreen(-1);
        }
        else if(c == '=' && !strcmp(state,"NORMAL") && !command)indent("Untitled.txt") , adjustScreen(-1);
        else if((!strcmp(state,"INSERT") || command) && x<width-1 && numOfChars[lineHolder]<width){
            if(c==127)continue;
            insch(c);
            if(saved) saved = 0 , adjusDownScreen(y,x);
            if(command){
                strncat(commandStr,&c,1) , x++ , move(y,x);
                continue;
            }
            x++;
            insertChar(c,lineHolder,x-startline-1) ,  numOfChars[lineHolder]++ ,move(y,x);
        }
    }


}
// on fire !
