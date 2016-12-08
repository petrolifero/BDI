#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "type.h"


#define TABLEALREADYEXISTS 1
#define PROBLEMSTOOPENFILE 2
#define MULTIPLEKEY 3
#define MULTIPLEORD 4
#define KEYORORDBUTNULL 5


/*enum for type of a atribute*/

/*struct to atribute*/
typedef struct
{
    char nameAtribute[501];
    Type type;
    bool notNull;
    bool ord;
    bool key;
} tableCTL;

/*list of atributes*/
typedef struct l
{
    tableCTL t;
    struct l *prox;
}LinkedList;


/*read atributes from sql and return a list of*/
LinkedList* readAtributes(FILE* sql)
{
    LinkedList* aux;
    char type[201];
    unsigned int i;
    char receive[31];
    char final;
    memset(receive,0,31);
    aux=(LinkedList*)malloc(sizeof(LinkedList));
    fscanf(sql,"%s %[^, )]", aux->t.nameAtribute, type);
    for(i=0; i<strlen(aux->t.nameAtribute); i++)
    {
        aux->t.nameAtribute[i]=toupper(aux->t.nameAtribute[i]);
    }
    aux->t.type=toupper(type[0])=='S'?STRING:INT;
	fscanf(sql,"%[^,)]%c",receive,&final);
	if(aux->t.nameAtribute[0]=='L')
		fprintf(stderr, "%d %s %c\n", __LINE__, receive, final);
	if(strlen(receive)==0)
	{
			char c=0;
			fscanf(sql,"%c", &c);
			if(c==')')
			{
				aux->prox=NULL;
				return aux;
			}
	}
	if(receive[0]==' ')
	{
			int i;
			int len=strlen(receive);
			for(i=1; i<len; i++)
			{
					receive[i-1]=receive[i];
			}
	}
    for(i=0; i<strlen(receive); i++)
    {
		receive[i]=toupper(receive[i]);
    }
    aux->t.notNull=strstr(receive,"NN")!=NULL;
    aux->t.key=strstr(receive,"KEY")!=NULL;
    aux->t.ord=strstr(receive,"ORD")!=NULL;
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

/*calcule number of atributes*/
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

/*return number of keys*/
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

/*free list*/
void freeList(LinkedList* l)
{
	if(!l)
	{
		freeList(l->prox);
		free(l);
	}
}

/*return number of ord atributes*/
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

/*return if exist some atribute key or ord but set to null*/
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

/*create a empty file*/
void createEmptyFile(char* str)
{
	FILE* fp=fopen(str,"w");
	fclose(fp);
}


/*main function to read sql and create ctl*/
void sql_createTable(FILE* sql)
{
    char* nameTable;
    char* nameFile;
    FILE* ctl;
    LinkedList *atributes;
    LinkedList *aux;
	int i;
    nameTable=(char*)calloc(sizeof(char),501); //prepare to table name up to 500
    nameFile=(char*)calloc(sizeof(char), 504); //table name + ".ctl"
    rewind(sql);
//    fscanf(sql,"%*13[^ ]"); /*ignore "CREATE TABLE "*/
	for(i=0; i<13; i++)
	{
			fscanf(sql,"%*c");
	}
    fscanf(sql,"%[^(]( ", nameTable); /*ignore the '(' and the space*/
    strcpy(nameFile,nameTable);
    strcat(nameFile,".ctl");
    ctl=fopen(nameFile, "r");
    if(ctl) /*if file exists*/
    {
        fclose(ctl);
        free(nameFile);
        free(nameTable);
		return;
    }
    ctl=fopen(nameFile, "w");
    if(!ctl)
    {
        free(nameFile);
        free(nameTable);
		return;
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

int main(int arc, char* arv[])
{
	FILE* fp;
	if(arc!=2) 
	{
			fprintf(stderr,"FORMAT : %s file\n", arv[0]);
			return 1;
	}
	fp=fopen(arv[1], "r");
	if(!fp)
	{
		fprintf(stderr,"ERRO NA ABERTURA DO ARQUIVO\n");
		return 2;
	}
	sql_createTable(fp);
	return 0;
}
