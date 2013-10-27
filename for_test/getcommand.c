#include <remocon.h>

int main(int argc, char **argv){
  FILE *file;
  char buf[80];
  struct opspec op;
  char *command[2];
  int ret;

  rc_getopt(argc, argv, &op);
  sprintf(buf, "%s/received.conf", op.dir);
  file = fopen(buf, "r");
  if(file == NULL){
    fprintf(stderr, "fail to open %s. %s\n", buf, strerror(errno));
    return -1;
  }

  command[0] = rc_getcommand(file, "00");
  printf("%s\n", command[0]);//strdup as necessary

  command[1] = rc_getcommand(file, "01");
  printf("%s\n", command[1]);//strdup as necessary

  fclose(file);
  return 0;
}
