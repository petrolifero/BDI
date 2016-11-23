#include <stdio.h>

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
  switch(initial)
  {
    case 'S':
      select(fp);
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
