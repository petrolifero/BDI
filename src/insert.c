#include <stdio.h>
#include <string.h>
#include "type.h"


typedef struct
{
	Type t;
	int notNull;
}Ctl;

typedef struct
{
	char value[301];
	Type t;
}Input;



int numberOfFields(FILE* fp)
{
	int x;
	rewind(fp);
	fscanf(fp,"%d", &x);
	return x;
}

typedef struct
{
	Input i;
	struct ListInput* prox;
}ListInput;

typedef struct
{
	Ctl ctl;
	struct ListCtl* prox;
}ListCtl;



void insert(FILE* sql)
{
        char tableName[301], tablectl[304];
	File* fp;
	int aux;
	int card;
	ListInput* input,*auxinput;
	ListCtl* ctl,*auxctl;
	char c='a';
	fseek(sql,12,SEEK_SET); /*INSERT INTO */
	fscanf(sql,"%s", tableName);
	strcpy(tablectl,tableName);
	strcat(tablectl,".ctl");
	fseek(sql,8,SEEK_CUR); /* VALUES */
/*	fseek(sql,1,SEEK_CUR); ( */
	fp=fopen(tablectl, "r");
	if(!fp)
		return;
	rewind(fp);
	while(a!='\n')
	{
		fscanf(fp,"%c", &a);
	}

	input=readInput(sql);
	ctl=readCtl(fp);

	if(len(input)!=len(ctl))
		return;
	for(auxctl=ctl,auxinput=input; auxctl && auxinput; auxctl=auxctl->prox,auxinput=auxinput->prox);
	if(auxctl!=auxinput) return;
	



}
