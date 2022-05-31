#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<math.h>
#define MAX_LEN (20) //单词长度限制
#define dummy_len (10) //关键字每个长度
#define dummy_num (11) //关键字个数
int col = 0, row = 1; //全局变量实时关注文件指针的位置
int len = 0, i = 0;
char** ID_bag;
int* ID_place;
int bag_len = 0;
int stc_len = 0;
int OK = 1;
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

typedef struct{
    char kind[4];
    int val;
}Stc;
Stc** stc_p;

typedef struct Link{
    int val;
    struct Link* next;
}Link;

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

void alloc(char* kind, int val){//尝试一次分配多个语句空间
    stc_p = (Stc** )realloc(stc_p, (stc_len + 1)*sizeof(Stc* ));
    stc_p[stc_len] = (Stc* )malloc(sizeof(Stc));
    strcpy(stc_p[stc_len]->kind, kind);
    stc_p[stc_len]->val = val;
    stc_len++;
}

int lookup(){
    int k;
    for(k = 0; k < bag_len; ++k){
        if(!strcmp(tk_p[i]->str, ID_bag[k])) 
            break;
    }
    if(k == bag_len)
        return -1;
    else
        return k;
}

int check_dual(){
    int k;
    for(k = 0; k < bag_len; ++k){
        if(!strcmp(tk_p[i]->str, ID_bag[k])) 
            break;
    }
    if(k == bag_len)
        return 1;
    else
        return 0;
}

void P();
void X();
Link* L();
void E();
Link* S();
Link* merge(Link* l1, Link* l2){
    Link* temp = l1;
    if(temp->val != -1){
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next= l2;
    }
    else return l2;
    return l1;
}

void init_link(Link* l){
    l->next = NULL;
    l->val = -1;
    return ;
}

void patch(Link* l, int loc){
    Link* temp = l;
    if(temp->val != -1){
        while(temp->next != NULL){
            stc_p[temp->val]->val = loc;
            temp = temp->next;
        }
        if(temp->val != -1) stc_p[temp->val]->val = loc;
    }
    return ;
}

void Link_print(Link* l){
    Link* temp = l;
    while(temp->next != NULL){
        printf("\n%d",temp->val);
        temp = temp->next;
    }
    return ;
}

Link* got_if(){
    Link* lst_empt = (Link*)malloc(sizeof(Link));
    init_link(lst_empt);
    E();
    if(!(OK)) 
        return lst_empt;
    alloc("jpc", -1);
    Link* lst = (Link*)malloc(sizeof(Link));
    Link* la = (Link*)malloc(sizeof(Link));
    Link* lst1 = (Link*)malloc(sizeof(Link));
    Link* lst2 = (Link*)malloc(sizeof(Link));
    init_link(lst);
    init_link(la);
    init_link(lst1);
    init_link(lst2);
    la->val = stc_len-1;
    //printf("%s %d",stc_p[la->val]->kind, stc_p[la->val]->val);
    move_i();
    if(!strcmp(tk_p[i]->str, "then")){
        //printf("\n%s[210]",tk_p[i]->str);//test
        lst1 = L();
        if(!OK) return lst_empt;
        alloc("jmp", -1);
        Link* lb = (Link*)malloc(sizeof(Link));
        lb->next = NULL;
        lb->val = stc_len-1;
        move_i();
        if(!strcmp(tk_p[i]->str, "else")){
            //printf("\n%s[214]",tk_p[i]->str);//test
            patch(la, stc_len);
            lst2 = L();
            if(!OK) return lst_empt;
            move_i();
            if(strcmp(tk_p[i]->str, "fi")){
                OK = 0;
                err_loc_print();
                printf(" expect a 'fi' here[%s][221]",tk_p[i]->str);
                return lst_empt;
            }
            else{
                //printf("\n%s[224]",tk_p[i]->str);//test
                lst = merge(merge(lst1, lb), lst2);
                //Link_print(lst);//Test
                return lst;
            }
        }
        else{
            OK = 0;
            err_loc_print();
            printf(" expect a 'else' here[230]");
            return lst_empt;
        }
    }
    else{
        OK = 0;
        err_loc_print();
        printf(" expect a 'then' here[236]");
        return lst_empt;
    } 
}

