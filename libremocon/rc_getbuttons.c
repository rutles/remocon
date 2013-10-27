#include <remocon.h>

int rc_getbuttons(FILE *file, char ***pbutton){
  char *s;
  int count;

  // count buttons
  count = 0;
  fseek(file, 0, SEEK_SET);
  while((s = rc_gettoken(file)) != NULL){
    if(strcmp(s, "button") == 0)
      count++;
  }

  // read buttons
  *pbutton = (char **)calloc(count, sizeof(char *));
  count = 0;
  fseek(file, 0, SEEK_SET);
  while((s = rc_gettoken(file)) != NULL){
    if(strcmp(s, "button") == 0){
      s = rc_gettoken(file);
      (*pbutton)[count++] = strdup(s);
    }
  }

  return count;
}

/*
int main(int argc, char **argv){
  FILE *file;
  char buf[80];
  int i;
  struct opspec op;
  char **button;
  int count;

  rc_getopt(argc, argv, &op);
  sprintf(buf, "%s/analyzer.conf", op.dir);
  file = fopen(buf, "r");
  if(file == NULL){
    fprintf(stderr, "fail to open %s. %s\n", buf, strerror(errno));
    return -1;
  }

  count = rc_getbuttons(file, &button);
  printf("count = %d\n", count);
  for(i = 0; i < count; i++){
    printf("button%d = %s\n", i, button[i]);
  }

  fclose(file);
  return 0;
}
*/
