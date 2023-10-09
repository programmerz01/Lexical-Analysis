#include "value.h"
#include "output.h"
#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>

static FILE* error_file;
static FILE* token_file;
static int tokens, errors;
static int ids, key_words, ints, floats, strings, chars, operators, seperators;

void init_output(int is_token_to_stdin, int is_error_to_stdin)
{
	if (is_error_to_stdin) {
		error_file = stderr;
	}
	else {
		error_file = fopen("errors.txt", "w");
	}
	if (is_token_to_stdin) {
		token_file = stdout;
	}
	else {
		token_file = fopen("tokens.txt", "w");
	}
	

	if (error_file == NULL || token_file == NULL) {
		printf("Can't open output file\n");
		exit(1);
	}

	fprintf(token_file, "  Lexical Analysis\t\nType\t\t\ttoken\n");
	fprintf(error_file, "\terrors in Lexical Analysis\n");
}

//输出错误行及错误信息
void error_output(const char* token, const char* info)
{
	int line = get_line();

	errors++;
	fprintf(error_file, "Error in line %d:\n%s : %s\n", line, token, info);
}

//输出识别出的一个记号及其类型
void token_output(const char* token, int type)
{
	switch (type) {
	case ID:
		fprintf(token_file, "Identifier  \t\t\%s\n", token);
		ids++;
		break;
	case KEY_WORD:
		fprintf(token_file, "Key word    \t\t%s\n", token);
		key_words++;
		break;
	case TYPE_INT:
		fprintf(token_file, "Type int    \t\t%s\n", token);
		ints++;
		break;
	case TYPE_FLOAT:
		fprintf(token_file, "Type float  \t\t%s\n", token);
		floats++;
		break;
	case OPERATOR:
		fprintf(token_file, "Operator    \t\t%s\n", token);
		operators++;
		break;
	case TYPE_STRING:
		fprintf(token_file, "Type str    \t\t%s\n", token);
		strings++;
		break;
	case TYPE_CHAR:
		fprintf(token_file, "Type char   \t\t%s\n", token);
		chars++;
		break;
	case SEPERATOR:
		fprintf(token_file, "Seperator   \t\t%s\n", token);
		seperators++;
		break;
	default:
		fprintf(token_file, "Unrecognizd tokens:%s\n", token);
		break;
	}
	tokens++;
}

void end_output()
{
	fprintf(token_file, "\n\tEnd lexical Analysis\nChars: %d\nlines: %d\ntokens: %d\n", get_chars(), get_line(), tokens);
	fprintf(token_file, "\n\tStatistical data for each type:\n");
	fprintf(token_file, "Identifiers:%d\nkey words:%d\n", ids, key_words);
	fprintf(token_file, "Ints:%d\nFloats:%d\n", ints, floats);
	fprintf(token_file, "Chars:%d\nStrings:%d\n", chars, strings);
	fprintf(token_file, "Operators:%d\nSeperators:%d\n", operators, seperators);

	if(error_file!=stderr)
		fclose(error_file);
	if (token_file != stdin)
		fclose(token_file);

}