#ifndef _OUTPUT_H_
#define _OUTPUT_H_


/*
	�ʷ��������������
*/

//��ӡͳ����Ϣ���ر�����ļ�
void end_output();

//��ʼ������ļ�
void init_output(int is_token_to_stdin,int is_error_to_stdin);

//��������м�������Ϣ
void error_output(const char* token, const char* info);

//���ʶ�����һ���Ǻż�������
void token_output(const char* token, int type);


#endif // !_OUTPUT_H_



