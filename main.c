//
//  main.c
//  OS_2020_tmp1
//
//  Created by 顾孟瀚 on 1/16/20.
//  Copyright © 2020 顾孟瀚. All rights reserved.
//


//
//  main.c
//  OS_2020_tmp1
//
//  Created by 顾孟瀚 on 1/16/20.
//  Copyright © 2020 顾孟瀚. All rights reserved.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//each word has a maximum size of 128
#define MAX 128

//pure_digit finds out whether the content of a char* is pure digit
//this function is used to check whether *(argv+1) is a valid cache size
int pure_digit(char* str){
    int i;
    for (i=0;*(str+i)!='\0';i++){
        if (*(str+i)>'9'||*(str+i)<'0'){
            return 0;
        }
    }
    return 1;
}

//num_of_words finds out how many segmentations each input word has
//that is, if an input word is "Gutenberg's", this function return 2
//because "Gutenberg's" is separated to 2 parts
int num_of_words(char * ptr){
    int i=0,count=0;
    for (i=0;*(ptr+i)!='\0';i++){
        if (!((*(ptr+i)<='Z'&&*(ptr+i)>='A')||(*(ptr+i)<='z'&&*(ptr+i)>='a')||(*(ptr+i)<='9'&&*(ptr+i)>='0'))){
            count++;
        }
    }
    return count+1;
}

//divide_words returns an array of the segmentations of each input word
//that is, if an input word is "Gutenberg's", this function return an char** cache with *cache="Gutenberg" and *(cache+1)="s"
char** divide_words(char *ptr){
    char** target;
    //num is the number of words after one word is divided by symbols
    int num=num_of_words(ptr);
    target=(char**)calloc(num, sizeof(char*));
    //i refers to the index in the original string
    //j refers to the index of a string in char** target
    int i=0,j=0;
    //index refers to the index in a particular word divided from an original word
    int index=0;
    //tmp refers to a template string
    char * tmp;
    tmp=(char*)calloc(MAX,sizeof(char));
    
    for(i=0;*(ptr+i)!='\0';i++){
        
        if((*(ptr+i)<='Z'&&*(ptr+i)>='A')||(*(ptr+i)<='z'&&*(ptr+i)>='a')||(*(ptr+i)<='9'&&*(ptr+i)>='0')){
            
            *(tmp+index)=*(ptr+i);
            index++;
        }
        else{
            //allocate '\0' at the end of each segmentation to make it an entire string
            *(tmp+index)='\0';
            *(target+j)=tmp;
            //printf("%s\n",*(target+j));
            index=0;
            j++;
            //calloc another space for next word
            tmp=(char*)calloc(MAX,sizeof(char));
        }
    }
    //adding the last divided word
    *(tmp+index)='\0';
    *(target+j)=tmp;
    //printf("%s\n",*(target+j));
    //free(tmp);
    return target;
}

//ASC2_hash returns an int indentifying where a certain string should be inserted in char** cache
int ASC2_hash (char * str, int array_size){
    int ASC=0;
    int i=0;
    for (i=0;*(str+i)!='\0';i++){
        ASC+=(int)(*(str+i));
    }
    return ASC % array_size;
}

int main(int argc, char** argv)
{
    //too few arguments error
    if (argc<3){
        fprintf(stderr, "ERROR: <Too Few Arguments>\n");
        return EXIT_FAILURE;
    }
    setvbuf(stdout,NULL,_IONBF,0);
    FILE *fp=NULL;
    //open the file to read
    fp=fopen(*(argv+2),"r");
    // missing input file error
    if (fp==NULL){
        fprintf(stderr, "ERROR: <Missing Input File>\n");
        return EXIT_FAILURE;
    }
    // invalid cache size error
    if (!pure_digit(*(argv+1))){
        fprintf(stderr, "ERROR: <Invalid Cache Size>\n");
        return EXIT_FAILURE;
    }
    
    int array_size=atoi(*(argv+1)); //array_size is the size of the cache
    char** cache;
    cache=(char**)calloc(array_size,sizeof(char*));
    //calloc failure error
    if (cache==NULL){
        fprintf(stderr, "ERROR: <Function Calloc() Failed>\n");
        return EXIT_FAILURE;
    }
    char* str;
    str=calloc(MAX,sizeof(char));  //allocate spaces for str
    int num_words=0;
    //EOF means the end of a file for fscanf
    while (fscanf(fp,"%s",str)!=EOF){
        
        // target of "Gutenberg's" is a char** contains "Gutenberg" and "s"
        char** target=divide_words(str);
        //num_words of "Gutenberg's" is 2
        num_words=num_of_words(str);
        //printf("%s, %s:%lu\n", str, target, strlen(target));
        int i=0;
        for (i=0;i<num_words;i++){
            if (strlen(*(target+i))<3) {
                free(*(target+i));
                continue;
            }
            //remain refers to where one word should be put in cache
            int remain=ASC2_hash(*(target+i), array_size);
            //*(cache+remain)==NULL means hasn't been allocated
            if (*(cache+remain)==NULL){
                *(cache+remain)=(char*)calloc(strlen(*(target+i))+1, sizeof(char));
                strcpy(*(cache+remain),*(target+i));
                printf("Word \"%s\" ==> %d (calloc)\n", *(target+i), remain);
            }
            //*(cache+remain)!=NULL means has been allocated
            else{
                *(cache+remain)=(char*)realloc(*(cache+remain), strlen(*(target+i))+1);
                strcpy(*(cache+remain),*(target+i));
                printf("Word \"%s\" ==> %d (realloc)\n", *(target+i), remain);
            }
            free(*(target+i));
        }
        free(target);
    }
    
    fclose(fp);
    int i=0;
    for (i=0;i<array_size;i++){
        if (*(cache+i)!=NULL){
            printf("Cache index %d ==> \"%s\"\n",i,*(cache+i));
        }
    }
    free(str);
    for (i=0;i<array_size;i++){
        free(*(cache+i));
    }
    free(cache);
    //#ifdef DEBUG_MODE
    //    fflush( stdout );
    //#endif
    
    //exit(EXIT_FAILURE); //without giving segmentation fault
    
    
    return EXIT_SUCCESS;
}
