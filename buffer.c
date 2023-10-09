#include "buffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int line = 1;
static int nchars = 0;
static int pstr = 0;	//����������ָ��
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

	// ����ʱ�������з�����δ�����뻺����
	while (!feof(fp) && strlen(pbuf) + 1 != half_size) {
		int a = strlen(pbuf);
		// �ظ���ȡ��ֱ����������
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
	
	//��һ�γ�ʼ��ֻ���ʼһ�룬�Ұ����get_char�г�ʼ��
	memset(buf->buf, 0, buf->size);
	fill_buf(buf, true);

	buf->forward = 0;	// forwardָ��
	buf->p = 0;			// frontָ��
}

// �ƶ�forwardָ�룬��ȡbuf����һ���ַ�����Ҫʱ��仺����
char get_char(buffer* buf)
{
	char ch = buf->buf[buf->forward++];

	//��Ҫ����µĻ�����
	if (ch == EOF) {
		// ����һ�����
		if (buf->p < buf->size / 2 - 1) {
			fill_buf(buf, false);
			buf->forward++;
		}
		// ����󻺳���
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
	//��chԭ����Ϊ�գ���ֱ���˳�
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
	// λ���󻺳�����ͷ����䵽�һ�����ĩβ
	if (buf->forward == 0) {
		buf->forward = buf->size - 2;
	}
	// λ���һ�������ͷ����䵽�󻺳���ĩβ
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

	// û�в�������
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