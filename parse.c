#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "buffer.h"
#include "parse.h"
#include "output.h"



static char keywords[32][10] = { "auto" , "break" , "case" , " char" , 
" const" , " continue","do" , "double" , "else" , "enum" , "extern" , 
"float" , "for" , "goto","if", "int" , "long" , "register" , "return" , 
"shorti" , "signed", "sizeof" , "static" , "struct" , "switch" , "default", 
"typedef" , "union", "unsigned" , "void" , "volatile" , "while" };

/*
	识别出每个单词符号，并以“记号”的形式输出每个单词符号

	记号：enum{...}，需要属性，二元式：运算符及界符、常数、关键字、
					 不需要：标识符（？）、
		
*/

static Boolean is_letter(char c);
static Boolean is_digit(char c);
static Boolean is_key(const char* str);
static Boolean is_excape(char ch);
static Boolean is_seperator(char ch);
static Boolean is_operator(char ch);



//分析主体程序
void parse(const char* str)
{
	int state = 0;
    char ch;
    int line = 0;

	buffer buf = {NULL,BUF_SIZE,0,0};
	buffer token = {NULL,TOKEN_SIZE,0,0};

	buf.buf = (char*)malloc(buf.size);
	token.buf = (char*)malloc(token.size);
    b_init(&buf, str);
    memset(token.buf, 0, token.size);
    init_output(0, 0);


    do {
        switch (state) {
            //初始状态，寻找需要识别的第一个字符，进入相应的识别过程
        case 0:
            ch = get_char(str, &buf);
            ch = get_nbc(str, &buf, ch);

            if (is_digit(ch)) {
                state = 2;
            }
            else if (is_letter(ch)) {
                state = 1;
            }
            else if (is_seperator(ch)) {
                //分隔符，直接输出记号
                cat(&token, ch);
                token_output(token.buf, SEPERATOR);
                clear_buf(&token);
            }
            else if (ch == '/') {
                state = 8;
            }
            else if (is_operator(ch)) {
                state = 12;
            }
            else if (ch == '\'') {
                state = 13;
            }
            else if (ch == '\"') {
                state = 16;
            }
            else if (ch == '\0') {}
            else {
                cat(&token, ch); error_output(token.buf, "Unrecognized symbol");
            }

            break;
            //识别标识符
        case 1:
            cat(&token, ch);
            ch = get_char(str, &buf);

            //字符串识别结束，进行错误识别或输出正确记号
            if (!(is_letter(ch) || is_digit(ch))) {
                //退回
                retract(&buf);

                if (is_key(token.buf)) {
                    token_output(token.buf, KEY_WORD);
                }
                else {
                    token_output(token.buf, ID);
                }
                state = 0;
                clear_buf(&token);
            }
            break;
            // 2-7 识别数字
        case 2:
            cat(&token, ch);
            ch = get_char(str, &buf);

            if (is_digit(ch)) {
                state = 2;
            }
            else if (ch == '.') {
                state = 3;
            }
            else if (ch == 'e') {
                state = 5;
            }
            else {
                //default:记号读取完毕，输出
                retract(&buf);
                state = 0;
                buf.p = buf.forward;
                token_output(token.buf, TYPE_INT);
                clear_buf(&token);
            }

            break;

        case 3:
            cat(&token, ch);
            ch = get_char(str, &buf);

            if (is_digit(ch)) {
                state = 4;
            }
            else {
                retract(&buf);
                cat(&token, ch); error_output(token.buf, "Invalid Number");
                clear_buf(&token);
                state = 0;
                buf.p = buf.forward;
            }
            break;
        case 4:
            cat(&token, ch);
            ch = get_char(str, &buf);

            if (is_digit(ch)) {
                state = 4;
            }
            else if (ch == 'e') {
                state = 5;
            }
            else {
                //default:记号读取完毕，输出
                retract(&buf);
                state = 0;
                buf.p = buf.forward;
                token_output(token.buf, TYPE_FLOAT);
                clear_buf(&token);
            }
            break;
        case 5:
            cat(&token, ch);
            ch = get_char(str, &buf);

            if (is_digit(ch)) {
                state = 7;
            }
            else if (ch == '+' || ch == '-') {
                state = 6;
            }
            else {
                retract(&buf);
                cat(&token, ch); error_output(token.buf, "Invalid Number");
                clear_buf(&token);
                buf.p = buf.forward;
                state = 0;
            }
            break;
        case 6:
            cat(&token, ch);
            ch = get_char(str, &buf);

            if (is_digit(ch)) {
                state = 7;
            }
            else {
                //retract(&buf);
                cat(&token, ch); error_output(token.buf, "Invalid Number");
                clear_buf(&token);
                state = 0;
                buf.p = buf.forward;
            }
            break;
        case 7:
            cat(&token, ch);
            ch = get_char(str, &buf);

            if (is_digit(ch)) {
                state = 7;
            }
            else {
                retract(&buf);
                token_output(token.buf, TYPE_FLOAT);
                state = 0;
                buf.p = buf.forward;
                clear_buf(&token);
            }
            break;
            // 8-11处理 /：注释或运算符
        case 8:
            cat(&token, ch);
            ch = get_char(str, &buf);
            //进入注释
            if (ch == '*') {
                state = 9;
                break;
            }
            else if (ch == '/') {
                state = 11;
                break;
            }
            //运算符
            if (ch == '=') {
                cat(&token, ch);
            }
            else {
                retract(&buf);
            }
            token_output(token.buf, OPERATOR);
            clear_buf(&token);
            state = 0;
            buf.p = buf.forward;
            break;
        case 9:
            ch = get_char(str, &buf);
            while (ch != '*' && ch != 0) {
                ch = get_char(str, &buf);
            }
            if (ch == 0) {
                cat(&token, ch); 
                error_output(token.buf, "Comment not closed");
            }
            else {
                state = 10;
            }
            break;
        case 10:
            ch = get_char(str, &buf);
            if (ch == '/') {
                state = 0;
                clear_buf(&token);
                buf.p = buf.forward;
            }
            else {
                state = 9;
            }
            break;
        case 11:
            //略过注释
            ch = get_char(str, &buf);
            while (ch != '\n') {
                ch = get_char(str, &buf);
            }

            state = 0;
            clear_buf(&token);
            buf.p = buf.forward;
            break;
            // 12 处理运算符
        case 12:
            //特殊处理~，其余运算符均可与=结合为运算符
            if (ch == '~') {
                cat(&token, ch);
                token_output(token.buf, OPERATOR);
                clear_buf(&token);
                break;
            }

            cat(&token, ch);
            ch = get_char(str, &buf);

            //处理两位运算符
            if (ch == '=' ||
                token.buf[token.p - 1] == '+' && ch == '+' ||
                token.buf[token.p - 1] == '-' && ch == '-' ||
                token.buf[token.p - 1] == '-' && ch == '>' ||
                token.buf[token.p - 1] == '&' && ch == '&' ||
                token.buf[token.p - 1] == '|' && ch == '|'
                ) {
                cat(&token, ch);
                token_output(token.buf, OPERATOR);
            }
            // 三位运算符：>>=、<<=
            else if (token.buf[token.p - 1] == '>' && ch == '>' ||
                token.buf[token.p - 1] == '<' && ch == '<') {
                cat(&token, ch);
                ch = get_char(str, &buf);
                //匹配成功
                if (ch == '=') {
                    cat(&token, ch);
                }
                else {
                    retract(&buf);
                }
                token_output(token.buf, OPERATOR);
            }
            //错误处理：两个不能连接的运算符
            else if (is_operator(ch)) {
                cat(&token, ch); error_output(token.buf, """Invalid Operators");
            }
            //其余情况，按单位运算符计
            else {
                retract(&buf);
                token_output(token.buf, OPERATOR);
            }

            clear_buf(&token);
            state = 0;
            buf.p = buf.forward;
            break;
            // 13-16 处理char型
        case 13:
            cat(&token, ch);
            ch = get_char(str, &buf);

            //正常字符
            if (is_digit(ch) || is_letter(ch)
                || is_seperator(ch) || is_operator(ch)
                || ch == '@' || ch == ' ') {
                state = 14;
            }
            // 转义字符
            else if (ch == '\\') {
                state = 15;
            }
            else {
                cat(&token, ch); error_output(token.buf, "Unrecognized Charactor");
            }

            break;
        case 14:
            cat(&token, ch);
            ch = get_char(str, &buf);

            if (ch == '\'') {
                cat(&token, ch);
                token_output(token.buf, TYPE_CHAR);
            }
            else {
                cat(&token, ch); error_output(token.buf, "char not closed");
            }
            state = 0;
            clear_buf(&token);
            buf.p = buf.forward;
            break;
        case 15:
            //转义字符
            cat(&token, ch);
            ch = get_char(str, &buf);

            // 错误情况输出后按正常情况处理
            if (is_excape(ch));
            else {
                cat(&token, ch); error_output(token.buf, "Invalid escape character");
            }
            state = 14;

            // 16-17 string
        case 16:
            cat(&token, ch);
            ch = get_char(str, &buf);
            //正常字符
            if (is_digit(ch) || is_letter(ch)
                || is_seperator(ch) || is_operator(ch)
                || ch == '@' || ch == ' ') {
                state = 16;
            }
            // 转义字符
            else if (ch == '\\') {
                state = 17;
            }
            else if (ch == '\"') {
                cat(&token, ch);
                token_output(token.buf, TYPE_STRING);
                state = 0;
                clear_buf(&token);
                buf.p = buf.forward;
            }
            else if (ch == '\0') {
                cat(&token, ch); error_output(token.buf, "String not closed");
            }
            else {
                cat(&token, ch); error_output(token.buf, "Unrecognized Charactor");
            }
            break;
        case 17:
            //转义字符
            cat(&token, ch);
            ch = get_char(str, &buf);

            // 错误情况输出后按正常情况处理
            if (is_excape(ch));
            else {
                cat(&token, ch); error_output(token.buf, "Invalid escape character");
            }
            state = 16;
            break;
        default:
            cat(&token, ch); error_output(token.buf, "Enter wrong state");
            exit(17);
        }
         //line = get_line();



    } while (ch != 0);

    end_output();





	free(buf.buf);
	free(token.buf);

	return 0;
}

static Boolean is_letter(char c) {
    if ((c >= 'a' && c < 'z') || (c >= 'A' && c < 'Z') || c == '_') {
        return true;
    }
    else {
        return false;
    }
}

static Boolean is_digit(char c) {
    if (c >= '0' && c < '9') {
        return true;
    }
    else {
        return false;
    }
}

static Boolean is_key(const char* str)
{
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
    {
        if (!strcmp(keywords[i], str)) {
            return true;
        }
    }
    return false;
}
static Boolean is_excape(char ch)
{
    if (ch == '\'' || ch == '\"' || ch == '\\' ||
        ch == 'r' || ch == 'n' || ch == 't' ||
        ch == 'b' || ch == 'v' || ch == 'a' ||
        ch == '0' || ch == 'f' || ch == '?') {
        return true;
    }
    return false;
}
static Boolean is_seperator(char ch)
{
    if (ch == '(' || ch == '{' || ch == '.' ||
        ch == ')' || ch == '}' || ch == ']' ||
        ch == '[' || ch == ';' || ch == ','
          ) {
        return true;
    }
    return false;
}
static Boolean is_operator(char ch)
{
    if (ch == '=' || ch == '+' || ch == '-' ||
        ch == '*' || ch == '>' || ch == '~' ||
        ch == '<' || ch == '!' || ch == '|' ||
        ch == '&' || ch == '^') {
        return true;
    }
    return false;
}
