#include <remocon.h>

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

  free(button);
  fclose(file);
  return 0;
}

