#ifndef _VALUE_H_
#define _VALUE_H_
#define  _CRT_SECURE_NO_WARNINGS

typedef enum bool_type { false, true } Boolean;

#define BUF_SIZE	64
#define TOKEN_SIZE	32

#define ID 0        //标识符
#define TYPE_NUM 1       //实数
#define TYPE_INT 2       //int
#define TYPE_FLOAT 3     //f
#define KEY_WORD 4  //关键字
#define OPERATOR 5  //运算符
#define TYPE_STRING 6    
#define TYPE_CHAR 7      

#define SEPERATOR 14    //分隔符


#endif

