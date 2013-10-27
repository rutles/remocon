#include <remocon.h>

// example
int main(int argc, char **argv){
  FILE *file;
  char buf[80];
  int ret;
  struct opspec op;
  struct rcspec rc;

  rc_getopt(argc, argv, &op);
  sprintf(buf, "%s/analyzer.conf", op.dir);
  file = fopen(buf, "r");
  if(file == NULL){
    fprintf(stderr, "fail to open %s. %s\n", buf, strerror(errno));
    return -1;
  }

  rc_getdata(file, "00", &rc);
  ret = rc_shot(&rc);
  return ret;
}
