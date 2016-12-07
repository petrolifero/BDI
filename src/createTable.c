#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TABLEALREADYEXISTS 1
#define PROBLEMSTOOPENFILE 2
#define MULTIPLEKEY 3
#define MULTIPLEORD 4
#define KEYORORDBUTNULL 5

typedef enum
{
    INT,STRING
} Type;

typedef struct
{
    char nameAtribute[501];
    Type type;
    bool notNull;
    bool ord;
    bool key;
} tableCTL;

typedef struct l
{
    tableCTL t;
    struct l *prox;
}LinkedList;

LinkedList* readAtributes(FILE* sql)
{
    LinkedList* aux;
    char type[201];
    unsigned int i;
    char receive[31];
    char final;
    memset(receive,0,31);
    aux=malloc(sizeof(LinkedList));
    fscanf(sql,"%s %[^, )]", aux->t.nameAtribute, type);
    for(i=0; i<strlen(aux->t.nameAtribute); i++)
    {
        aux->t.nameAtribute[i]=toupper((int)aux->t.nameAtribute[i]);
    }
    aux->t.type=toupper(type[0])=='S'?STRING:INT;
    fscanf(sql,"%[^,)]%c",receive,&final);
    for(i=0; i<strlen(receive); i++)
    {
	receive[i]=toupper(receive[i]);
    }
    aux->t.notNull=strstr(receive,"NN")==NULL;
    aux->t.key=strstr(receive,"KEY")==NULL;
    aux->t.ord=strstr(receive,"ORD")==NULL;
    if(final==')')
    {
	aux->prox=NULL;
    }
    else
    {
        char a;
	next:
	fscanf(sql,"%c", &a);
	if(a!=' ')
	{
		fseek(sql,-1,SEEK_CUR);
		aux->prox=readAtributes(sql);
	}
        else
	{
		goto next;
	}
    }
    return aux;
}

int degree(LinkedList* l)
{
	int len=0;
	while(l)
	{
		len++;
		l=l->prox;
	}
	return len;
}

int howMuchKeys(LinkedList* l)
{
	int count=0;
	while(l)
	{
		if(l->t.key)
			count++;
		l=l->prox;
	}
	return count;
}

void freeList(LinkedList* l)
{
	if(!l)
	{
		freeList(l->prox);
		free(l);
	}
}

int howMuchOrd(LinkedList* l)
{
        int count=0;
        while(l)
        {
                if(l->t.ord)
                        count++;
                l=l->prox;
        }
        return count;
}

int keyOrOrdButNULL(LinkedList* l)
{
	while(l)
        {
                if((l->t.ord || l->t.key) && !l->t.notNull)
                        return 1;
                l=l->prox;
        }
	return 0;
}

void createEmptyFile(char* str)
{
	FILE* fp=fopen(str,"w");
	fclose(fp);
}


void createTable(FILE* sql)
{
    char* nameTable;
    char* nameFile;
    FILE* ctl;
    LinkedList *atributes;
    LinkedList *aux;
    nameTable=(char*)calloc(sizeof(char),501); //prepare to table name up to 500
    nameFile=(char*)calloc(sizeof(char), 504); //table name + ".ctl"
    rewind(sql);
    fscanf(sql,"%*13s"); /*ignore "CREATE TABLE "*/
    fscanf(sql,"%s( ", nameTable); /*ignore the '(' and the space*/
    strcpy(nameFile,nameTable);
    strcat(nameFile,".ctl");
    ctl=fopen(nameFile, "r");
    if(ctl) /*if file exists*/
    {
        fclose(ctl);
        free(nameFile);
        free(nameTable);
    }
    ctl=fopen(nameFile, "w");
    if(!ctl)
    {
        free(nameFile);
        free(nameTable);
    }
    atributes=readAtributes(sql);
    if(howMuchKeys(atributes) != 1)
    {
        freeList(atributes);
        free(nameFile);
        free(nameTable);
        fclose(ctl);
    }
    if(howMuchOrd(atributes) != 1)
    {
        freeList(atributes);
        free(nameFile);
        free(nameTable);
        fclose(ctl);
    }
    if(keyOrOrdButNULL(atributes))
    {
        freeList(atributes);
        free(nameFile);
        free(nameTable);
        fclose(ctl);
    }
    strcpy(nameFile, nameTable);
    strcat(nameFile, ".dad");
    createEmptyFile(nameFile);
    free(nameTable);
    free(nameFile);
    fprintf(ctl,"%d,0\n", degree(atributes)); //numberOfFields, numberOfTuples
    for(aux=atributes; aux!=NULL; aux=aux->prox)
    {
        fprintf(ctl, "%s,%c", aux->t.nameAtribute, aux->t.type==INT?'I':'C');
        if(aux->t.notNull)
        {
            fprintf(ctl, ",nn");
        }
        if(aux->t.ord)
        {
            fprintf(ctl, ",ord");
        }
        if(aux->t.key)
        {
            fprintf(ctl, ",chv");
        }
        fprintf(ctl, "\n");
    }
    freeList(atributes);
    fclose(ctl);
}
