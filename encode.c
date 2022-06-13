/**
 * @file encode.c
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
#include<math.h>

#include "encode.h"
#include "word.h"

#define MAX_LEN (20)

int bag_len = 0;
int stc_len = 0;
int OK = 1;
int i = 0;
char** ID_bag;
Stc** stc_p;

void X();
Link* L();
void E();
Link* S();

void move_i(){
    if(i == return_len()) 
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

Link* merge(Link* l1, Link* l2){
    Link* temp = l1;
    if(temp->val != -1){
        while(temp->next != NULL){
            temp = temp->next;
        }
        if(l2->val != -1)
            temp->next= l2;
    }
    else return l2;
    return l1;
}

void patch(Link* l, int loc){
    Link* temp = l;
    if(temp->val != -1){
        while(temp->next != NULL){
            stc_p[temp->val]->val = loc;
            temp = temp->next;
        }
        stc_p[temp->val]->val = loc;
    }
    return ;
}

Link* link_init(){
    Link* lst = (Link*)malloc(sizeof(Link));
    lst->next = NULL;
    lst->val = -1;
    return lst;
}

Link* got_if(){
    Link* lst_empt;
    lst_empt = link_init();
    E();
    if(!(OK)) 
        return lst_empt;
    alloc("jpc", -1);
    Link *lst, *la, *lst1, *lst2;
    lst = link_init();
    la = link_init();
    lst1 = link_init();
    lst2 = link_init();
    la->val = stc_len-1;
    move_i();
    if(!strcmp(tk_p[i]->str, "then")){
        printf("\n%s ",tk_p[i]->str);//test
        lst1 = L();
        if(!OK) return lst_empt;
        alloc("jmp", -1);
        Link* lb;
        lb = link_init();
        lb->val = stc_len-1;
        move_i();
        if(!strcmp(tk_p[i]->str, "else")){
            printf("\n%s ",tk_p[i]->str);//test
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
                printf("\n%s ",tk_p[i]->str);//test
                lst = merge(merge(lst1, lb), lst2);
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
    Link* lst, *lst_empt;
    lst = link_init();
    lst_empt = link_init();
    loc = stc_len;
    E();
    if(!OK) return lst_empt;
    alloc("jpc", -1);
    Link* la;
    la = link_init();
    la->val = stc_len-1;
    move_i();
    if(!strcmp(tk_p[i]->str, "do")){
        printf("\n%s ",tk_p[i]->str);//test
        lst = L();
        if(!OK) return lst_empt;
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
            printf("\n%s ",tk_p[i]->str);//test
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
        printf("%s ",tk_p[i]->str);//test
        move_i();
        if((a = lookup()) == -1){
            OK = 0;
            err_loc_print();
            printf(" %s wasn't identified[278]", tk_p[i]->str);
        } 
        else{
            printf("%s ",tk_p[i]->str);//test scanf("%d", &ID_place[k]);   
            alloc("red", a);         
            move_i();
            if(strcmp(tk_p[i]->str, ")")){
                OK = 0;
                err_loc_print();
                printf(" expect a ')' here[286]");
            }
            else{
                printf("%s ",tk_p[i]->str);//test
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
        printf("%s ",tk_p[i]->str);//test
        E();
        alloc("wrt", -1);
        if(!OK) return ;
        move_i();
        if(!strcmp(tk_p[i]->str, ")")){
            printf("%s ",tk_p[i]->str);//test
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
        printf(" '%s' wasn't identified[332]", tk_p[i]->str);
    }
    else{
        printf("\n%s ",tk_p[i]->str);//test
        move_i();
        if(!strcmp(tk_p[i]->str, ":=")){
            printf("%s ",tk_p[i]->str);//test
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
                printf("%s ", tk_p[i]->str); //test   
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
            printf("%s ", tk_p[i]->str);//test
        }
    }
    else if(!strcmp(tk_p[i]->str, "(")){
        printf("%s ",tk_p[i]->str);//test
        E();
        if(!OK) return ;
        move_i();
        if(!strcmp(tk_p[i]->str, ")")){
            printf("%s ",tk_p[i]->str);//test
        }
        else{
            OK = 0;
            err_loc_print();
            printf(" Expect a ')' here[385]");
        }
    }
    else if(!strcmp(tk_p[i]->str, "~")){
        printf("\n%s ",tk_p[i]->str);//test
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
        printf("%s ",tk_p[i]->str);//test
        D();
        alloc("equ", -1);
    }
    else if(!strcmp(tk_p[i]->str, "<")){
        printf("%s ",tk_p[i]->str);//test
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
        printf(" %s ",tk_p[i]->str);//test
        F();
        alloc("mul", -1);
        if(!OK) return ;
        FS();
    }
    else if(!strcmp(tk_p[i]->str, "/")){
        printf(" %s ",tk_p[i]->str);//test
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
        printf("%s ",tk_p[i]->str);//test
        T();
        alloc("add", -1);
        if(!OK) return ;
        TS();
    }
    else if(!strcmp(tk_p[i]->str, "-")){
        printf("%s ",tk_p[i]->str);//test
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
    Link* lst, *lst_empty;
    lst = link_init();
    lst_empty = link_init();
    if(tk_p[i]->d_kind == d_skip){
        printf("\n%s ",tk_p[i]->str);//test
    }
    else if(tk_p[i]->d_kind == d_if){
        printf("\n%s ",tk_p[i]->str);//test
        lst = got_if();
        return lst;
    }
    else if(tk_p[i]->d_kind == d_read){
        printf("\n%s ",tk_p[i]->str);//test
        got_read();
    }
    else if(tk_p[i]->d_kind == d_write){
        printf("\n%s ",tk_p[i]->str);//test
        got_write();
    }
    else if(tk_p[i]->d_kind == d_while){
        printf("\n%s ",tk_p[i]->str);//test
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
    Link* lst, *lst0, *lst_empt;
    lst = link_init();
    lst0 = link_init();
    lst_empt = link_init();
    move_i();
    if(!strcmp(tk_p[i]->str, "skip") || tk_p[i]->kind == TK_ID || 
    !strcmp(tk_p[i]->str, "while") || !strcmp(tk_p[i]->str, "write") || 
    !strcmp(tk_p[i]->str, "read") || !strcmp(tk_p[i]->str, "skip") || !strcmp(tk_p[i]->str, "if")){
        lst0 = S();
        if(!OK) return lst_empt;
        patch(lst0, stc_len);
        move_i();
        if(!strcmp(tk_p[i]->str, ";")){
            printf("%s ",tk_p[i]->str);//test
            lst = L();
            return lst;
        }
        else back_i();
    }
    else{
        err_loc_print();
        printf("  no legal word here");
        OK = 0;
        return lst_empt;
    }
    return lst0;
}

void X(){
    do{
        if(strcmp(tk_p[i]->str,"var") != 0) 
            printf("%s",tk_p[i]->str);
        move_i();
        if(tk_p[i]->kind == TK_ID){
            if(bag_len && !check_dual()){
                OK = 0;
                err_loc_print();
                printf(" '%s' already been defined", tk_p[i]->str);
                break;
            }
            ID_bag = (char** )realloc(ID_bag, (bag_len + 1)*sizeof(char*));
            ID_bag[bag_len] = (char* )malloc(sizeof(char)*MAX_LEN);
            strcpy(ID_bag[bag_len], tk_p[i]->str);
            ++bag_len;
            printf("%s",tk_p[i]->str);
            move_i();
        }
        else if(tk_p[i]->kind == TK_DUMMY){
            OK = 0;
            err_loc_print();
            printf(" '%s' is a dummy", tk_p[i]->str);
            break;
        }
        else{
            OK = 0;
            err_loc_print();
            printf(" '%s' is not a legal ID", tk_p[i]->str);
            break;
        }
    }while(!strcmp(tk_p[i]->str, ","));
    return ;
}

void P(){
    int k;
    Link* lst;
    lst = link_init();
    if(tk_p[i]->d_kind == d_var){
        printf("\n%s ",tk_p[i]->str);//test
        X();
        if(!OK) return ;
        if(strcmp(tk_p[i]->str, ";") != 0) {
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
            printf("%s ",tk_p[i]->str);//test
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

void test_result(){
    int k;
    move_i();
    if(tk_p[i]->kind == TK_EOF){
        for(k = 0; k<stc_len; ++k){
            if(stc_p[k]->val != -1)
                printf("\n%s %d", stc_p[k]->kind, stc_p[k]->val);
            else
                printf("\n%s", stc_p[k]);
        }
        printf("\n\nおめでとう！");
    }
    else
        printf("\n[%d][%s]もう少し～",  i, tk_p[i]->str);
    return ;
}

int return_OK(){
    return OK;
}