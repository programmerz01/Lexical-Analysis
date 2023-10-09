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
	ʶ���ÿ�����ʷ��ţ����ԡ��Ǻš�����ʽ���ÿ�����ʷ���

	�Ǻţ�enum{...}����Ҫ���ԣ���Ԫʽ���������������������ؼ��֡�
					 ����Ҫ����ʶ����������
		
*/

static Boolean is_letter(char c);
static Boolean is_digit(char c);
static Boolean is_key(const char* str);
static Boolean is_excape(char ch);
static Boolean is_seperator(char ch);
static Boolean is_operator(char ch);



//�����������
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
            //��ʼ״̬��Ѱ����Ҫʶ��ĵ�һ���ַ���������Ӧ��ʶ�����
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
                //�ָ�����ֱ������Ǻ�
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
            //ʶ���ʶ��
        case 1:
            cat(&token, ch);
            ch = get_char(str, &buf);

            //�ַ���ʶ����������д���ʶ��������ȷ�Ǻ�
            if (!(is_letter(ch) || is_digit(ch))) {
                //�˻�
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
            // 2-7 ʶ������
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
                //default:�ǺŶ�ȡ��ϣ����
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
                //default:�ǺŶ�ȡ��ϣ����
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
            // 8-11���� /��ע�ͻ������
        case 8:
            cat(&token, ch);
            ch = get_char(str, &buf);
            //����ע��
            if (ch == '*') {
                state = 9;
                break;
            }
            else if (ch == '/') {
                state = 11;
                break;
            }
            //�����
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
            //�Թ�ע��
            ch = get_char(str, &buf);
            while (ch != '\n') {
                ch = get_char(str, &buf);
            }

            state = 0;
            clear_buf(&token);
            buf.p = buf.forward;
            break;
            // 12 ���������
        case 12:
            //���⴦��~�����������������=���Ϊ�����
            if (ch == '~') {
                cat(&token, ch);
                token_output(token.buf, OPERATOR);
                clear_buf(&token);
                break;
            }

            cat(&token, ch);
            ch = get_char(str, &buf);

            //������λ�����
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
            // ��λ�������>>=��<<=
            else if (token.buf[token.p - 1] == '>' && ch == '>' ||
                token.buf[token.p - 1] == '<' && ch == '<') {
                cat(&token, ch);
                ch = get_char(str, &buf);
                //ƥ��ɹ�
                if (ch == '=') {
                    cat(&token, ch);
                }
                else {
                    retract(&buf);
                }
                token_output(token.buf, OPERATOR);
            }
            //�����������������ӵ������
            else if (is_operator(ch)) {
                cat(&token, ch); error_output(token.buf, """Invalid Operators");
            }
            //�������������λ�������
            else {
                retract(&buf);
                token_output(token.buf, OPERATOR);
            }

            clear_buf(&token);
            state = 0;
            buf.p = buf.forward;
            break;
            // 13-16 ����char��
        case 13:
            cat(&token, ch);
            ch = get_char(str, &buf);

            //�����ַ�
            if (is_digit(ch) || is_letter(ch)
                || is_seperator(ch) || is_operator(ch)
                || ch == '@' || ch == ' ') {
                state = 14;
            }
            // ת���ַ�
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
            //ת���ַ�
            cat(&token, ch);
            ch = get_char(str, &buf);

            // �����������������������
            if (is_excape(ch));
            else {
                cat(&token, ch); error_output(token.buf, "Invalid escape character");
            }
            state = 14;

            // 16-17 string
        case 16:
            cat(&token, ch);
            ch = get_char(str, &buf);
            //�����ַ�
            if (is_digit(ch) || is_letter(ch)
                || is_seperator(ch) || is_operator(ch)
                || ch == '@' || ch == ' ') {
                state = 16;
            }
            // ת���ַ�
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
            //ת���ַ�
            cat(&token, ch);
            ch = get_char(str, &buf);

            // �����������������������
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
