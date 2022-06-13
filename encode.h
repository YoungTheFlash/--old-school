/**
 * @file encode.h
 * @author Young
 * @brief 
 * @version 0.1
 * @date 2022-05-31
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef encode
#define encode
typedef struct{
    char kind[4];
    int val;
}Stc;

typedef struct Link{
    int val;
    struct Link* next;
}Link;

void test_result();
void err_loc_print();
int return_OK();
void P();

#endif