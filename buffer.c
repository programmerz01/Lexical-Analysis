#include "buffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int line = 0;
static int nchars = 0;
static int pstr = 0;	//����������ָ��

char* b_init(buffer *buf, const char* str)
{
	memset(buf->buf, 0, buf->size);

	buf->buf[buf->size / 2 - 1] = EOF;
	buf->buf[buf->size - 1] = EOF;
	//��һ�γ�ʼ��ֻ���ʼһ�룬�Ұ����get_char�г�ʼ��
	memcpy(buf->buf, str + pstr, buf->size / 2 - 1);
	pstr += buf->size / 2 - 1;


	buf->forward = 0;	// forwardָ��
	buf->p = 0;			// frontָ��
	
}


// �ƶ�forwardָ�룬��ȡbuf����һ���ַ�����Ҫʱ��仺����
char get_char(const char* prog_str, buffer *buf)
{
	char ch = buf->buf[buf->forward++];

	//��Ҫ����µĻ�����
	if (ch == EOF) {
		// ����һ�����
		if (buf->p < buf->size / 2 - 1) {
			memcpy(buf->buf + buf->size / 2, prog_str + pstr, buf->size - 2 - (buf->size / 2 - 1));
			pstr += buf->size - 2 - (buf->size / 2 - 1);
			buf->forward;
		}
		// ����󻺳���
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
	//��chԭ����Ϊ�գ���ֱ���˳�
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

int get_line()
{
	return line;
}

int get_chars()
{
	return nchars;
}