Link* got_while(){
    int loc;
    Link* lst = (Link*)malloc(sizeof(Link));
    Link* lst_empt = (Link*)malloc(sizeof(Link));
    init_link(lst);
    init_link(lst_empt);
    loc = stc_len;
    E();
    if(!OK) return lst_empt;
    alloc("jpc", -1);
    Link* la = (Link*)malloc(sizeof(Link));
    la->next = NULL;
    la->val = stc_len-1;
    move_i();
    if(!strcmp(tk_p[i]->str, "do")){
        //printf("\n%s[246]",tk_p[i]->str);//test
        lst = L();
        alloc("jmp", loc);
        patch(lst, loc);
        move_i();
        if(strcmp(tk_p[i]->str, "od")){
            OK = 0;
            err_loc_print();
            printf(" expect a 'od' here[%s][252]", tk_p[i]->str);
            return lst_empt;
        }
        else{
            //printf("\n%s[255]",tk_p[i]->str);//test
        }
    }
    else{
        OK = 0;
        err_loc_print();
        printf(" expect a 'do' here[261]");
        return lst_empt;
    } 
    return la;
}

void got_read(){
    int k, a;
    move_i();
    if(!strcmp(tk_p[i]->str, "(")){
        //printf("%s[270]",tk_p[i]->str);//test
        move_i();
        if((a = lookup()) == -1){
            OK = 0;
            err_loc_print();
            printf(" %s wasn't identified[278]", tk_p[i]->str);
        } 
        else{
            //printf("%s[281]",tk_p[i]->str);//test scanf("%d", &ID_place[k]);   
            alloc("red", a);         
            move_i();
            if(strcmp(tk_p[i]->str, ")")){
                OK = 0;
                err_loc_print();
                printf(" expect a ')' here[286]");
            }
            else{
                //printf("%s[289]",tk_p[i]->str);//test
            }
        }
    }
    else{
        OK = 0;
        err_loc_print();
        printf(" expect a '(' here[296]");
    }
    return ;
}

void got_write(){ 
    int k;
    move_i();
    if(!strcmp(tk_p[i]->str, "(")){
        //printf("%s[307]",tk_p[i]->str);//test
        E();
        alloc("wrt", -1);
        if(!OK) return ;
        move_i();
        if(!strcmp(tk_p[i]->str, ")")){
            //printf("%s[308]",tk_p[i]->str);//test
        }
        else{
            OK = 0;
            err_loc_print();
            printf(" expect a ')' here[313]");
        }
    }
    else {
        OK = 0;
        err_loc_print();
        printf(" expect a '(' here[319]");
    }
    return ;
}

void got_assign(){
    int a;
    if( (a = lookup()) == -1){
        OK = 0;
        err_loc_print();
        printf(" %s wasn't identified here[332]", tk_p[i]->str);
    }
    else{
        //printf("\n%s[335]",tk_p[i]->str);//test
        move_i();
        if(!strcmp(tk_p[i]->str, ":=")){
            //printf("%s[338]",tk_p[i]->str);//test
            E();
            alloc("sto", a);
            if(!OK) return ;
        }
        else{
            OK = 0;
            err_loc_print();
            printf(" Expect a ':=' here[345]");
        }
    }
    return ;
}

