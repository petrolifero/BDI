#include <stdio.h>
#include "select.cpp"
#include "createTable.c"

int main(int argc, char* argv[])
{
  FILE *fp;
  char initial;
  fp=fopen(argv[1], "r");
  if(!fp)
  {
    fprintf(stderr, "ERROR ON OPENING SQL FILE\n");
    return 1;
  }
  fscanf(fp,"%c", &initial);
  rewind(fp);
  while(1)
  {
    char c = fgetc(fp);
    if(feof(fp))
	break;
    if(c=='\n')
	c=' ';
    fseek(fp,-1,SEEK_CUR);
    fprintf(fp,"%c",c);
  }
  switch(initial)
  {
    case 'S':
      sql_select(fp);
      break;
    case 'I':
      insert(fp);
      break;
    case 'C':
      createTable(fp);
      break;
  }
  return 0;
}
