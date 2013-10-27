#include <remocon.h>

int main(int argc, char **argv){
  FILE *file;
  char buf[80];
  int ret;
  int i;
  int neq;
  struct opspec op;
  struct rcspec rc1[2], rc2;
  unsigned char code1[RC_SIGCNT], code2[RC_SIGCNT];

  rc_getopt(argc, argv, &op);
  sprintf(buf, "%s/analyzer.conf", op.dir);
  file = fopen(buf, "r");
  if(file == NULL){
    fprintf(stderr, "fail to open %s. %s\n", buf, strerror(errno));
    return -1;
  }

  rc_getdata(file, "00", &rc1[0]);
  rc_getdata(file, "01", &rc1[1]);

  rc2.code1 = code1;
  rc2.code2 = code2;

  while(1){
    ret = rc_analyze(op.port, &rc2);
    if(ret < 0)
      return -1;
    if(rc_speccmp(&rc1[0], &rc2) == 0){
      system("ls -l");
    } else
    if(rc_speccmp(&rc1[1], &rc2) == 0){
      system("ls -l");
    }
  }
  fclose(file);
  return 0;
}
