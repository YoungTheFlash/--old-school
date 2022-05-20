#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#define MAX_LEN (20) //单词长度限制
#define dummy_len (10) //关键字每个长度
#define dummy_num (11) //关键字个数
int col = 0, row = 1; //全局变量实时关注文件指针的位置
int len = 0, i = 0;
char** ID_bag;
int* ID_place;
int bag_len = 0;
enum tok_kind{
    TK_ID = 1, TK_NUM, TK_SEMI, TK_ASSIN,
    TK_DUMMY, TK_EOF, TK_ERR
};

enum lex_err{
    err_char, err_len
};

enum dummy_kind{
    d_while = 1, d_if, d_fi, d_do, d_od, 
    d_write, d_then, d_var, d_skip, d_read, d_else
};

enum grammar_err{
    illegal_err = 1, dismatch_err, undefine_err, dual_err
};

typedef struct{
    enum tok_kind kind;
    char str[MAX_LEN];
    enum dummy_kind d_kind;
    int n_row;
    int n_col;
    enum lex_err l_err;
    enum grammar_err g_err;
}Token;
Token** tk_p;

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
            col++;
            c = fgetc(fp);
            if(c == '\n') {
                row++;
                col = 0;
            }
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

void move_i(){
    if(i == len) 
        printf("\nError of i");
    else 
        ++i;
}

void back_i(){
    --i;
}

void err_loc_print(){
    printf("\nline: %d list: %d", tk_p[i]->n_row, tk_p[i]->n_col);
}

int P();
int X();
int L();
int E();
int S();

int got_if(){
    int OK = 1;
    if(!(OK = E())) 
        return OK;
    move_i();
    if(!strcmp(tk_p[i]->str, "then")){
        printf("\n%s[210]",tk_p[i]->str);//test
        OK = L();
        if(!OK) return OK;
        move_i();
        if(!strcmp(tk_p[i]->str, "else")){
            printf("\n%s[214]",tk_p[i]->str);//test
            OK = L();
            if(!OK) return OK;
            move_i();
            if(strcmp(tk_p[i]->str, "fi")){
                OK = 0;
                err_loc_print();
                printf(" expect a 'fi' here[%s][221]",tk_p[i]->str);//test
            }
            else{
                printf("\n%s[224]",tk_p[i]->str);//test
            }
        }
        else{
            OK = 0;
            err_loc_print();
            printf(" expect a 'else' here[230]");//test
        }
    }
    else{
        OK = 0;
        err_loc_print();
        printf(" expect a 'then' here[236]");//test
    }   
    return OK;
}

int got_while(){
    int OK = 1;
    OK = E();
    if(!OK) return OK;
    move_i();
    if(!strcmp(tk_p[i]->str, "do")){
        printf("\n%s[246]",tk_p[i]->str);//test
        OK = L();
        move_i();
        if(strcmp(tk_p[i]->str, "od")){
            OK = 0;
            err_loc_print();
            printf(" expect a 'od' here[%s][252]", tk_p[i]->str);//test
        }
        else{
            printf("\n%s[255]",tk_p[i]->str);//test
        }
    }
    else{
        OK = 0;
        err_loc_print();
        printf(" expect a 'do' here[261]");//test
    } 
    return OK;
}

int got_read(){
    int k, OK = 1;
    move_i();
    if(!strcmp(tk_p[i]->str, "(")){
        printf("%s[270]",tk_p[i]->str);//test
        move_i();
        for(k = 0; k < bag_len; ++k){
            if(!strcmp(tk_p[i]->str, ID_bag[k])) break;
        }
        if(k == bag_len){
            OK = 0;
            err_loc_print();
            printf(" %s wasn't identified[278]", tk_p[i]->str);//test
        } 
        else{
            printf("%s[281]",tk_p[i]->str);//test scanf("%d", &ID_place[k]);            
            move_i();
            if(strcmp(tk_p[i]->str, ")")){
                OK = 0;
                err_loc_print();
                printf(" expect a ')' here[286]");//test
            }
            else{
                printf("%s[289]",tk_p[i]->str);//test
            }
        }
    }
    else{
        OK = 0;
        err_loc_print();
        printf(" expect a '(' here[296]");//test
    }
    return OK;
}

