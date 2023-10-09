#include "buffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int line = 0;
static int nchars = 0;
static int pstr = 0;	//待分析程序指针

char* b_init(buffer *buf, const char* str)
{
	memset(buf->buf, 0, buf->size);

	buf->buf[buf->size / 2 - 1] = EOF;
	buf->buf[buf->size - 1] = EOF;
	//第一次初始化只需初始一半，右半会在get_char中初始化
	memcpy(buf->buf, str + pstr, buf->size / 2 - 1);
	pstr += buf->size / 2 - 1;


	buf->forward = 0;	// forward指针
	buf->p = 0;			// front指针
	
}


// 移动forward指针，获取buf中下一个字符，必要时填充缓冲区
char get_char(const char* prog_str, buffer *buf)
{
	char ch = buf->buf[buf->forward++];

	//需要填充新的缓冲区
	if (ch == EOF) {
		// 填充右缓冲区
		if (buf->p < buf->size / 2 - 1) {
			memcpy(buf->buf + buf->size / 2, prog_str + pstr, buf->size - 2 - (buf->size / 2 - 1));
			pstr += buf->size - 2 - (buf->size / 2 - 1);
			buf->forward;
		}
		// 填充左缓冲区
		else {
			memcpy(buf->buf, prog_str + pstr, buf->size / 2 - 1);
			pstr += buf->size / 2 - 1;
			buf->forward = 0;
		}
		ch = buf->buf[buf->forward++]; 
		if (ch == '\n') {
			line++;
		}
	}
	else if (ch == '\n') {
		line++;
	}
	if (ch != 0) {
		nchars++;
	}
	

	return ch;
}

char get_nbc(const char* prog_str, buffer *buf, char ch)
{
	//若ch原本不为空，则直接退出
	if (!(ch == ' ' || ch == '\n' || ch == '\t')) {
		return ch;
	}
	//else
	ch = get_char(prog_str, buf);

	while (ch == ' ' || ch == '\n' || ch == '\t')
	{
		ch = get_char(prog_str, buf);
	}

	return ch;
}

char retract(buffer* buf)
{
	// 位于左缓冲区开头，填充到右缓冲区末尾
	if (buf->forward == 0) {
		buf->forward = buf->size - 2;
	}
	// 位于右缓冲区开头，填充到左缓冲区末尾
	else if (buf->size == buf->size / 2) {
		buf->forward = buf->size / 2 - 2;
	}
	else {
		buf->forward--;
	}
	return buf->buf[buf->forward];
}

void cat(buffer *token, char ch)
{
	if (token->p == token->size - 1)
	{
		printf("Too long string or identifier\n");
		exit(1);
	}
	token->buf[token->p++] = ch;
}

void clear_buf(buffer *buf)
{
	memset(buf->buf, 0, buf->size);
	buf->p = 0;
}

int get_line()
{
	return line;
}

int get_chars()
{
	return nchars;
}
