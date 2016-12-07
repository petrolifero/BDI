#include <stdio.h>
#include <string.h>
#include "type.h"
#include <stdlib.h>

typedef struct Ctl
{
	Type t;
	int notNull;
}Ctl;

typedef struct Input
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

typedef struct ListInput
{
	Input i;
	struct ListInput* prox;
}ListInput;

typedef struct ListCtl
{
	Ctl ctl;
	struct ListCtl* prox;
}ListCtl;


int lenc(ListCtl* l )
{
		int count=0;
		while(l)
		{
				count++;
				l=l->prox;
		}
		return count;
}

int leni(ListInput* l ) 
{
        int count=0;
        while(l)
        {
                count++;
                l=l->prox;
        }
        return count;
}

ListInput* readInput(FILE* sql)
{
	fseek(sql,1,SEEK_CUR);	
}

ListCtl* readCtl(FILE* fp)
{
	
}

int tamFile(FILE* fp)
{
	int actual;
	int len;
	actual=ftell(fp);
	fseek(fp,0,SEEK_END);
	len=ftell(fp);
	fseek(fp,actual,SEEK_SET);
	return len;
}

void writeLine(FILE* fp, ListInput* input)
{
}


void writeOnFile(ListInput* input, char* tableName)
{
		char* fileName=(char*)calloc(strlen(tableName)+1+4,sizeof(char));
		FILE* fp;
		strcpy(fileName, tableName);
		strcat(fileName, ".dad");
		fp=fopen(fileName, "w+");
		if(!fp) exit(1);
		if(tamFile(fp)==0)
		{
				writeLine(fp,input);
		}
		
}



void sql_insert(FILE* sql)
{
    char tableName[301], tablectl[304];
	FILE* fp;
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
	while(c!='\n')
	{
		fscanf(fp,"%c", &c);
	}

	input=readInput(sql);
	ctl=readCtl(fp);

	if(leni(input)!=lenc(ctl))
		return;
	for(auxctl=ctl,auxinput=input; auxctl && auxinput; auxctl=auxctl->prox,auxinput=auxinput->prox)
	{
		if((auxctl->ctl.notNull && auxinput->i.t==NULLVALUE) || (auxinput->i.t!=NULLVALUE && auxinput->i.t!=auxctl->ctl.t)  )
				return ;
	}
	writeOnFile(input,tableName);
}