void D(){
    int a = 0;
    move_i();
    if(tk_p[i]->kind == TK_ID || tk_p[i]->kind == TK_NUM){
        if(tk_p[i]->kind == TK_ID){
            if((a = lookup()) == -1){
                OK = 0;
                err_loc_print();
                printf(" %s wasn't identified[364]", tk_p[i]->str);
            }    
            else{
                alloc("lod", a);
                //printf("%s[367]", tk_p[i]->str); //test   
            }
        }
        else if(tk_p[i]->kind == TK_NUM){
            char *s;
            int j, len = strlen(tk_p[i]->str);
            s = (char*)malloc(len*sizeof(char)); 
            strcpy(s, tk_p[i]->str);
            for(j = 0; j<len; ++j){
                a = a + (s[j]-'0')*pow(10, (len-j-1));
            }
            alloc("lit", a);
            //printf("%s[371]", tk_p[i]->str);//test
        }
    }
    else if(!strcmp(tk_p[i]->str, "(")){
        //printf("%s[375]",tk_p[i]->str);//test
        E();
        if(!OK) return ;
        move_i();
        if(!strcmp(tk_p[i]->str, ")")){
            //printf("%s[380]",tk_p[i]->str);//test
        }
        else{
            OK = 0;
            err_loc_print();
            printf(" Expect a ')' here[385]");
        }
    }
    else if(!strcmp(tk_p[i]->str, "~")){
        //printf("\n%s[389]",tk_p[i]->str);//test
        E();
        alloc("lit", 0);
        alloc("equ", -1);
    }
    else{
        OK = 0;
        err_loc_print();
        printf(" expect 'V', 'N', '~' or '(' here[395]");
    }
    return ;
}

void D1(){
    move_i();
    if(!strcmp(tk_p[i]->str, "=")){
        //printf("%s[404]",tk_p[i]->str);//test
        D();
        alloc("equ", -1);
    }
    else if(!strcmp(tk_p[i]->str, "<")){
        //printf("%s[404]",tk_p[i]->str);//test
        D();
        alloc("lth", -1);
    }
    else back_i();
    return ;
}

void F(){
    D();
    if(!OK) return ;
    D1();
    return ;
}

void FS(){
    move_i();
    if(!strcmp(tk_p[i]->str, "*")){
        //printf("\n%s[427]",tk_p[i]->str);//test
        F();
        alloc("mul", -1);
        if(!OK) return ;
        FS();
    }
    else if(!strcmp(tk_p[i]->str, "/")){
        //printf("\n%s[427]",tk_p[i]->str);//test
        F();
        alloc("div", -1);
        if(!OK) return ;
        FS();
    }
    else back_i();
    return ;
}

void T(){
    F();
    if(!OK) return ;
    FS();
    return ;
}

void TS(){
    move_i();
    if(!strcmp(tk_p[i]->str, "+")) {
        //printf("%s[452]",tk_p[i]->str);//test
        T();
        alloc("add", -1);
        if(!OK) return ;
        TS();
    }
    else if(!strcmp(tk_p[i]->str, "-")){
        //printf("%s[452]",tk_p[i]->str);//test
        T();
        alloc("sub", -1);
        if(!OK) return ;
        TS();
    }
    else back_i();
    return ;
}

void E(){
    T();
    if(!OK) return ;
    TS();
    return ;
}

Link* S(){
    Link* lst = (Link*)malloc(sizeof(Link));
    Link* lst_empty = (Link*)malloc(sizeof(Link));
    init_link(lst);
    init_link(lst_empty);
    if(tk_p[i]->d_kind == d_skip){
        //printf("\n%s[476]",tk_p[i]->str);//test
    }
    else if(tk_p[i]->d_kind == d_if){
        //printf("\n%s[480]",tk_p[i]->str);//test
        lst = got_if();
        return lst;
    }
    else if(tk_p[i]->d_kind == d_read){
        //printf("\n%s[484]",tk_p[i]->str);//test
        got_read();
    }
    else if(tk_p[i]->d_kind == d_write){
        //printf("\n%s[488]",tk_p[i]->str);//test
        got_write();
    }
    else if(tk_p[i]->d_kind == d_while){
        //printf("\n%s[492]",tk_p[i]->str);//test
        lst = got_while();
        return lst;
    }
    else if(tk_p[i]->kind == TK_ID){
        got_assign();
    }
    else{
        err_loc_print();
        printf(" syntax error with '%s'[500]", tk_p[i]->str);
        OK = 0;
    }
    return lst_empty;
}

