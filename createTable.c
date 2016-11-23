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
    INT,STRING;
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
    int i;
    aux=malloc(sizeof(LinkedList));
    fscanf(sql,"%s %s", aux->t.nameAtribute, type);
    for(i=0; i<strlen(aux->t.nameAtribute); i++)
    {
        aux->t.nameAtribute[i]=toUpper(aux->t.nameAtribute[i]);
    }
    if(type[strlen(type)-2]==',')
    {
        aux->t.type=type[strlen(type)-3]=='G'?STRING:INT;
        aux->prox=readAtributes(sql);
    }
    else if(type[strlen(type)-2]==')')
    {
        aux->t.type=type[strlen(type)-3]=='G'?STRING:INT;
        aux->prox=NULL;
    }
    else
    {
        aux->t.type=type[strlen(type)-2]=='G'?STRING:INT;
        char other[5];
        int r;
        char comma;
        r=fscanf(sql, "%s", other);
        while(r==1)
        {
            if(other[0]=='N')
            {
                aux->t.notNull=true;
            }
            else if(other[0]=='K')
            {
                aux->t.key=true;
            }
            else
            {
                aux->t.ord=true;
            }
            if(other[3])
                r=fscanf(sql, "%s", other);

        }
    }
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
    fscanf(sql,"%s(%*c", nameTable); /*ignore the '(' and the newLine*/
    strcpy(nameFile,nameTable);
    strcat(nameFile,".ctl");
    ctl=fopen(nameFile, "r");
    if(ctl) /*if file exists*/
    {
        fclose(ctl);
        free(nameFile);
        free(nameTable);
        return TABLEALREADYEXISTS;
    }
    ctl=fopen(nameFile, "w");
    if(!ctl)
    {
        free(nameFile);
        free(nameTable);
        return PROBLEMSTOOPENFILE;
    }
    atributes=readAtributes(sql);
    if(howMuchKeys(atributes) != 1)
    {
        freeList(atributes);
        free(nameFile);
        free(nameTable);
        fclose(ctl);
        return MULTIPLEKEY;
    }
    if(howMuchOrd(atributes) != 1)
    {
        freeList(atributes);
        free(nameFile);
        free(nameTable);
        fclose(ctl);
        return MULTIPLEORD;
    }
    if(keyOrOrdButNULL(atributes))
    {
        freeList(atributes);
        free(nameFile);
        free(nameTable);
        fclose(ctl);
        return KEYORORDBUTNULL;
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
    return 0;
}
