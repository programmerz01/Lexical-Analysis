#define  _CRT_SECURE_NO_WARNINGS

#include "parse.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
	
	char program[2048];
	char filename = "program2.txt";
	memset(program, 0, sizeof(program));
	FILE* fp = fopen("./program.txt","r");
	int i = 0;

	if (fp == NULL) {
		printf("Can't open file:%s",filename);
		exit(1);
	}

	while (!feof(fp)){
		program[i++] = fgetc(fp);
	}
	//printf("%d", program[i - 1]);
	//printf("%s\n", program);
	parse(program);
	
	fclose(fp);

	return 0;
}