int got_write(){ 
    int k, OK = 1;
    move_i();
    if(!strcmp(tk_p[i]->str, "(")){
        printf("%s[307]",tk_p[i]->str);//test
        OK = E();
        if(!OK) return OK;
        move_i();
        if(!strcmp(tk_p[i]->str, ")")){
            printf("%s[308]",tk_p[i]->str);//test
        }
        else{
            OK = 0;
            err_loc_print();
            printf(" expect a ')' here[313]");//test
        }
    }
    else {
        OK = 0;
        err_loc_print();
        printf(" expect a '(' here[319]");//test
    }
    return OK;
}

int got_assign(){
    int k, j, OK = 1;
    for(k = 0; k < bag_len; ++k){
        if(!strcmp(tk_p[i]->str, ID_bag[k])) break;
    }
    if(k == bag_len){
        OK = 0;
        err_loc_print();
        printf("%s wasn't identified here[332]", tk_p[i]->str);//test
    }
    else{
        printf("\n%s[335]",tk_p[i]->str);//test
        move_i();
        if(!strcmp(tk_p[i]->str, ":=")){
            printf("%s[338]",tk_p[i]->str);//test
            OK = E();
            if(!OK) return OK;
        }
        else{
            OK = 0;
            err_loc_print();
            printf(" Expect a ':=' here[345]");//test
        }
    }
    return OK;
}

int D(){
    int k, OK = 1;
    move_i();
    if(tk_p[i]->kind == TK_ID || tk_p[i]->kind == TK_NUM){
        if(tk_p[i]->kind == TK_ID){
            for(k = 0; k < bag_len; ++k){
                if(!strcmp(tk_p[i]->str, ID_bag[k])) 
                    break;
            }
            if(k == bag_len){
                OK = 0;
                err_loc_print();
                printf(" %s wasn't identified[364]", tk_p[i]->str);//test
            }    
            else{
                printf("%s[367]", tk_p[i]->str); //test   
            }
        }
        else if(tk_p[i]->kind == TK_NUM){
            printf("%s[371]", tk_p[i]->str);//test
        }
    }
    else if(!strcmp(tk_p[i]->str, "(")){
        printf("%s[375]",tk_p[i]->str);//test
        OK = E();
        if(!OK) return OK;
        move_i();
        if(!strcmp(tk_p[i]->str, ")")){
            printf("%s[380]",tk_p[i]->str);//test
        }
        else{
            OK = 0;
            err_loc_print();
            printf(" Expect a ')' here[385");//test
        }
    }
    else if(!strcmp(tk_p[i]->str, "~")){
        printf("\n%s[389]",tk_p[i]->str);//test
        OK = E();
    }
    else{
        OK = 0;
        err_loc_print();
        printf(" expect 'V', 'N', '~' or '(' here[395]");//test
    }
    return OK;
}

int D1(){
    int OK = 1;
    move_i();
    if(!strcmp(tk_p[i]->str, "=") || !strcmp(tk_p[i]->str, "<")){
        printf("%s[404]",tk_p[i]->str);//test
        OK = D();
    }
    else back_i();
    return OK;
}

int F(){
    int OK = 1;
    OK = D();
    if(!OK) return OK;
    OK = D1();
    return OK;
}

int FS(){
    int OK = 1;
    move_i();
    if(!strcmp(tk_p[i]->str, "*") || !strcmp(tk_p[i]->str, "/")){
        printf("\n%s[427]",tk_p[i]->str);//test
        OK = F();
        if(!OK) return OK;
        OK = FS();
    }
    else back_i();
    return OK;
}

int T(){
    int OK = 1;
    OK = F();
    if(!OK) return OK;
    OK = FS();
    return OK;
}

int TS(){
    int OK = 1;
    move_i();
    if(!strcmp(tk_p[i]->str, "+") || !strcmp(tk_p[i]->str, "-")){
        printf("%s[452]",tk_p[i]->str);//test
        OK = T();
        if(!OK) return OK;
        OK = TS();
    }
    else back_i();
    return OK;
}

int E(){
    int OK = 1;
    OK = T();
    if(!OK) return OK;
    OK = TS();
    return OK;
}

