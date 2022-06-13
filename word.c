/**
 * @file word.c
 * @author Young
 * @brief 
 * @version 0.1
 * @date 2022-05-31
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#include "word.h"
#include "encode.h"

#define MAX_LEN (20)
#define dummy_len (10) //关键字每个长度
#define dummy_num (11) //关键字个数

int len = 0 ;
int col = 0, row = 1; //全局变量实时关注文件指针的位置

int overflowed(int n){
    if(n >= MAX_LEN) 
        return 1;
    else return 0;
}

void tk_print(Token *tk){
    switch(tk->kind){
        case TK_ID: printf("标识符：%s\n", tk->str); break;
        case TK_NUM: printf("常数：%s\n", tk->str); break;
        case TK_SEMI: printf("分届符：%s\n", tk->str); break;
        case TK_ASSIN: printf("赋值符号：%s\n", tk->str); break;
        case TK_DUMMY: printf("保留字：%s\n", tk->str); break;
    }
    return;
}

int dummy(Token *tk){
    char key[dummy_num][dummy_len] = {"while", "if", "fi", "do", "od", "write", "then", "var", "skip", "read", "else"};
    int i = 1;
    for(i = 1; i <= dummy_num; ++i){
        if(!strcmp(tk->str, *(key+i-1))){
            return i;
        }
    }
    return 0;
}

Token* get_token(FILE* fp){
    char c, *s;
    int n = 0;
    s = (char* )malloc(sizeof(char)*10);
    Token *tk = (Token* )malloc(sizeof(Token));
    while((c = fgetc(fp)) != EOF){
        if(c == '\n') {
            row++;
            col = 0;
        }
        else col++;
        while(isspace(c)){
            c = fgetc(fp);
            if(c == '\n') {
                row++;
                col = 0;
            }
            else col++;
        }
        if(c == EOF)
            break;
        if (c <= '9' && c >= '0'){
            do{
                s[n++] = c;
                c = fgetc(fp);
                col++;
                if(overflowed(n)) {
                    printf("%d",n);
                    tk->kind = TK_ERR;
                    tk->l_err = err_len;
                    tk->n_col = col;
                    tk->n_row = row;
                    return tk;
                }
            }while(c <= '9' && c >= '0');
            if(c != EOF) {
                fseek(fp,-1L,1);
                col--;
            }
            tk->kind = TK_NUM; //判断为常数
            s[n] = '\0';
            strcpy(tk->str, s);
            tk->n_col = col;
            tk->n_row = row;
            return tk;
        }
        else if(c <= 'z' && c >= 'a'){
            do{
                s[n++] = c;
                c = fgetc(fp);
                col++;
                if(overflowed(n)) {
                    tk->kind = TK_ERR;
                    tk->l_err = err_len;
                    tk->n_col = col;
                    tk->n_row = row;
                    return tk;
                }
            }while(c <= '9' && c >= '0' ||  c <= 'z' && c >= 'a');
            s[n] = '\0';
            strcpy(tk->str, s);
            if(tk->d_kind = dummy(tk))
                tk->kind = TK_DUMMY;
            else
                tk->kind = TK_ID;
            if(c != EOF) {
                fseek(fp,-1L,1);
                col--;
            }
            tk->n_col = col;
            tk->n_row = row;
            return tk;
        }
        else if(c <= '-' && c >= '(' || c == '/' || c == '~' || c == ';' || c == '=' || c == '<'){
            s[n++] = c;
            s[n] = '\0';
            tk->kind = TK_SEMI; //判断为分界符
            strcpy(tk->str, s);
            tk->n_col = col;
            tk->n_row = row;
            return tk;
        }
        else if( c == ':'){
            s[n++] = c;
            if((c = fgetc(fp)) != '='){
                tk->kind = 6;
                fseek(fp,-1L,1);
                tk->n_col = col;
                tk->n_row = row;
                return tk;
            }
            else s[n++] = c;
            col++;
            s[n] = '\0';
            tk->kind = 3; //判断为赋值符号
            strcpy(tk->str, s);
            tk->n_col = col;
            tk->n_row = row;
            return tk;
        }
        else {
            tk->kind = TK_ERR; //输入有误
            tk->n_col = col;
            tk->n_row = row;
            return tk;
        }
    } 
    tk->n_col = col;
    tk->n_row = row;
    tk->kind = TK_EOF; //判断文件结束
    return tk;
    free(tk);
}

void read_word(FILE* fp){
    tk_p = (Token** )malloc((len+1)*sizeof(Token*));
    do{
        tk_p[len] = get_token(fp);
        if(tk_p[len]->kind == TK_ERR){
            err_loc_print();
            printf("输入有误！错误位置：");
            break;
        }
        ++len;
        tk_p = (Token** )realloc(tk_p, (len+1)*sizeof(Token*));   
    }while(tk_p[len-1]->kind != TK_EOF);
    return ;
}

int return_len(){
    return len;
}