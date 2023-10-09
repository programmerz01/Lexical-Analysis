#ifndef _BUFFER_H_
#define _BUFFER_H_
#include "value.h"

typedef struct struct_buffer buffer;

struct struct_buffer {
	char* buf;
	int size;
	int p;	//ָ��λ�ã�ָ��ǰ�ַ�ĩβ
	int forward; // ָ�룬ָ��ǰ���ڶ�ȡ�Ǻŵ�ĩβ
};

/*
	�ʷ�������ʹ�õĻ���������
*/

//�ַ�������������ͷ�
void b_init(buffer* buf, const char* str);

// �ƶ�forwardָ�룬��ȡbuf����һ���ַ�����Ҫʱ��仺����
char get_char(buffer *buf);

// �����ո��Ʊ�������з�����ȡ��һ���ǿ��ַ�
char get_nbc(buffer *buf, char ch);

// forward��buf�˻�һ���ַ�
char retract(buffer *buf);

// ��chƴ����token��
void cat(buffer *token, char ch);

//���ַ�����������գ������´�����
void clear_buf(buffer *buf);

//��ȡ��ǰ����
int get_line();

//��ȡ��ǰ����
int get_chars();

#endif // !_BUFFER_H_