Link* L(){
    //改一改
    Link* lst = (Link*)malloc(sizeof(Link));
    Link* lst0 = (Link*)malloc(sizeof(Link));
    Link* lst_empt = (Link*)malloc(sizeof(Link));
    init_link(lst);
    init_link(lst0);
    init_link(lst_empt);
    move_i();
    if(!strcmp(tk_p[i]->str, "skip") || tk_p[i]->kind == TK_ID || 
    !strcmp(tk_p[i]->str, "while") || !strcmp(tk_p[i]->str, "write") || 
    !strcmp(tk_p[i]->str, "read") || !strcmp(tk_p[i]->str, "skip") || !strcmp(tk_p[i]->str, "if")){
        lst0 = S();
        if(!OK) return lst_empt;
        patch(lst0, stc_len);
        move_i();
        if(!strcmp(tk_p[i]->str, ";")){
            //printf("%s[518]",tk_p[i]->str);//test
            lst = L();
            return lst;
        }
        else back_i();
    }
    else{
        err_loc_print();
        printf(" ';' is redundant");
        OK = 0;
        return lst_empt;
    }
    return lst0;
}

void X(){
    do{
        move_i();
        if(tk_p[i]->kind == TK_ID){
            if(bag_len && !check_dual()){
                OK = 0;
                err_loc_print();
                printf(" '%s' already been defined", tk_p[i]->str);
                break;
            }
            ID_bag = (char** )realloc(ID_bag, (bag_len + 1)*sizeof(char*));
            ID_place = (int* )realloc(ID_place, (bag_len + 1)*sizeof(int));
            ID_bag[bag_len] = (char* )malloc(sizeof(char)*MAX_LEN);
            strcpy(ID_bag[bag_len], tk_p[i]->str);
            //printf(" %s ", ID_bag[bag_len]); //test
            ++bag_len;
            move_i();
        }
        else if(tk_p[i]->kind == TK_DUMMY){
            OK = 0;
            err_loc_print();
            printf(" '%s' is a dummy", tk_p[i]->str);
            break;
        }
        else{//修改2022.5.30
            OK = 0;
            err_loc_print();
            printf(" something error", tk_p[i]->str);
            break;
        }
    }while(!strcmp(tk_p[i]->str, ","));
    return ;
}

void P(){
    int k;
    Link* lst = (Link*)malloc(sizeof(Link));
    init_link(lst);
    if(tk_p[i]->d_kind == d_var){
        //printf("\n%s[557]",tk_p[i]->str);//test
        X();
        if(!OK) return ;
        if(strcmp(tk_p[i]->str, ";")) {
            err_loc_print();
            printf(" Expect a ';' here[562]");
            OK = 0;
        }
        else{
            alloc("int", bag_len);
            for(k = 0; k<bag_len; ++k){
                alloc("lit", 0);
                alloc("sto", k);
            }
            //printf("%s[566]",tk_p[i]->str);//test
            lst = L();
            patch(lst, stc_len);
            alloc("nop", -1);
        }
    } 
    else{
        alloc("int", 0);
        lst = L();
        patch(lst, stc_len);
        alloc("nop", -1);
    }
    return ;
}

int main(){
    int k;
    printf("プログラムスタート\n");
    FILE *fp = fopen("test.txt", "r"); 
    if(!(fp == NULL)){
        tk_p = (Token** )malloc((len+1)*sizeof(Token*));
        do{
            tk_p[len] = get_token(fp);
            if(tk_p[len]->kind == TK_ERR){
                printf("输入有误！错误位置：%d, %d\n", row, col);
                break;
            }
            ++len;
            tk_p = (Token** )realloc(tk_p, (len+1)*sizeof(Token*));   
        }while(tk_p[len-1]->kind != TK_EOF);
        fclose(fp);
        P();
        move_i();
        if(OK){
            if(tk_p[i]->kind == TK_EOF){
                for(k = 0; k<stc_len; ++k){
                    if(stc_p[k]->val != -1)
                        printf("\n%s %d", stc_p[k]->kind, stc_p[k]->val);
                    else
                        printf("\n%s", stc_p[k]);
                }
                printf("\n\nもう終わりだ、おめでとう");
            }
            else
                printf("\n[%d][%s]もう少し～",  i, tk_p[i]->str);
        }
        else printf("\n失敗するね~");
        free(tk_p);
    }
    else printf("Sorry, something wrong with your file path");
    return 0;
}