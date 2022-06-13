/**
 * @file main.c
 * @author Young
 * @brief 
 * @version 0.1
 * @date 2022-06-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<math.h>

#include"word.h"
#include"encode.h"

int main(){
    printf("\nプログラムスタート:\n");//程序开始执行
    FILE *fp = fopen("t5.txt", "r"); 
    if(!(fp == NULL)){
        read_word(fp);
        fclose(fp);
        printf("文法の分析することは始めよう:\n");//文法分析开始
        P();
        if(return_OK()){
            printf("\n\nコードを生成する:\n");//中间代码生成
            test_result();
            printf("\n分析も生成の階段は終わりだ\n");//文法分析和中间代码生成完成
        }
        else printf("\n\n失敗するね~");//失败了呢
    }
    else printf("\nファイルパスにチェックしてください\n");//文件的路径有问题
    return 0;
}