#ifndef _OUTPUT_H_
#define _OUTPUT_H_


/*
	词法分析的输出程序
*/

//打印统计信息并关闭输出文件
void end_output();

//初始化输出文件
void init_output(int is_token_to_stdin,int is_error_to_stdin);

//输出错误行及错误信息
void error_output(const char* token, const char* info);

//输出识别出的一个记号及其类型
void token_output(const char* token, int type);


#endif // !_OUTPUT_H_



