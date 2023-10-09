#include "buffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int line = 1;
static int nchars = 0;
static int pstr = 0;	//待分析程序指针
static FILE* fp = 0;

static Boolean fill_buf(buffer* buf, Boolean fill_left);

static Boolean fill_buf(buffer* buf, Boolean fill_left)
{
	int half_size;
	char* pbuf;

	if (fill_left) {
		half_size = buf->size / 2;
		pbuf = buf->buf;
	}
	else {
		half_size = buf->size - buf->size / 2 ;
		pbuf = buf->buf + buf->size / 2;
	}

	fgets(pbuf, half_size, fp);

	// 读入时遇到换行符导致未读满半缓冲区
	while (!feof(fp) && strlen(pbuf) + 1 != half_size) {
		int a = strlen(pbuf);
		// 重复读取，直至缓冲区满
		fgets(pbuf + strlen(pbuf), half_size - strlen(pbuf), fp);
	}

	pbuf[half_size - 1] = EOF;

}

void b_init(buffer* buf,const char* file_name)
{
	if (!(fp = fopen(file_name, "r"))) {
		printf("Can't find or open file: %s",file_name);
		exit(1);
	}
	
	//第一次初始化只需初始一半，右半会在get_char中初始化
	memset(buf->buf, 0, buf->size);
	fill_buf(buf, true);

	buf->forward = 0;	// forward指针
	buf->p = 0;			// front指针
}

// 移动forward指针，获取buf中下一个字符，必要时填充缓冲区
char get_char(buffer* buf)
{
	char ch = buf->buf[buf->forward++];

	//需要填充新的缓冲区
	if (ch == EOF) {
		// 填充右缓冲区
		if (buf->p < buf->size / 2 - 1) {
			fill_buf(buf, false);
			buf->forward++;
		}
		// 填充左缓冲区
		else {
			fill_buf(buf, true);
			buf->forward = 0;
		}
		ch = buf->buf[buf->forward++];
	}

	line += (ch == '\n');
	nchars += (ch != 0);

	return ch;
}


char get_nbc(buffer *buf, char ch)
{
	//若ch原本不为空，则直接退出
	if (!(ch == ' ' || ch == '\n' || ch == '\t')) {
		return ch;
	}
	//else
	ch = get_char(buf);

	while (ch == ' ' || ch == '\n' || ch == '\t')
	{
		ch = get_char(buf);
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

int get_line() { return line; }
int get_chars() { return nchars; }

int main(int argc, char** args) {

	buffer buf = { NULL,BUF_SIZE,0,0 };
	buffer token = { NULL,TOKEN_SIZE,0,0 };
	char file_name[64] = "";

	buf.buf = (char*)malloc(buf.size);
	token.buf = (char*)malloc(token.size);

	// 没有参数传入
	if (argc < 2) {
		printf("Input file name:");
		scanf("%s", file_name);
	}
	else {
		strcpy(file_name, args[1]);
	}


	b_init(&buf, file_name);

	char ch = get_char(&buf);
	while (ch != 0) 
	{
		putchar(ch);
		ch = get_char(&buf);
		buf.p = buf.forward;
	}

	printf("\nline: %d\nchars: %d", line, nchars);

	return 0;
}