int S(){
    int OK = 1;
    if(tk_p[i]->d_kind == d_skip){
        printf("\n%s[476]",tk_p[i]->str);//test
    }
    else if(tk_p[i]->d_kind == d_if){
        printf("\n%s[480]",tk_p[i]->str);//test
        OK = got_if();
    }
    else if(tk_p[i]->d_kind == d_read){
        printf("\n%s[484]",tk_p[i]->str);//test
        OK = got_read();
    }
    else if(tk_p[i]->d_kind == d_write){
        printf("\n%s[488]",tk_p[i]->str);//test
        OK = got_write();
    }
    else if(tk_p[i]->d_kind == d_while){
        printf("\n%s[492]",tk_p[i]->str);//test
        OK = got_while();
    }
    else if(tk_p[i]->kind == TK_ID){
        OK = got_assign();
    }
    else{
        err_loc_print();
        printf(" syntax error with '%s'[500]", tk_p[i]->str);
        OK = 0;
    }
    return OK;
}

int L(){
    int OK = 1;
    move_i();
    if(!strcmp(tk_p[i]->str, "skip") || tk_p[i]->kind == TK_ID || !strcmp(tk_p[i]->str, "while") || !strcmp(tk_p[i]->str, "write") || !strcmp(tk_p[i]->str, "read") || !strcmp(tk_p[i]->str, "skip") || !strcmp(tk_p[i]->str, "if")){
        OK = S();
        if(!OK) return OK;
        move_i();
        if(!strcmp(tk_p[i]->str, ";")){
            printf("%s[518]",tk_p[i]->str);//test
            OK = L();
        }
        else back_i();
    }
    else{
        err_loc_print();
        printf(" ';' is redundant");//test
        OK = 0;
    }
    return OK;
}

int X(){
    int OK = 1;
    do{
        move_i();
        if(tk_p[i]->kind == TK_ID)
        ID_bag = (char** )realloc(ID_bag, (bag_len + 1)*sizeof(char*));
        ID_place = (int* )realloc(ID_place, (bag_len + 1)*sizeof(int));
        ID_bag[bag_len] = (char* )malloc(sizeof(char)*MAX_LEN);
        strcpy(ID_bag[bag_len], tk_p[i]->str);
        printf(" %s ", ID_bag[bag_len]); //test
        ++bag_len;
        move_i();
    }while(!strcmp(tk_p[i]->str, ","));
    return OK;
}

int P(){
    int OK = 1;
    //test
    // int k;
    // for(k = 0; k<len; ++k){
    //     if(!(k%5)) printf("\n");
    //     printf(" %-3d:  %-10s ", k, tk_p[k]->str);
    // }
    //tset
    if(tk_p[i]->d_kind == d_var){
        printf("%s[557]",tk_p[i]->str);//test
        OK = X();
        if(strcmp(tk_p[i]->str, ";")) {
            err_loc_print();
            printf(" Expect a ';' here[562]");//test
            OK = 0;
        }
        else{
            printf("%s[566]",tk_p[i]->str);//test
            OK = L();
        }
    } 
    else 
        OK = L();
    return OK;
}

int main(){
    int OK = 1;
    printf("プログラムスタート\n");
    FILE *fp = fopen("whi-test1.txt", "r"); 
    if(!(fp == NULL)){
        tk_p = (Token** )malloc((len+1)*sizeof(Token*));
        do{
            tk_p[len] = get_token(fp);
            if(tk_p[len]->kind == TK_ERR){
                printf("输入有误！错误位置：%d, %d\n", row, col);
                break;
            }
            else{
                //printf("\n%s[551]",tk_p[i]->str);//test
            } 
            ++len;
            tk_p = (Token** )realloc(tk_p, (len+1)*sizeof(Token*));   
        }while(tk_p[len-1]->kind != TK_EOF);
        fclose(fp);
        OK = P();
        move_i();
        if(OK){
            if(tk_p[i]->kind == TK_EOF)
                printf("\nもう終わりだ、おめでとう");
            else{
                printf("\n[%d][%s]もう少し～",  i, tk_p[i]->str);//test
            }
        }
        else printf("\n失敗するね~");
        free(tk_p);
    }
    else printf("Sorry, something wrong with your file path");
    return 0;
}