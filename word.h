/**
 * @file word.h
 * @author Young
 * @brief 
 * @version 0.1
 * @date 2022-05-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef word
#define word
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

typedef struct{
    enum tok_kind kind;
    char str[20];
    enum dummy_kind d_kind;
    int n_row;
    int n_col;
    enum lex_err l_err;
}Token;

Token** tk_p;

void read_word(FILE* );
int return_len();

#endif