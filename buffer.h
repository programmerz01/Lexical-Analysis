#ifndef _BUFFER_H_
#define _BUFFER_H_
#include "value.h"

typedef struct struct_buffer buffer;

struct struct_buffer {
	char* buf;
	int size;
	int p;	//指针位置，指向当前字符末尾
	int forward; // 指针，指向当前正在读取记号的末尾
};

/*
	词法分析中使用的缓冲区函数
*/

//字符数组的申请与释放
void b_init(buffer* buf, const char* str);

// 移动forward指针，获取buf中下一个字符，必要时填充缓冲区
char get_char(buffer *buf);

// 跳过空格、制表符、换行符，获取第一个非空字符
char get_nbc(buffer *buf, char ch);

// forward向buf退回一个字符
char retract(buffer *buf);

// 将ch拼接在token后
void cat(buffer *token, char ch);

//将字符串缓冲区清空，便于下次输入
void clear_buf(buffer *buf);

//获取当前行数
int get_line();

//获取当前行数
int get_chars();

#endif // !_BUFFER_H_

